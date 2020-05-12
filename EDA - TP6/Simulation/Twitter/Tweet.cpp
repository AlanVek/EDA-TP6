#include "Tweet.h"
#include "boost/algorithm/string/replace.hpp"

const char* monthToNumber(const std::string& month);

//Original date format:      Sun Mar 28 18:17:53 +0000 2010
//Expected date format:      28/03/10 - 18:17

//Tweet constructor. Keeps username and content, and reshapes date into expected format.
Tweet::Tweet(const std::string& username_, const std::string& content_, const std::string& date_) : username(username_)
{
	std::string month = monthToNumber(date_.substr(4, 3));
	std::string day = date_.substr(8, 2);
	std::string year = date_.substr(date_.length() - 2, 2);
	std::string time = date_.substr(11, 5);

	date = day + '/' + month + '/' + year + " - " + time;

	content = username + " - " + content_.substr(0, content_.find("http"));

	transformData();
};

Tweet::~Tweet() {};

//Tweet printing.
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

//Class getters.
const std::string& Tweet::getContent() const { return content; }
const std::string& Tweet::getDate() const { return date; }
const std::string& Tweet::getUsername() const { return username; }

//Gets month's abbreviation and returns number in string.
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

void Tweet::transformData() {
	boost::replace_all(content, "á", "a");
	boost::replace_all(content, "é", "e");
	boost::replace_all(content, "í", "i");
	boost::replace_all(content, "ó", "o");
	boost::replace_all(content, "ú", "u");
	boost::replace_all(content, "Á", "A");
	boost::replace_all(content, "É", "E");
	boost::replace_all(content, "Í", "I");
	boost::replace_all(content, "Ó", "O");
	boost::replace_all(content, "Ú", "U");
	boost::replace_all(content, "ñ", "n");
	boost::replace_all(content, "Ñ", "N");
	boost::replace_all(content, "¿", "?");
	boost::replace_all(content, "¡", "!");
	boost::replace_all(content, "@", "_");
}