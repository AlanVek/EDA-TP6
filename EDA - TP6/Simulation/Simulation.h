#pragma once
#include "Twitter/TwitterClient.h"
#include "LCD/concreteLCD.h"
#include "GUI/GUI.h"

const enum class loadState {
	notLoaded,
	loadedTC,
	requestedTweets,
};

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

	void roll();

	basicLCD* lcd;
	TwitterClient* tc;
	GUI* gui;
	bool running;

	bool rollTweets;

	int positionRoll;

	loadState loaded;

	unsigned int tweetNumber;

	void loadingMessage(int*);

	void showNextTweet();
	void showPreviousTweet();
	void reloadTweet();

	ALLEGRO_TIMER* timer;
	ALLEGRO_EVENT_QUEUE* queue;
	ALLEGRO_EVENT ev;

	double timeRoll;
};
