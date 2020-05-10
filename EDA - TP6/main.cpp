#include <iostream>
#include "Simulation.h"

int main() {
	int result = -1;

	int cc;
	//Attempts to create simulation and perform.
	try {
		Simulation sim;

		while (sim.isRunning()) {
			/*Dispatch should get cc from GUI, not from console.
			It's just to test it.*/
			std::cin >> cc;
			sim.dispatch(cc);
		}

		result = 0;
	}

	//If there's and error, it shows the error's message.
	catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	}

	return result;
}