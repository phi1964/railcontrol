#include <algorithm>
#include <cstring>		//memset
#include <netinet/in.h>
#include <signal.h>
#include <sstream>
#include <sys/socket.h>
#include <sys/stat.h>
#include <thread>
#include <unistd.h>
#include <webserver/HtmlResponseNotFound.h>
#include <webserver/HtmlTagInputSliderLocoSpeed.h>
#include "datamodel/datamodel.h"
#include "railcontrol.h"
#include "util.h"
#include "webclient.h"
#include "webserver.h"
#include "webserver/HtmlTagButtonCancel.h"
#include "webserver/HtmlTagButtonCommand.h"
#include "webserver/HtmlTagButtonOK.h"
#include "webserver/HtmlTagButtonPopup.h"
#include "webserver/HtmlTagInputHidden.h"
#include "webserver/HtmlTagInputTextWithLabel.h"
#include "webserver/HtmlTagSelect.h"

using std::map;
using std::stoi;
using std::string;
using std::stringstream;
using std::thread;
using std::to_string;
using std::vector;
using datamodel::Loco;

namespace webserver {

	WebClient::WebClient(const unsigned int id, int socket, WebServer& webserver, Manager& m) :
		id(id),
		clientSocket(socket),
		run(false),
		server(webserver),
		clientThread(thread([this] {worker();})),
		manager(m),
		buttonID(0) {
	}

	WebClient::~WebClient() {
		run = false;
		clientThread.join();
	}

	// worker is the thread that handles client requests
	void WebClient::worker() {
		xlog("Executing webclient");
		run = true;

		char buffer_in[1024];
		memset(buffer_in, 0, sizeof(buffer_in));

		size_t pos = 0;
		string s;
		while(pos < sizeof(buffer_in) - 1 && s.find("\n\n") == string::npos) {
			pos += recv_timeout(clientSocket, buffer_in + pos, sizeof(buffer_in) - 1 - pos, 0);
			s = string(buffer_in);
			str_replace(s, string("\r\n"), string("\n"));
			str_replace(s, string("\r"), string("\n"));
		}
		vector<string> lines;
		str_split(s, string("\n"), lines);

		if (lines.size() < 1) {
			xlog("Invalid request");
			close(clientSocket);
			return;
		}
		string method;
		string uri;
		string protocol;
		map<string, string> arguments;
		map<string, string> headers;
		interpretClientRequest(lines, method, uri, protocol, arguments, headers);
		xlog("%s %s", method.c_str(), uri.c_str());

		// if method is not implemented
		if ((method.compare("GET") != 0) && (method.compare("HEAD") != 0)) {
			xlog("Method %s not implemented", method.c_str());
			const char* reply =
				"HTTP/1.0 501 Not implemented\r\n\r\n"
				"<!DOCTYPE html><html><head><title>501 Not implemented</title></head><body><p>Method not implemented</p></body></html>";
			send_timeout(clientSocket, reply, strlen(reply), 0);
			close(clientSocket);
			return;
		}

		/*
		for (auto argument : arguments) {
			xlog("Argument: %s=%s", argument.first.c_str(), argument.second.c_str());
		}
		for (auto header : headers) {
			xlog("Header: %s=%s", header.first.c_str(), header.second.c_str());
		}
		*/

		// handle requests
		if (arguments["cmd"].compare("quit") == 0) {
			simpleReply("Stopping Railcontrol");
			manager.booster(ControlTypeWebserver, BoosterStop);
			stopRailControlWebserver();
		}
		else if (arguments["cmd"].compare("on") == 0) {
			simpleReply("Turning booster on");
			manager.booster(ControlTypeWebserver, BoosterGo);
		}
		else if (arguments["cmd"].compare("off") == 0) {
			simpleReply("Turning booster off");
			manager.booster(ControlTypeWebserver, BoosterStop);
		}
		else if (arguments["cmd"].compare("loco") == 0) {
			printLoco(arguments);
		}
		else if (arguments["cmd"].compare("locospeed") == 0) {
			handleLocoSpeed(arguments);
		}
		else if (arguments["cmd"].compare("locodirection") == 0) {
			handleLocoDirection(arguments);
		}
		else if (arguments["cmd"].compare("locofunction") == 0) {
			handleLocoFunction(arguments);
		}
		else if (arguments["cmd"].compare("locoedit") == 0) {
			handleLocoEdit(arguments);
		}
		else if (arguments["cmd"].compare("locosave") == 0) {
			handleLocoSave(arguments);
		}
		else if (arguments["cmd"].compare("protocol") == 0) {
			handleProtocol(arguments);
		}
		else if (arguments["cmd"].compare("updater") == 0) {
			handleUpdater(headers);
		}
		else if (uri.compare("/") == 0) {
			printMainHTML();
		}
		else {
			deliverFile(uri);
		}

		xlog("Terminating webclient");
		close(clientSocket);
	}

