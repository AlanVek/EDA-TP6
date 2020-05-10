#include "TwitterClient.h"
#include "MyErrors.h"
#include "concreteLCD.h"
#include "basicLCD.h"
#include <iostream>

int main() {
	////Creates TwitterClient with username and tweet count.
	//TwitterClient TC("__SOMEUSERNAME__");

	//bool running = true;
	//int result = -1;

	////Attempts to retrieve tweets.
	//try {
	//	//Attempts to get client token.
	//	TC.getToken();

	//	//Attempts to loop until client gets all tweets.
	//	while (running)
	//		running = TC.getTweets();

	//	//Prints tweets.
	//	TC.printTweets();
	//	result = 0;
	//}

	////Otherwise, it prints error message.
	//catch (std::exception& e) {
	//	std::cout << e.what() << std::endl;
	//}

	//return result;
	concreteLCD c_lcd;
	basicLCD* lcd = &c_lcd;

	if (lcd->lcdInitOk())
		*lcd << 'a';

	al_rest(4);
}