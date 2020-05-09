#include "TwitterClient.h"
#include "CurlErrors.h"
#include <iostream>

int main() {
	//Creates TwitterClient with username and tweet count.
	TwitterClient TC("sofvignolle", 10);

	bool isOver;

	try {
		//Gets client token.
		TC.getToken();

		//Gets tweets.
		isOver = TC.getTweets();

		/*while (!isOver)
			isOver = TC.getTweets();*/

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