#include "TwitterClient.h"
#include "json.hpp"
#include "CurlErrors.h"
#include <iostream>

using json = nlohmann::json;

json j;

const char* userLink = "https://api.twitter.com/1.1/statuses/user_timeline.json?screen_name=";
const char* tokenLink = "https://api.twitter.com/oauth2/token";
const char* auxBearerStr = "Authorization: Bearer ";
const char* countCode = "&count=";
const char* API_key = "HCB39Q15wIoH61KIkY5faRDf6";
const char* API_SecretKey = "7s8uvgQnJqjJDqA6JsLIFp90FcOaoR5Ic41LWyHOic0Ht3SRJ6";
const char* contentType = "Content-Type: application/x-www-form-urlencoded;charset=UTF-8";
const char* grantType = "grant_type=client_credentials";

size_t writeCallback(char*, size_t, size_t, void*);

TwitterClient::TwitterClient(std::string username_, int tweetCount_) : username(username_), tweetCount(tweetCount_)
{
	query = userLink + username + countCode + std::to_string(tweetCount);
	multiHandler = nullptr;

	handler = nullptr;

	stillRunning = 1;
}

void TwitterClient::configurateTokenClient(void) {
	struct curl_slist* list = NULL;

	curl_easy_setopt(handler, CURLOPT_VERBOSE, 1L);
	//Sets URL to read from.
	curl_easy_setopt(handler, CURLOPT_URL, tokenLink);

	//Tells cURL to redirect if requested.
	curl_easy_setopt(handler, CURLOPT_FOLLOWLOCATION, 1L);

	//Sets protocols (HTTP and HTTPS).
	curl_easy_setopt(handler, CURLOPT_PROTOCOLS, CURLPROTO_HTTP | CURLPROTO_HTTPS);
	curl_easy_setopt(handler, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);

	//Sets cURL to have API key and secret key.
	std::string password = API_key + (std::string)":" + API_SecretKey;
	curl_easy_setopt(handler, CURLOPT_USERPWD, password.c_str());

	//Sets header to be of contentType.
	list = curl_slist_append(list, contentType);
	curl_easy_setopt(handler, CURLOPT_HTTPHEADER, list);

	//Sets data in header.
	std::string grantTypeHeader = grantType;
	curl_easy_setopt(handler, CURLOPT_POSTFIELDSIZE, grantTypeHeader.size());
	curl_easy_setopt(handler, CURLOPT_POSTFIELDS, grantTypeHeader.c_str());

	//Sets callback and userData.
	curl_easy_setopt(handler, CURLOPT_WRITEFUNCTION, &writeCallback);
	curl_easy_setopt(handler, CURLOPT_WRITEDATA, &unparsedAnswer);

	//std::string pp = "POST";
	//curl_easy_setopt(handler, CURLOPT_CUSTOMREQUEST, pp.c_str());
}

void TwitterClient::getToken(void) {
	handler = curl_easy_init();

	if (!handler)
		throw(CurlErrors("Failed to initialize Curl.\n"));

	configurateTokenClient();

	errorEasy = curl_easy_perform(handler);

	if (errorEasy != CURLE_OK) {
		curl_easy_cleanup(handler);
		throw (CurlErrors("Failed to perform cURL easy mode.\n"));
	}
	curl_easy_cleanup(handler);

	j = json::parse(unparsedAnswer);

	try
	{
		std::string aux = j["access_token"];
		token = aux;
	}
	catch (std::exception& e)
	{
		throw (CurlErrors("Failed to get token from json answer.\n"));
	}
}

void TwitterClient::configurateTweetClient(void) {
	struct curl_slist* list = nullptr;

	unparsedAnswer.clear();

	curl_multi_add_handle(multiHandler, handler);

	curl_easy_setopt(handler, CURLOPT_URL, query.c_str());
	curl_easy_setopt(handler, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(handler, CURLOPT_PROTOCOLS, CURLPROTO_HTTP | CURLPROTO_HTTPS);

	std::string aux = auxBearerStr + token;
	list = curl_slist_append(list, aux.c_str());
	curl_easy_setopt(handler, CURLOPT_HTTPHEADER, list);

	curl_easy_setopt(handler, CURLOPT_WRITEFUNCTION, writeCallback);
	curl_easy_setopt(handler, CURLOPT_WRITEDATA, &unparsedAnswer);
}

void TwitterClient::getTweets(void) {
	handler = curl_easy_init();
	if (!handler)
		throw (CurlErrors("Failed to initialize easy handler.\n"));

	multiHandler = curl_multi_init();

	if (!multiHandler)
		throw (CurlErrors("Failed to initialize multi handler.\n"));

	configurateTweetClient();

	if (stillRunning) {
		errorMulti = curl_multi_perform(multiHandler, &stillRunning);

		if (errorMulti != CURLE_OK) {
			curl_easy_cleanup(handler);
			curl_multi_cleanup(multiHandler);
			throw (CurlErrors("Failed to perform cURL to get tweets.\n"));
		}
	}
	else {
		curl_easy_cleanup(handler);
		curl_multi_cleanup(multiHandler);

		j = json::parse(unparsedAnswer);

		try
		{
			std::string content, date;
			for (auto object : j) {
				std::string content = j["text"];
				std::string date = j["created_at"];

				tweetVector.emplace_back(Tweet(username, content, date));
			}
		}
		catch (std::exception& e)
		{
			throw (CurlErrors("Failed to get tweets from json answer.\n"));
		}
	}
}

void TwitterClient::printTweets(void) {
	for (auto tw : tweetVector) {
		std::cout << tw << std::endl;
	}
}

size_t writeCallback(char* ptr, size_t size, size_t nmemb, void* userData) {
	std::string* userDataPtr = (std::string*) userData;

	userDataPtr->append(ptr, size * nmemb);

	return size * nmemb;
}

TwitterClient::~TwitterClient() {
	/*if (handler)
		curl_easy_cleanup(handler);
	if (multiHandler) {
		curl_multi_cleanup(multiHandler);
	}*/
}