	int WebClient::stop() {
		// inform thread to stop
		run = false;
		return 0;
	}

	void WebClient::interpretClientRequest(const vector<string>& lines, string& method, string& uri, string& protocol, map<string,string>& arguments, map<string,string>& headers) {
		if (lines.size()) {
			for (auto line : lines) {
				if (line.find("HTTP/1.") != string::npos) {
					vector<string> list;
					str_split(line, string(" "), list);
					if (list.size() == 3) {
						method = list[0];
						// transform method to uppercase
						std::transform(method.begin(), method.end(), method.begin(), ::toupper);
						// if method == HEAD set membervariable
						headOnly = false;
						if (method.compare("HEAD") == 0) {
							headOnly = true;
						}
						// set uri and protocol
						uri = list[1];
						protocol = list[2];
						// read GET-arguments from uri
						vector<string> uri_parts;
						str_split(uri, "?", uri_parts);
						if (uri_parts.size() == 2) {
							vector<string> argumentStrings;
							str_split(uri_parts[1], "&", argumentStrings);
							for (auto argument : argumentStrings) {
								vector<string> argumentParts;
								str_split(argument, "=", argumentParts);
								string argumentValue = argumentParts[1];
								// decode %20 and similar
								while (true) {
									size_t pos = argumentValue.find('%');
									if (pos == string::npos || pos + 3 > argumentValue.length()) {
										break;
									}
									char c1 = argumentValue[pos + 1];
									char c2 = argumentValue[pos + 2];
									if (c1 >= 'a') {
										c1 -= 'a' - 10;
									}
									else if (c1 >= 'A') {
										c1 -= 'A' - 10;
									}
									else if (c1 >= '0') {
										c1 -= '0';
									}
									if (c2 >= 'a') {
										c2 -= 'a' - 10;
									}
									else if (c2 >= 'A') {
										c2 -= 'A' - 10;
									}
									else if (c2 >= '0') {
										c2 -= '0';
									}
									char c = c1 * 16 + c2;
									argumentValue.replace(pos, 3, 1, c);
								}
								arguments[argumentParts[0]] = argumentValue;
							}
						}
					}
				}
				else {
					vector<string> list;
					str_split(line, string(": "), list);
					if (list.size() == 2) {
						headers[list[0]] = list[1];
					}
				}
			}
		}
	}

	void WebClient::deliverFile(const string& virtualFile) {
		stringstream ss;
		char workingDir[128];
		int rc;
		if (getcwd(workingDir, sizeof(workingDir))) {
			ss << workingDir << "/html" << virtualFile;
		}
		string sFile = ss.str();
		const char* realFile = sFile.c_str();
		xlog(realFile);
		FILE* f = fopen(realFile, "r");
		if (f) {
			struct stat s;
			rc = stat(realFile, &s);
			if (rc == 0) {
				size_t length = virtualFile.length();
				const char* contentType = NULL;
				if (length > 3 && virtualFile[length - 3] == '.' && virtualFile[length - 2] == 'j' && virtualFile[length - 1] == 's') {
					contentType = "application/javascript";
				}
				else if (length > 4 && virtualFile[length - 4] == '.') {
					if (virtualFile[length - 3] == 'i' && virtualFile[length - 2] == 'c' && virtualFile[length - 1] == 'o') {
						contentType = "image/x-icon";
					}
					else if (virtualFile[length - 3] == 'c' && virtualFile[length - 2] == 's' && virtualFile[length - 1] == 's') {
						contentType = "text/css";
					}
				}
				char header[1024];
				snprintf(header, sizeof(header),
					"HTTP/1.0 200 OK\r\n"
					"Cache-Control: max-age=3600"
					"Content-Lenth: %lu\r\n"
					"Content-Type: %s\r\n\r\n",
					s.st_size, contentType);
				send_timeout(clientSocket, header, strlen(header), 0);
				if (headOnly == false) {
					char* buffer = static_cast<char*>(malloc(s.st_size));
					if (buffer) {
						size_t r = fread(buffer, 1, s.st_size, f);
						send_timeout(clientSocket, buffer, r, 0);
						free(buffer);
						fclose(f);
						return;
					}
				}
			}
			fclose(f);
		}
		std::stringstream reply;
		reply << HtmlResponseNotFound(virtualFile);
		send_timeout(clientSocket, reply.str().c_str(), reply.str().size(), 0);
	}

