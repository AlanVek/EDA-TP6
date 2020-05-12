#pragma once
#include "Twitter/TwitterClient.h"
#include "LCD/concreteLCD.h"
#include "GUI/GUI.h"

const enum class loadState {
	notLoaded,
	loadedTC,
	requestedTweets,
};

namespace {
	const int loadingDotsNumber = 3;
}

class Simulation {
public:
	Simulation(void);
	~Simulation(void);

	void dispatch(void);

	bool isRunning(void);

	void getFirstData(void);

private:
	void loadClient(const char*, int = NULL);
	void performRequest(void);

	basicLCD* lcd;
	TwitterClient* tc;
	GUI* gui;
	bool running;

	loadState loaded;

	unsigned int tweetNumber;

	void loadingMessage(int*);

	void showNextTweet();
	void showPreviousTweet();
};
