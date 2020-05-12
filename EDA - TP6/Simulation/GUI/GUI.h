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

	const codes checkStatus(void);

	bool firstRun(void);

	const std::string& getUsername(void) const;
	const int getTweetCount(void) const;

	const float getSpeed(void) const;

protected:
	void initialSetup(void) const;

	ALLEGRO_DISPLAY* guiDisp;
	ALLEGRO_EVENT_QUEUE* guiQueue;
	ALLEGRO_EVENT guiEvent;
	bool checkGUIEvents(void);

	std::string username;

	bool usernameOk;
	int tweetCount;
	float speed;
};
