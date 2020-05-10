#pragma once
#include "Twitter/TwitterClient.h"
#include "LCD/concreteLCD.h"
class Simulation {
public:
	Simulation(void);
	~Simulation(void);

	void dispatch();

	bool isRunning(void);

private:
	void loadClient(const char*, int = NULL);
	void performRequest(void);

	basicLCD* lcd;
	TwitterClient* tc;
	/*GUI* gui;*/
	bool running;
};
