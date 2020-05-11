#include "Simulation.h"
#include <exception>
#include <string>
#include "Twitter/API_request_error.h"

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
}

//Simulation constructor.
Simulation::Simulation(void) : running(true), loaded(0)
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
		if (loaded == loadedTC) {
			tweetNumber = 0;
			performRequest();
		}
		break;
	case LOAD:
		loadClient("AlanVekselman", 35);
		/*loadClient(gui->getUsername(), gui->getTweetCount());*/
		break;
	case END:
		running = false;
		break;
	case NEXT:
		if (loaded == requestedTweets)
			showNextTweet();
		break;
	case PREVIOUS:
		if (loaded == requestedTweets)
			showPreviousTweet();
		break;
	case SETCURSOR:
		cursorPosition temp;
		temp.column = 0; temp.row = 0;
		/*temp.column = gui->newCursorColumn();
		temp.row = gui->newCursorRow();*/
		if (!lcd->lcdSetCursorPosition(temp))
			throw std::exception("Failed to change cursor's position.");
		break;
	case CLEARALL:
		if (!lcd->lcdClear())
			throw std::exception("Failed to clear LCD.");
		break;
	case CLEAREOL:
		if (!lcd->lcdClearToEOL())
			throw std::exception("Failed to clear to EOL in LCD");
		break;
	}
}

//Loads client with username and tweetCount.
void Simulation::loadClient(const char* username, int tweetCount) {
	tc->newUsername(username);

	if (tweetCount)
		tc->newTweetCount(tweetCount);

	//Shows username in LCD.
	*lcd << (unsigned char*)username;

	loaded = true;
}
//Requests tweets.
void Simulation::performRequest(void) {
	//Creates timer resources for dynamic loading identifier.
	ALLEGRO_TIMER* timer;
	ALLEGRO_EVENT_QUEUE* queue;
	ALLEGRO_EVENT ev;

	//Sets variables to use in function.
	bool going = true;

	int sign = 0;

	//Attempts to set timer resources.
	try {
		timer = al_create_timer(0.1);
		queue = al_create_event_queue();
		al_register_event_source(queue, al_get_timer_event_source(timer));
		al_start_timer(timer);
		while (going) {
			//Checks if there's been a timer event.
			if (al_get_next_event(queue, &ev)) {
				loadingMessage(&sign);
			}

			//Performs tweet request.
			going = tc->requestTweets();
		}

		//Destroys timer resources.
		al_destroy_timer(timer);
		al_destroy_event_queue(queue);

		//Clears LCD and writes first tweet's date and content.
		lcd->lcdClear();
		*lcd << (unsigned char*)tc->getTweets()[0].getDate().c_str();
		*lcd << (unsigned char*)tc->getTweets()[0].getContent().c_str();
	}
	//If it's a Twitter error, it shows it on display.
	catch (API_request_error& e) {
		lcd->lcdClear();
		*lcd << (unsigned char*)e.what();
	}

	//If it's another type of error, it throws it again.
	catch (std::exception& e) {
		throw e;
	}
}

//Getter.
bool Simulation::isRunning(void) { return running; }

//Shows next tweet or shows error message in lcd if there are no more tweets.
void Simulation::showNextTweet() {
	try {
		//Notice of last tweet.
		if (tweetNumber >= (tc->getTweets().size() - 1)) {
			tweetNumber = tc->getTweets().size();
			lcd->lcdClear();
			*lcd << (unsigned char*)"No more tweets.";
		}

		//Shows next tweet and updates tweetNumber.
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
		//Does nothing if there isn't a previous tweet.
		if (tweetNumber <= 0) {
			return;
		}

		//Shows previous tweet and updates tweetNumber.
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

void Simulation::loadingMessage(int* dots) {
	cursorPosition temp;
	temp.row = 1;
	temp.column = 0;
	//Clears second row.
	lcd->lcdSetCursorPosition(temp);
	lcd->lcdClearToEOL();

	//Writes "Requesting" plus 1,2 or 3 dots.
	*lcd << (unsigned char*)"Requesting";
	for (int i = 0; i < *dots; i++)
		*lcd << (unsigned char)'.';

	//Updates number of dots.
	if (*dots == 3)
		*dots = -1;

	(*dots)++;
}