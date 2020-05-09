#include "Tweet.h"

const char* extraString = "+0000";
const int extraStrSize = strlen(extraString);

Tweet::Tweet(std::string username_, std::string content_, std::string date_) : username(username_), content(content_)
{
	//Finds '+0000' structure that comes in API date form.
	int pos = date_.find(extraString);

	//If there was a '+0000', it removes it.
	if (pos != std::string::npos)
		date = date_.substr(0, pos) + date_.substr(pos + extraStrSize + 1, date_.length() - pos - 1 - extraStrSize);

	else
		date = date_;
};

Tweet::~Tweet() {};

std::ostream& operator << (std::ostream& o, const Tweet& tw) {
	o << tw.username << std::endl;

	int pos = tw.content.find("https");
	if (pos != std::string::npos)
		o << tw.content.substr(0, pos) << std::endl;
	else
		o << tw.content << std::endl;

	o << tw.date << std::endl;
	return o;
}