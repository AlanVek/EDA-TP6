#include "TwitterClient.h"
#include <iostream>
#include "concreteLCD.h"

int main() {
	const std::string username = "__SOMEUSERNAME__";
	const int tweetCount = 3;

	//Creates TwitterClient with username and tweet count.
	TwitterClient TC(username, tweetCount);

	bool running = true;
	int result = -1;

	//Creates LCD.
	concreteLCD c_lcd;
	basicLCD& lcd = c_lcd;

	if (lcd.lcdInitOk()) {
		//Shows username in display.
		lcd << (unsigned char*)username.c_str();

		try {
			//Attempts to get client token.
			TC.requestToken();

			//Attempts to loop until client gets all tweets.
			while (running)
				running = TC.requestTweets();

			//Prints tweets.
			//TC.printTweets();

			//Prints tweet dates.
			al_rest(2);
			lcd.lcdClear();
			for (auto tw : TC.getTweets()) {
				lcd << (unsigned char*)tw.getDate().c_str();
				al_rest(2);
				lcd.lcdClear();
			}

			result = 0;
		}

		//Otherwise, it prints error message.
		catch (std::exception& e) {
			lcd.lcdClear();
			lcd << (unsigned char*)e.what();
			al_rest(3);
		}
	}
	else
		std::cout << "Failed to initialize LCD. Code: " << lcd.lcdGetError() << std::endl;
}