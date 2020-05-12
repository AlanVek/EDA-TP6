#include "Simulation.h"
#include <exception>
#include "Twitter/API_request_error.h"

namespace {
	const int loadingDotsNumber = 3;
}

//Simulation constructor.
Simulation::Simulation(void) : running(true), loaded(loadState::notLoaded), tweetNumber(NULL)
{
	//Attempts to create new LCD and TwitterClient.
	lcd = new (std::nothrow) concreteLCD;
	tc = new (std::nothrow) TwitterClient;

	//Requests TwitterClient token.
	tc->requestToken();

	gui = new (std::nothrow) GUI;

	//Checks for errors.
	if (!lcd)
		throw std::exception("Failed to allocate memory for lcd.");
	if (!lcd->lcdInitOk()) {
		std::string errStr = "Failed to create lcd instance. Code: " + std::to_string(lcd->lcdGetError());
		throw std::exception(errStr.c_str());
	}
	if (!tc)
		throw std::exception("Failed to allocate memory for Twitter client.");
	if (!gui)
		throw std::exception("Failed to allocate memory for GUI.");
}

//Gets first data input from GUI.
void Simulation::getFirstData(void) {
	/*If user asked to leave, running will be False
	and program will exit.*/
	running = gui->firstRun();

	//Otherwise, it loads client and requests tweets.
	if (running) {
		loadClient(gui->getUsername().c_str(), gui->getTweetCount());
		performRequest();
	}
}

//Simulation destructor. Deletes used resources.
Simulation::~Simulation() {
	if (lcd)
		delete lcd;
	if (tc)
		delete tc;
	if (gui)
		delete gui;
}

//Polls GUI and dispatches according to button code.
void Simulation::dispatch() {
	codes code = gui->checkStatus();

	switch (code) {
	case codes::NOTHING:
		/*Roll display...*/
		/*Or show next tweet...*/
		break;
	case codes::DOWN:
		if (!lcd->lcdMoveCursorDown())
			throw std::exception("Failed to move cursor down.");
		break;
	case codes::UP:
		if (!lcd->lcdMoveCursorUp())
			throw std::exception("Failed to move cursor up.");
		break;
	case codes::LEFT:
		if (!lcd->lcdMoveCursorLeft())
			throw std::exception("Failed to move cursor left.");
		break;
	case codes::RIGHT:
		if (!lcd->lcdMoveCursorRight())
			throw std::exception("Failed to move cursor right.");
		break;
	case codes::REQUEST:
		tweetNumber = 0;
		loadClient(gui->getUsername().c_str(), gui->getTweetCount());
		performRequest();
		break;
	case codes::END:
		running = false;
		break;
	case codes::NEXT:
		if (loaded == loadState::requestedTweets)
			showNextTweet();
		break;
	case codes::PREVIOUS:
		if (loaded == loadState::requestedTweets)
			showPreviousTweet();
		break;
	case codes::CLEARALL:
		if (!lcd->lcdClear())
			throw std::exception("Failed to clear LCD.");
		break;
	case codes::CLEAREOL:
		if (!lcd->lcdClearToEOL())
			throw std::exception("Failed to clear to EOL in LCD.");
		break;
	default:
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
		if (!lcd->lcdClear())
			throw std::exception("Failed to clear LCD.");
		*lcd << (unsigned char*)username;

		loaded = loadState::loadedTC;
	}
	catch (API_request_error& e) {
		if (!lcd->lcdClear())
			throw std::exception("Failed to clear LCD.");
		*lcd << (unsigned char*)e.what();
	}
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

			//Checks if user cancelled request.
			if (gui->checkStatus() == codes::CANCEL)
				going = false;
		}

		//Destroys timer resources.
		al_destroy_timer(timer);
		al_destroy_event_queue(queue);

		//Clears LCD and writes first tweet's date and content.
		if (!lcd->lcdClear())
			throw std::exception("Failed to clear LCD");
		if (tc->getTweets().size()) {
			*lcd << (unsigned char*)tc->getTweets()[0].getDate().c_str();
			*lcd << (unsigned char*)tc->getTweets()[0].getContent().c_str();
			loaded = loadState::requestedTweets;
		}
		else {
			*lcd << (unsigned char*)"No tweets.";
			loaded = loadState::notLoaded;
		}
	}
	//If it's a Twitter error, it shows it on display.
	catch (API_request_error& e) {
		if (!lcd->lcdClear())
			throw std::exception("Failed to clear LCD");
		*lcd << (unsigned char*)e.what();
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
			if (!lcd->lcdClear())
				throw std::exception("Failed to clear LCD");
			*lcd << (unsigned char*)"No more tweets.";
		}

		//Shows next tweet and updates tweetNumber.
		else {
			tweetNumber++;
			if (!lcd->lcdClear())
				throw std::exception("Failed to clear LCD");
			*lcd << (unsigned char*)tc->getTweets()[tweetNumber].getDate().c_str();
			*lcd << (unsigned char*)tc->getTweets()[tweetNumber].getContent().c_str();
		}
	}
	catch (std::exception&) {
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
			if (!lcd->lcdClear())
				throw std::exception("Failed to clear LCD.");
			*lcd << (unsigned char*)tc->getTweets()[tweetNumber].getDate().c_str();
			*lcd << (unsigned char*)tc->getTweets()[tweetNumber].getContent().c_str();
		}
	}
	catch (std::exception&) {
		return;
	}
}
void Simulation::loadingMessage(int* dots) {
	cursorPosition temp;
	temp.row = 1;
	temp.column = 0;

	if (!*dots) {
		//Clears second row.
		if (!lcd->lcdSetCursorPosition(temp))
			throw std::exception("Failed to set cursor's position.");
		if (!lcd->lcdClearToEOL())
			throw std::exception("Failed to clear to EOL in LCD.");

		//Writes "Requesting" in LCD and then adds 1, 2 or 3 dots.
		*lcd << (unsigned char*)"Requesting";
	}
	else {
		*lcd << (unsigned char)'.';

		if (*dots == loadingDotsNumber)
			*dots = -1;
	}
	//Updates number of dots.
	(*dots)++;
}