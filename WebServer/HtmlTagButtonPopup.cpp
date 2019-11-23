/*
RailControl - Model Railway Control Software

Copyright (c) 2017-2019 Dominik (Teddy) Mahrer - www.railcontrol.org

RailControl is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 3, or (at your option) any
later version.

RailControl is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with RailControl; see the file LICENCE. If not see
<http://www.gnu.org/licenses/>.
*/

#include <sstream>
#include <vector>

#include "Utils/Utils.h"
#include "WebServer/HtmlTagButtonPopup.h"
#include "WebServer/HtmlTagInput.h"

namespace WebServer
{
	HtmlTagButtonPopup::HtmlTagButtonPopup(const std::string& value, const std::string& command, const std::map<std::string,std::string>& arguments)
	:	HtmlTagButton(value, command)
	{
		std::vector<std::string> parts;
		Utils::Utils::SplitString(command, "_", parts);
		std::stringstream ss;
		ss << "var myUrl = '/?cmd=" << parts[0];
		for (auto argument : arguments) {

			ss << "&" << argument.first << "=" << argument.second;
		}
		ss <<"';"
			"loadPopup(myUrl);"
			"return false;";
		AddAttribute("onclick", ss.str());
	}
};