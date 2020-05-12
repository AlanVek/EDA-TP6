#pragma once
#include <curl/curl.h>
#include "Tweet.h"
#include "json.hpp"

class TwitterClient
{
public:
	TwitterClient(const std::string& username_, const int& tweetCount_ = NULL);

	TwitterClient(void);

	void initialSetup(void);
	bool requestTweets(void);

	const std::vector<Tweet>& getTweets() const;

	void newUsername(const char*);
	void newTweetCount(int);

	~TwitterClient(void);

private:
	void configurateTokenClient(void);
	void configurateTweetClient(void);
	void loadTweetVector(const  nlohmann::json&);

	CURL* handler, * multiHandler;
	CURLcode errorEasy;
	CURLMcode errorMulti;
	std::string token, query, username, unparsedAnswer;
	int tweetCount, stillRunning;

	std::vector<Tweet> tweetVector;
};
