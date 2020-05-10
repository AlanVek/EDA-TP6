#include "Tweet.h"

const char* extraString = "+0000";
const int extraStrSize = strlen(extraString);

const char* monthToNumber(const std::string& month);

//Original date format:      Sun Mar 28 18:17:53 +0000 2010
//Expected date format:      28/03/10 - 18:17
Tweet::Tweet(const std::string& username_, const std::string& content_, const std::string& date_) : username(username_), content(content_)
{
	std::string month = monthToNumber(date_.substr(4, 3));
	std::string day = date_.substr(8, 2);
	std::string year = date_.substr(date_.length() - 2, 2);
	std::string time = date_.substr(11, 5);

	date = day + '/' + month + '/' + year + " - " + time;
};

Tweet::~Tweet() {};

std::ostream& operator << (std::ostream& o, const Tweet& tw) {
	o << tw.username << std::endl;
	int pos = tw.content.find("http");
	if (pos != std::string::npos)
		o << tw.content.substr(0, pos) << std::endl;
	else
		o << tw.content << std::endl;
	o << tw.date << std::endl;
	return o;
}

const std::string& Tweet::getContent() const { return content; }
const std::string& Tweet::getDate() const { return date; }
const std::string& Tweet::getUsername() const { return username; }

const char* monthToNumber(const std::string& month) {
	if (month == "Jan")
		return "01";
	if (month == "Feb")
		return "02";
	if (month == "Mar")
		return "03";
	if (month == "Apr")
		return "04";
	if (month == "May")
		return "05";
	if (month == "Jun")
		return "06";
	if (month == "Jul")
		return "07";
	if (month == "Aug")
		return "08";
	if (month == "Sep")
		return "09";
	if (month == "Oct")
		return "10";
	if (month == "Nov")
		return "11";
	if (month == "Dic")
		return "12";
	else
		return "00";
}