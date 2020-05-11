#include <iostream>
#include "Simulation/Simulation.h"

int main() {
	int result = -1;

	int step = 0;

	int cc = 6;
	//Attempts to create simulation and perform.
	try {
		Simulation sim;

		while (sim.isRunning()) {
			/*Dispatch should get cc from GUI, not from console.
			It's just to test it.*/
			sim.dispatch(cc);

			/*Toy dispatch sequence.
			Loads username, requests tweets,
			exits main*/
			switch (cc) {
			case 6:
				al_rest(1);
				cc = 5;
				break;
			case 5:
				al_rest(1);
				if (step)
					cc = 7;
				else {
					cc = 6;
					step++;
					al_rest(1);
				}
				break;
			}
		}

		result = 0;
	}

	//If there's and error, it shows the error's message.
	catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	}

	return result;
}