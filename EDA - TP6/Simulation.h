#pragma once
#include "Twitter/TwitterClient.h"
#include "LCD/concreteLCD.h"
class Simulation {
public:
	Simulation(void);
	~Simulation(void);

	void dispatch(int);

	bool isRunning(void);

private:
	void loadClient(const char*, int = NULL);
	void performRequest(void);

	basicLCD* lcd;
	TwitterClient* tc;
	/*GUI* gui;*/
	bool running;

	bool loaded;

	int tweetNumber;

	void showNextTweet();
	void showPreviousTweet();
};
