#include "Tweet.h"

Tweet::Tweet(std::string username_, std::string content_, std::string date_) : username(username_), content(content_), date(date_) {};

Tweet::~Tweet() {};

std::ostream& operator << (std::ostream& o, const Tweet& tw) {
	o << tw.username << std::endl;
	o << tw.content.substr(0, tw.content.find("https")) << std::endl;
	o << tw.date << std::endl;
	return o;
}