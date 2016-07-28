#include "DKRockettoRML.h"
#include "DKLog.h"

//////////////////////////////////////////////////////////////
bool DKRocketToRML::toRml(const DKString& html, DKString& rml)
{
	rml = html;

	//Rocket doesn't like <!DOCTYPE html> tags
	replace(rml, "<!DOCTYPE html>", "");

	//Rocket doesn't like <meta> tags
	replace(rml, "<meta name=\"referrer\" content=\"no-referrer\"></meta>", "");

	//Add DKRocket.css
	replace(rml, "<link rel=\"stylesheet\" type=\"text/css\" href=\"DKRocket/DK.css\"></link>",
					"<link rel=\"stylesheet\" type=\"text/css\" href=\"DKRocket/DK.css\"></link>\n"
					"<link rel=\"stylesheet\" type=\"text/css\" href=\"DKRocket/DKRocket.css\"></link>");

	//Convert style sheets to rcss
	replace(rml, "type=\"text/css\"", "type=\"text/rcss\""); 

	// :rgba(r,g,b,a)  <- convert a to 0-255
	size_t end = 0;
	while(has(rml,":rgba(")){
		size_t temp = rml.find(":rgba(",end);
        if(temp == std::string::npos){return true;}
		size_t start = rml.find(",",temp);
		start = rml.find(",",start+1);
		start = rml.find(",",start+1);
		end = rml.find(")",start);
		if(end == std::string::npos){end = rml.size();}

		DKString out = rml.substr(start+1, end-start-1);
		float var = toFloat(out);

		if(var){
			int final = (int)(var * 255);
			rml.replace(start+1,end-start-1,toString(final));   
			//return true;
			continue;
		}
		DKLog("Error: in HtmlToRml()\n", DKERROR);
		return false;
	}
	return true;
}
