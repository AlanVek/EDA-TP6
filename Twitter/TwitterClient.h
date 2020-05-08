#pragma once
#include <curl/curl.h>
#include <string>
#include <vector>
#include "Tweet.h"

class TwitterClient
{
public:
	TwitterClient(std::string username_, int tweetCount_);

	void getToken(void);
	void getTweets(void);

	void printTweets(void);

	bool running(void) { return stillRunning; }

	~TwitterClient(void);

private:
	void configurateTokenClient(void);
	void configurateTweetClient(void);

	CURL* handler, * multiHandler;
	CURLcode errorEasy;
	CURLMcode errorMulti;
	std::string token, query, username, unparsedAnswer;
	int tweetCount, stillRunning;

	std::vector<Tweet> tweetVector;
};
