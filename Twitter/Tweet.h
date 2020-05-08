#pragma once
#include <string>
#include <ostream>
class Tweet
{
public:
	Tweet(std::string username_, std::string content_, std::string date_);

	friend std::ostream& operator <<(std::ostream& o, const Tweet& tw);

	~Tweet();
private:
	std::string username, content, date;
};
