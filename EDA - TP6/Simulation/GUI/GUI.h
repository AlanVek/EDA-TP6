#pragma once

#include <allegro5/allegro.h>
#include <string>

const enum class codes {
	NOTHING,
	DOWN,
	UP,
	LEFT,
	RIGHT,
	REQUEST,
	END,
	NEXT,
	PREVIOUS,
	CLEARALL,
	CLEAREOL,
	CANCEL,
	RELOAD,
	SPEED
};

class GUI {
public:

	GUI();

	~GUI();

	codes checkStatus(void);

	bool firstRun(void);

	std::string& getUsername(void);
	int getTweetCount(void);

	float getSpeed(void);

protected:
	void initialSetup(void);

	ALLEGRO_DISPLAY* guiDisp;
	ALLEGRO_EVENT_QUEUE* guiQueue;
	ALLEGRO_EVENT guiEvent;
	bool checkGUIEvents(void);

	std::string username;

	bool usernameOk;
	int tweetCount;
	float speed;
};
