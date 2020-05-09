#include "TwitterClient.h"
#include "CurlErrors.h"
#include <iostream>

int main() {
	//Creates TwitterClient with username and tweet count.
	TwitterClient TC("__SOMEUSERNAME__", 10);

	bool running = true;
	int result = -1;

	try {
		//Gets client token.
		TC.getToken();

		//Loops until client gets all tweets.
		while (running)
			running = TC.getTweets();

		//Prints tweets.
		TC.printTweets();
		result = 0;
	}

	//Otherwise, it shows error message.
	catch (CurlErrors& e) {
		std::cout << e.what() << std::endl;
	}
	catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	}

	return result;
}