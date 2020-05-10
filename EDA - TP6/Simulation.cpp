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
	/*case 1: ... */
	}
}

//Loads client with username and tweetCount.
void Simulation::loadClient(const char* username, int tweetCount) {
	tc->newUsername(username);

	if (tweetCount)
		tc->newTweetCount(tweetCount);

	//Shows username in LCD.
	*lcd << (unsigned char*)username;
}

//Requests tweets.
void Simulation::performRequest(void) {
	tc->requestTweets();
}

//Getter.
bool Simulation::isRunning(void) { return running; }