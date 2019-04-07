#include <sstream>

#include "webserver/HtmlTagStreet.h"

using std::string;
using std::to_string;

namespace webserver
{
	HtmlTagStreet::HtmlTagStreet(const datamodel::Street* street)
	{
		unsigned int layoutPosX = street->GetPosX() * EdgeLength;
		unsigned int layoutPosY = street->GetPosY() * EdgeLength;

		HtmlTag div1("div");
		string streetIdString = to_string(street->objectID);
		string id("st_" + streetIdString);
		div1.AddAttribute("id", id);
		div1.AddClass("layout_item");
		div1.AddClass("street_item");
		div1.AddAttribute("style", "left:" + to_string(layoutPosX) + "px;top:" + to_string(layoutPosY) + "px;");
		string image = "<svg width=\"" + EdgeLengthString + "\" height=\"" + EdgeLengthString + "\" id=\"_img\"><polygon points=\"1,20 7,20 7,28 1,28\" fill=\"none\" stroke=\"black\"/><polygon points=\"34,7 28,7 28,15 34,15\" fill=\"none\" stroke=\"black\"/><polyline points=\"7,24 15,24 20,11 28,11\" stroke=\"black\" fill=\"none\"/></svg>";
		div1.AddChildTag(HtmlTag().AddContent(image));
		div1.AddChildTag(HtmlTag("span").AddClass("tooltip").AddContent(street->name));
		div1.AddAttribute("onclick", "return onClickStreet(" + streetIdString + ");");
		div1.AddAttribute("oncontextmenu", "return onContextLayoutItem(event, '" + id + "');");
		AddChildTag(div1);

		HtmlTag div2("div");
		div2.AddClass("contextmenu");
		div2.AddAttribute("id", id + "_context");
		div2.AddAttribute("style", "left:" + to_string(layoutPosX + 5) + "px;top:" + to_string(layoutPosY + 30) + "px;");
		div2.AddChildTag(HtmlTag("ul").AddClass("contextentries")
			.AddChildTag(HtmlTag("li").AddClass("contextentry").AddContent(street->name))
			.AddChildTag(HtmlTag("li").AddClass("contextentry").AddContent("Release").AddAttribute("onClick", "fireRequestAndForget('/?cmd=streetrelease&switch=" + streetIdString + "');"))
			.AddChildTag(HtmlTag("li").AddClass("contextentry").AddContent("Edit").AddAttribute("onClick", "loadPopup('/?cmd=streetedit&street=" + streetIdString + "');"))
			.AddChildTag(HtmlTag("li").AddClass("contextentry").AddContent("Delete").AddAttribute("onClick", "loadPopup('/?cmd=streetaskdelete&street=" + streetIdString + "');"))
			);
		AddChildTag(div2);
	}
};
