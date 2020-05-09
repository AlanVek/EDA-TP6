#include "TwitterClient.h"
#include "json.hpp"
#include "CurlErrors.h"
#include <iostream>

using json = nlohmann::json;

json j;

//Sets text constants to use during API request.
const char* userLink = "https://api.twitter.com/1.1/statuses/user_timeline.json?screen_name=";
const char* tokenLink = "https://api.twitter.com/oauth2/token";
const char* auxBearerStr = "Authorization: Bearer ";
const char* countCode = "&count=";
const char* API_key = "HCB39Q15wIoH61KIkY5faRDf6";
const char* API_SecretKey = "7s8uvgQnJqjJDqA6JsLIFp90FcOaoR5Ic41LWyHOic0Ht3SRJ6";
const char* contentType = "Content-Type: application/x-www-form-urlencoded;charset=UTF-8";
const char* grantType = "grant_type=client_credentials";

const int invalidUsername = 34;

size_t writeCallback(char*, size_t, size_t, void*);

//TwitterClient constructor.
TwitterClient::TwitterClient(std::string username_, int tweetCount_) : username(username_), tweetCount(tweetCount_)
{
	query = userLink + username + countCode + std::to_string(tweetCount);
	multiHandler = nullptr;

	handler = nullptr;

	stillRunning = 1;
}

//Configurates client for token request.
void TwitterClient::configurateTokenClient(void) {
	struct curl_slist* list = NULL;

	//Sets URL to read from.
	curl_easy_setopt(handler, CURLOPT_URL, tokenLink);

	//Tells cURL to redirect if requested.
	curl_easy_setopt(handler, CURLOPT_FOLLOWLOCATION, 1L);

	//Sets protocols (HTTP and HTTPS).
	curl_easy_setopt(handler, CURLOPT_PROTOCOLS, CURLPROTO_HTTP | CURLPROTO_HTTPS);

	//Sets authentification mode.
	curl_easy_setopt(handler, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);

	//Sets cURL to have API key and secret key.
	std::string password = API_key + (std::string)":" + API_SecretKey;
	curl_easy_setopt(handler, CURLOPT_USERPWD, password.c_str());

	//Sets header to be of contentType.
	list = curl_slist_append(list, contentType);
	curl_easy_setopt(handler, CURLOPT_HTTPHEADER, list);

	//Sets data in header.
	curl_easy_setopt(handler, CURLOPT_POSTFIELDSIZE, strlen(grantType));
	curl_easy_setopt(handler, CURLOPT_POSTFIELDS, grantType);

	//Sets callback and userData.
	curl_easy_setopt(handler, CURLOPT_WRITEFUNCTION, &writeCallback);
	curl_easy_setopt(handler, CURLOPT_WRITEDATA, &unparsedAnswer);
}

//Gets Token.
void TwitterClient::getToken(void) {
	//Creates and verifies handler.
	handler = curl_easy_init();

	if (!handler)
		throw(CurlErrors("Failed to initialize Curl."));

	//Configurates request parameters.
	configurateTokenClient();

	//Performs request.
	errorEasy = curl_easy_perform(handler);

	//Throws exception if error occurred.
	if (errorEasy != CURLE_OK) {
		curl_easy_cleanup(handler);
		throw (CurlErrors("Failed to perform cURL easy mode."));
	}
	curl_easy_cleanup(handler);

	j = json::parse(unparsedAnswer);

	//Attempts to get access token or throws error if there's been one.
	if (j.find("access_token") != j.end()) {
		std::string aux = j["access_token"];
		token = aux;
	}
	else
		throw (CurlErrors("Failed to get token from json answer."));
}

//Configurates client for tweet request.
void TwitterClient::configurateTweetClient(void) {
	unparsedAnswer.clear();

	struct curl_slist* list = nullptr;

	//Sets handler and multihandler.
	curl_multi_add_handle(multiHandler, handler);

	//Sets URL to read from.
	curl_easy_setopt(handler, CURLOPT_URL, query.c_str());

	//Tells cURL to redirect if requested.
	curl_easy_setopt(handler, CURLOPT_FOLLOWLOCATION, 1L);

	//Sets protocols (HTTP and HTTPS).
	curl_easy_setopt(handler, CURLOPT_PROTOCOLS, CURLPROTO_HTTP | CURLPROTO_HTTPS);

	//Sets header with token.
	std::string aux = auxBearerStr + token;
	list = curl_slist_append(list, aux.c_str());
	curl_easy_setopt(handler, CURLOPT_HTTPHEADER, list);

	//Sets callback and userData.
	curl_easy_setopt(handler, CURLOPT_WRITEFUNCTION, &writeCallback);
	curl_easy_setopt(handler, CURLOPT_WRITEDATA, &unparsedAnswer);
}

//Gets tweets.
bool TwitterClient::getTweets(void) {
	static bool step = false;

	bool finished = true;

	if (!step) {
		//Sets easy and multi modes with error checker.
		handler = curl_easy_init();
		if (!handler)
			throw (CurlErrors("Failed to initialize easy handler."));

		multiHandler = curl_multi_init();

		if (!multiHandler)
			throw (CurlErrors("Failed to initialize multi handler."));

		//If it's the first time in this run, it sets the request parameters.
		configurateTweetClient();
		step = true;
	}

	//Should be an if. Performs one request and checks for errors.
	if (stillRunning) {
		errorMulti = curl_multi_perform(multiHandler, &stillRunning);
		if (errorMulti != CURLE_OK) {
			curl_easy_cleanup(handler);
			curl_multi_cleanup(multiHandler);
			throw (CurlErrors("Failed to perform cURL to get tweets."));
		}
	}
	else {
		//Cleans used variables.
		curl_easy_cleanup(handler);
		curl_multi_cleanup(multiHandler);

		//Resets step to false.
		step = false;

		//Parses answer.
		j = json::parse(unparsedAnswer);

		//If there's been an error in the request...
		if (j.find("errors") != j.end()) {
			//If any of them has the code invalidUsername, it throws that error.
			for (auto x : j["errors"]) {
				if (x["code"] == invalidUsername)
					throw (CurlErrors("Username doesn't exist."));
			}

			//Otherwise, it throws a generic error.
			throw (CurlErrors("Unknown json error during request."));
		}

		//Attempts to load tweet vector or throws error if it wasn't possible.
		try {
			std::string content, date;
			for (auto object : j) {
				std::string content = object["text"];
				std::string date = object["created_at"];
				tweetVector.emplace_back(Tweet(username, content, date));
			}
		}
		catch (std::exception& e) {
			throw (CurlErrors("Failed to get tweets from json answer."));
		}

		//Sets result to 'FALSE', to end loop.
		finished = false;
	}
	return finished;
}

void TwitterClient::printTweets(void) {
	for (auto tw : tweetVector)
		std::cout << tw << std::endl;
}

size_t writeCallback(char* ptr, size_t size, size_t nmemb, void* userData) {
	std::string* userDataPtr = (std::string*) userData;

	userDataPtr->append(ptr, size * nmemb);

	return size * nmemb;
}

TwitterClient::~TwitterClient() {}