	void WebClient::handleLocoSpeed(const map<string, string>& arguments)
	{
		locoID_t locoID = GetIntegerMapEntry(arguments, "loco", LocoNone);
		speed_t speed = GetIntegerMapEntry(arguments, "speed", MinSpeed);

		manager.locoSpeed(ControlTypeWebserver, locoID, speed);

		stringstream ss;
		ss << "Loco &quot;" << manager.getLocoName(locoID) << "&quot; is now set to speed " << speed;
		string sOut = ss.str();
		simpleReply(sOut);
	}

	void WebClient::handleLocoDirection(const map<string, string>& arguments)
	{
		locoID_t locoID = GetIntegerMapEntry(arguments, "loco", LocoNone);
		string directionText = GetStringMapEntry(arguments, "direction", "forward");
		direction_t direction = (directionText.compare("forward") == 0 ? DirectionRight : DirectionLeft);

		manager.locoDirection(ControlTypeWebserver, locoID, direction);

		stringstream ss;
		ss << "Loco &quot;" << manager.getLocoName(locoID) << "&quot; is now set to " << direction;
		string sOut = ss.str();
		simpleReply(sOut);
	}

	void WebClient::handleLocoFunction(const map<string, string>& arguments)
	{
		locoID_t locoID = GetIntegerMapEntry(arguments, "loco", LocoNone);
		function_t function = GetIntegerMapEntry(arguments, "function", 0);
		bool on = GetIntegerMapEntry(arguments, "on", false);

		manager.locoFunction(ControlTypeWebserver, locoID, function, on);

		stringstream ss;
		ss << "Loco &quot;" << manager.getLocoName(locoID) << "&quot; has now set f";
		ss << function << " to " << (on ? "on" : "off");
		string sOut = ss.str();
		simpleReply(sOut);
	}

	void WebClient::handleLocoEdit(const map<string, string>& arguments)
	{
		stringstream ss;
		locoID_t locoID = GetIntegerMapEntry(arguments, "loco", LocoNone);
		controlID_t controlID = ControlNone;
		protocol_t protocol = ProtocolNone;
		address_t address = AddressNone;
		string name("New Loco");
		if (locoID > LocoNone)
		{
			const datamodel::Loco* loco = manager.getLoco(locoID);
			controlID = loco->controlID;
			protocol = loco->protocol;
			address = loco->address;
			name = loco->name;
		}
		HtmlTag h1("h1");
		h1.AddContent("Edit loco &quot;" + name + "&quot;");
		ss << h1;
		ss << "<form id=\"editform\">";
		ss << HtmlTagInputHidden("cmd", "locosave");
		ss << HtmlTagInputHidden("loco", to_string(locoID));
		ss << HtmlTagInputTextWithLabel("name", "Loco Name:", name);

		std::map<controlID_t,string> controls = manager.controlListNames();
		std::map<string, string> controlOptions;
		for(auto control : controls)
		{
			controlOptions[to_string(control.first)] = control.second;
		}
		ss << "<label>Control:</label>";
		ss << HtmlTagSelect("control", controlOptions, to_string(controlID));

		std::map<protocol_t,string> protocols = manager.protocolsOfControl(controlID);
		std::map<string, string> protocolOptions;
		for(auto protocol : protocols)
		{
			protocolOptions[to_string(protocol.first)] = protocol.second;
		}
		ss << "<label>Protocol:</label>";
		ss << "<div id=\"protocol\">";
		ss << HtmlTagSelect("protocol", protocolOptions, to_string(protocol));
		ss << "</div>";
		ss << HtmlTagInputTextWithLabel("address", "Address:", to_string(address));
		ss << "</form>";
		ss << HtmlTagButtonCancel();
		ss << HtmlTagButtonOK();
		ss << "<script>\n";
		ss << "//# sourceURL=handleLocoEdit.js";
		ss << "</script>\n";
		string sOut = ss.str();
		simpleReply(sOut);
	}

