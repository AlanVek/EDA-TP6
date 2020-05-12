#include <iostream>
#include "Simulation/Simulation.h"

int main() {
	int result = -1;

	//Attempts to create simulation and perform.
	try {
		Simulation sim;

		sim.getFirstData();

		while (sim.isRunning()) {
			sim.dispatch();
		}

		result = 0;
	}

	//If there's and error, it shows the error's message.
	catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	}

	return result;
}