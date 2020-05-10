#include "Simulation.h"
#include <exception>
#include <string>

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
}

//Simulation constructor.
Simulation::Simulation(void) : running(true), loaded(false)
{
	//Attempts to create new LCD and TwitterClient.
	lcd = new (std::nothrow) concreteLCD;
	tc = new (std::nothrow) TwitterClient;

	/*gui = new GUI;*/

	//Checks for errors.
	if (!lcd)
		throw std::exception("Failed to allocate memory for lcd.");
	if (!lcd->lcdInitOk()) {
		std::string errStr = "Failed to create lcd instance. Code: " + std::to_string(lcd->lcdGetError());
		throw std::exception(errStr.c_str());
	}
	if (!tc)
		throw std::exception("Failed to create Twitter client.");

	//Requests TwitterClient token.
	tc->requestToken();

	tweetNumber = 0;
}

//Simulation destructor. Deletes used resources.
Simulation::~Simulation() {
	if (lcd)
		delete lcd;
	if (tc)
		delete tc;
	/*if (gui)
		delete gui;*/
}

//Polls GUI and dispatches according to button code.
void Simulation::dispatch(int code) {
	/*int code = gui->pressed();*/
	switch (code) {
	case NOTHING:
		/*Roll display...*/
		/*Or show next tweet...*/
		break;
	case DOWN:
		if (!lcd->lcdMoveCursorDown())
			throw std::exception("Failed to move cursor down.");
		break;
	case UP:
		if (!lcd->lcdMoveCursorUp())
			throw std::exception("Failed to move cursor up.");
		break;
	case LEFT:
		if (!lcd->lcdMoveCursorLeft())
			throw std::exception("Failed to move cursor left.");
		break;
	case RIGHT:
		if (!lcd->lcdMoveCursorRight())
			throw std::exception("Failed to move cursor right.");
		break;
	case REQUEST:
		if (loaded) {
			tweetNumber = 0;
			performRequest();
		}
		break;
	case LOAD:
		/*loadClient(gui->getUsername(), gui->getTweetCount());*/
		break;
	case END:
		running = false;
		break;
	case NEXT:
		if (loaded)
			showNextTweet();
		break;
	case PREVIOUS:
		if (loaded)
			showPreviousTweet();
		break;
	case SETCURSOR:
		cursorPosition temp;
		temp.column = 0; temp.row = 0;
		/*temp.column = gui->newCursorColumn();
		temp.row = gui->newCursorRow();*/
		lcd->lcdSetCursorPosition(temp);
		break;
	case CLEARALL:
		lcd->lcdClear();
		break;
	case CLEAREOL:
		lcd->lcdClearToEOL();
		break;
	}
}

//Loads client with username and tweetCount.
void Simulation::loadClient(const char* username, int tweetCount) {
	try {
		tc->newUsername(username);

		if (tweetCount)
			tc->newTweetCount(tweetCount);

		//Shows username in LCD.
		*lcd << (unsigned char*)username;

		loaded = true;
	}
	catch (std::exception& e) {
		lcd->lcdClear();
		*lcd << (unsigned char*)e.what();
	}
}

//Requests tweets.
void Simulation::performRequest(void) {
	bool going = true;
	while (going)
		going = tc->requestTweets();
	lcd->lcdClear();
	*lcd << (unsigned char*)tc->getTweets()[0].getDate().c_str();
	*lcd << (unsigned char*)tc->getTweets()[0].getContent().c_str();
}

//Getter.
bool Simulation::isRunning(void) { return running; }

//Shows next tweet or shows error message in lcd if there are no more tweets.
void Simulation::showNextTweet() {
	try {
		if (tweetNumber >= (tc->getTweets().size() - 1)) {
			tweetNumber = tc->getTweets().size();
			lcd->lcdClear();
			*lcd << (unsigned char*)"No more tweets.";
		}
		else {
			tweetNumber++;
			lcd->lcdClear();
			*lcd << (unsigned char*)tc->getTweets()[tweetNumber].getDate().c_str();
			*lcd << (unsigned char*)tc->getTweets()[tweetNumber].getContent().c_str();
		}
	}
	catch (std::exception& e) {
		return;
	}
}

//Shows previous tweet (if there is one).
void Simulation::showPreviousTweet() {
	try {
		if (tweetNumber <= 0) {
			return;
		}
		else {
			tweetNumber--;
			lcd->lcdClear();
			*lcd << (unsigned char*)tc->getTweets()[tweetNumber].getDate().c_str();
			*lcd << (unsigned char*)tc->getTweets()[tweetNumber].getContent().c_str();
		}
	}
	catch (std::exception& e) {
		return;
	}
}