	void WebClient::handleProtocol(const map<string, string>& arguments)
	{
		stringstream ss;
		controlID_t controlId = GetIntegerMapEntry(arguments, "control", ControlIdNone);
		if (controlId > ControlIdNone)
		{
			ss << "<label>Protocol:</label>";
			protocol_t selectedProtocol = static_cast<protocol_t>(GetIntegerMapEntry(arguments, "protocol", ProtocolNone));
			std::map<protocol_t,string> protocols = manager.protocolsOfControl(controlId);
			std::map<string,string> protocolsTextMap;
			for(auto protocol : protocols)
			{
				protocolsTextMap[to_string(protocol.first)] = protocol.second;
			}
			ss << HtmlTagSelect("protocol", protocolsTextMap, to_string(selectedProtocol));
		}
		else
		{
			ss << "Unknown control";
		}
		string sOut = ss.str();
		simpleReply(sOut);
	}

	void WebClient::handleLocoSave(const map<string, string>& arguments)
	{
		stringstream ss;
		locoID_t locoID = GetIntegerMapEntry(arguments, "loco", LocoNone);
		if (locoID > LocoNone)
		{
			string name = GetStringMapEntry(arguments, "name");
			controlID_t controlId = GetIntegerMapEntry(arguments, "control", ControlIdNone);
			protocol_t protocol = static_cast<protocol_t>(GetIntegerMapEntry(arguments, "protocol", ProtocolNone));
			address_t address = GetIntegerMapEntry(arguments, "address", AddressNone);
			string result;
			if (!manager.locoSave(locoID, name, controlId, protocol, address, result))
			{
				ss << "<p>" << result << "</p>";
			}
			else
			{
				ss << "<p>Loco &quot;" << locoID << "&quot; saved.</p>";
			}
		}
		else
		{
			ss << "<p>Unable to save loco.</p>";
		}

		string sOut = ss.str();
		simpleReply(sOut);
	}

	void WebClient::handleUpdater(const map<string, string>& headers)
	{
		char reply[1024];
		int ret = snprintf(reply, sizeof(reply),
			"HTTP/1.0 200 OK\r\n"
			"Cache-Control: no-cache, must-revalidate\r\n"
			"Pragma: no-cache\r\n"
			"Expires: Sun, 12 Feb 2016 00:00:00 GMT\r\n"
			"Content-Type: text/event-stream; charset=utf-8\r\n\r\n");
		send_timeout(clientSocket, reply, ret, 0);

		unsigned int updateID = GetIntegerMapEntry(headers, "Last-Event-ID");
		while(run)
		{
			string s;
			if (server.nextUpdate(updateID, s))
			{
				ret = snprintf(reply, sizeof(reply), "id: %i\r\n%s\r\n\r\n", updateID, s.c_str());
				ret = send_timeout(clientSocket, reply, ret, 0);
				++updateID;
				if (ret < 0)
				{
					return;
				}
			}
			else
			{
				// FIXME: use conditional variables instead of sleep
				usleep(100000);
			}
		}
	}

	void WebClient::simpleReply(const string& text, const string& code)
	{
		size_t contentLength = text.length();
		char reply[256 + contentLength];
		snprintf(reply, sizeof(reply),
			"HTTP/1.0 %s\r\n"
			"Cache-Control: no-cache, must-revalidate\r\n"
			"Pragma: no-cache\r\n"
			"Expires: Sun, 12 Feb 2016 00:00:00 GMT\r\n"
			"Content-Type: text/html; charset=utf-8\r\n\r\n"
			"%s",
			code.c_str(), text.c_str());
		send_timeout(clientSocket, reply, strlen(reply), 0);
	}

	string WebClient::selectLoco(const map<string,string>& options)
	{
		stringstream ss;
		ss << "<form method=\"get\" action=\"/\" id=\"selectLoco_form\">";
		HtmlTagSelect selectLoco("loco", options);
		selectLoco.AddAttribute("onchange", "loadDivFromForm('selectLoco_form', 'loco')");
		ss << selectLoco;
		ss << HtmlTagInputHidden("cmd", "loco");
		ss << "</form>";
		return ss.str();
	}

