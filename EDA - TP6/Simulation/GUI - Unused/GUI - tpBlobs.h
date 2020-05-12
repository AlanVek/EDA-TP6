#pragma once

#include <allegro5/allegro.h>

namespace {
	const int NOTHING = 0;
	const int DOWN = 1;
	const int UP = 2;
	const int LEFT = 3;
	const int RIGHT = 4;
	const int REQUEST = 5;
	const int LOAD = 6;
	const int END = 7;
	const int NEXT = 8;
	const int PREVIOUS = 9;
	const int SETCURSOR = 10;
	const int CLEARALL = 11;
	const int CLEAREOL = 12;

	const int loadedTC = 1;
	const int requestedTweets = 2;

	const int loadingDotsNumber = 3;

	const unsigned int MAXUSERNAME = 100;
}

class GUI {
public:

	GUI();

	~GUI();

	int checkGUIStatus(void);

	bool firstRun(void);

	const char* getUsername(void);
	int getTweetCount(void);

protected:
	void GUI_setUp(void);

	ALLEGRO_DISPLAY* guiDisp;
	ALLEGRO_EVENT_QUEUE* guiQueue;
	ALLEGRO_EVENT guiEvent;
	bool checkGUIEvents(void);

	char username[MAXUSERNAME];

	int dataEntered;

	bool request;
	int tweetCount;
};
