#include "TwitterClient.h"
#include "CurlErrors.h"
#include <iostream>

int main() {
	//Creates TwitterClient with username and tweet count.
	TwitterClient TC("__SOMEUSERNAME__", 10);

	try {
		//Gets client token.
		TC.getToken();

		//Gets tweets.
		bool running = TC.getTweets();

		//Loops until client gets all tweets.
		while (running)
			running = TC.getTweets();

		//Prints tweets.
		TC.printTweets();
	}

	//Otherwise, it shows error message.
	catch (CurlErrors& e) {
		std::cout << e.what() << std::endl;
	}
	catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	}
}