	void WebClient::printLoco(const map<string, string>& arguments)
	{
		string sOut;
		locoID_t locoID = GetIntegerMapEntry(arguments, "loco", LocoNone);
		if (locoID > LocoNone)
		{
			stringstream ss;
			Loco* loco = manager.getLoco(locoID);
			HtmlTag p("p");
			p.AddContent(loco->name);
			ss << p;

			unsigned int speed = loco->Speed();
			map<string,string> buttonArguments;
			buttonArguments["loco"] = to_string(locoID);
			ss << HtmlTagInputSliderLocoSpeed("speed", "locospeed", MinSpeed, MaxSpeed, speed, locoID);
			buttonArguments["speed"] = "0";
			ss << HtmlTagButtonCommand("0%", "locospeed", buttonArguments);
			buttonArguments["speed"] = "255";
			ss << HtmlTagButtonCommand("25%", "locospeed", buttonArguments);
			buttonArguments["speed"] = "511";
			ss << HtmlTagButtonCommand("50%", "locospeed", buttonArguments);
			buttonArguments["speed"] = "767";
			ss << HtmlTagButtonCommand("75%", "locospeed", buttonArguments);
			buttonArguments["speed"] = "1023";
			ss << HtmlTagButtonCommand("100%", "locospeed", buttonArguments);
			buttonArguments.erase("speed");

			buttonArguments["function"] = "0";
			buttonArguments["on"] = "1";
			ss << HtmlTagButtonCommand("f0 on", "locofunction", buttonArguments);
			buttonArguments["on"] = "0";
			ss << HtmlTagButtonCommand("f0 off", "locofunction", buttonArguments);
			buttonArguments.erase("function");
			buttonArguments.erase("on");

			buttonArguments["direction"] = "forward";
			ss << HtmlTagButtonCommand("forward", "locodirection", buttonArguments);
			buttonArguments["direction"] = "reverse";
			ss << HtmlTagButtonCommand("reverse", "locodirection", buttonArguments);
			buttonArguments.erase("direction");

			ss << HtmlTagButtonPopup("Edit", "locoedit", buttonArguments);
			sOut = ss.str();
		}
		else
		{
			sOut = "No locoID provided";
		}
		simpleReply(sOut);
	}

	void WebClient::printMainHTML() {
		// handle base request
		stringstream ss;
		ss << "HTTP/1.0 200 OK\r\n"
			"Cache-Control: no-cache, must-revalidate\r\n"
			"Pragma: no-cache\r\n"
			"Expires: Sun, 12 Feb 2016 00:00:00 GMT\r\n"
			"Content-Type: text/html; charset=utf-8\r\n\r\n"
			"<!DOCTYPE html><html><head>"
			"<title>RailControl</title>"
			"<link rel=\"stylesheet\" type=\"text/css\" href=\"/style.css\" />"
			"<script src=\"/jquery-3.1.1.min.js\"></script>"
			"<script src=\"/javascript.js\"></script>"
			"<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
			"<meta name=\"robots\" content=\"noindex,nofollow\">"
			"</head>"
			"<body onload=\"loadDivFromForm('selectLoco_form', 'loco')\">"
			"<h1>Railcontrol</h1>"
			"<div class=\"menu\">";
		ss << HtmlTagButtonCommand("X", "quit");
		ss << HtmlTagButtonCommand("On", "on");
		ss << HtmlTagButtonCommand("Off", "off");
		ss << HtmlTagButtonPopup("NewLoco", "locoedit");
		ss << "</div>";
		ss << "<div class=\"locolist\">";
		// locolist
		const map<locoID_t, Loco*>& locos = manager.locoList();
		map<string,string> options;
		for (auto locoTMP : locos) {
			Loco* loco = locoTMP.second;
			options[to_string(loco->objectID)] = loco->name;
		}
		ss << selectLoco(options);
		ss <<"</div>";
		ss << "<div class=\"loco\" id=\"loco\">";
		ss << "</div>";
		ss << "<div class=\"popup\" id=\"popup\">Popup</div>"
			"<div class=\"status\" id=\"status\"></div>"
			"<script type=\"application/javascript\">\n"
			"var updater = new EventSource('/?cmd=updater');\n"
			"updater.onmessage = function(e) {\n"
			" var status = document.getElementById('status');\n"
			" var arguments = e.data.split(';');\n"
			" var argumentMap = new Map();\n"
			" arguments.forEach(function(argument) {\n"
			"  var parts = argument.split('=');\n"
			"  if (parts[0] == 'status') {\n"
			"   status.innerHTML += parts[1] + '<br>';\n"
			"   status.scrollTop = status.scrollHeight - status.clientHeight;\n"
			"  }\n"
			"  else {\n"
			"   argumentMap.set(parts[0], parts[1]);\n"
			"  }\n"
			" })\n"
			" if (argumentMap.get('command') == 'locospeed') {\n"
			"  var elementName = 'locospeed_' + argumentMap.get('loco');\n"
			"  var element = document.getElementById(elementName);\n"
			"  if (element) element.value = argumentMap.get('speed');\n"
			" }\n"
			"};\n"

			// FIXME: get first locoid in db
			"</script>"
			"</body>"
			"</html>";
		string sOut = ss.str();
		const char* html = sOut.c_str();
		send_timeout(clientSocket, html, strlen(html), 0);
	}

} ; // namespace webserver
