#include <iostream>
#include "Simulation.h"

/*********************************************

	 Toy main to test TwitterClient and LCD.

*********************************************/
int main() {
	int result = -1;
	try {
		Simulation sim;

		while (sim.isRunning()) {
			sim.dispatch();
		}

		result = 0;
	}
	catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	}

	return result;
}