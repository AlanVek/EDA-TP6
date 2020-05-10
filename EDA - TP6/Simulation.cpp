#include "Simulation.h"
#include <exception>
#include <string>

//Simulation constructor.
Simulation::Simulation(void) : running(true)
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
void Simulation::dispatch() {
	int code = NULL;
	/*int code = gui->pressed();*/
	switch (code) {
	case 0:
		/*Roll display...*/
	case 1:
		if (!lcd->lcdMoveCursorDown())
			throw std::exception("Failed to move cursor down.");
		break;
	case 2:
		if (!lcd->lcdMoveCursorUp())
			throw std::exception("Failed to move cursor up.");
		break;
	case 3:
		if (!lcd->lcdMoveCursorLeft())
			throw std::exception("Failed to move cursor left.");
		break;
	case 4:
		if (!lcd->lcdMoveCursorRight())
			throw std::exception("Failed to move cursor right.");
		break;
	case 5:
		tweetNumber = 0;
		performRequest();
		break;
	case 6:
		/*loadClient(gui->getUsername(), gui->getTweetCount());*/
		break;
	case 7:
		running = false;
		break;
	case 8:
		showNextTweet();
		break;
	case 9:
		showPreviousTweet();
		break;
	case 10:
		cursorPosition temp;
		temp.column = 0; temp.row = 0;
		/*temp.column = gui->newCursorColumn();
		temp.row = gui->newCursorRow();*/
		lcd->lcdSetCursorPosition(temp);
		break;
	case 11:
		lcd->lcdClear();
		break;
	case 12:
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
	}
	catch (std::exception& e) {
		lcd->lcdClear();
		*lcd << (unsigned char*)e.what();
	}
}

//Requests tweets.
void Simulation::performRequest(void) {
	tc->requestTweets();
}

//Getter.
bool Simulation::isRunning(void) { return running; }

//Shows next tweet or shows error message in lcd if there are no more tweets.
void Simulation::showNextTweet() {
	if (tweetNumber >= tc->getTweets().size() - 1) {
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

//Shows previous tweet (if there is one).
void Simulation::showPreviousTweet() {
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