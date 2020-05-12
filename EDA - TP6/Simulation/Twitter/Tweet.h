#pragma once
#include <string>
#include <ostream>
class Tweet
{
public:
	Tweet(const std::string& username_, const std::string& content_, const std::string& date_);

	friend std::ostream& operator <<(std::ostream& o, const Tweet& tw);

	const std::string& getContent() const;
	const std::string& getDate() const;
	const std::string& getUsername() const;

	~Tweet();
private:
	std::string username, content, date;
	void transformData();
};
