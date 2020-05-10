#include "Simulation.h"
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include "Templates.h"
#include <typeinfo>
#include "GrownBlob.h"
#include "GoodOldBlob.h"
#include "BabyBlob.h"
#include <allegro5/keyboard.h>
#include <random>


using namespace std;

//Simulation constructor.
Simulation::Simulation(unsigned int width_, unsigned int height_, double FPS_, unsigned int blobAmount_, 
	unsigned int generalMaxSpeed_, float generalRelativeSpeed_, int mode_, int foodCount_) : 

	width(width_), height(height_), FPS(FPS_), blobAmount(blobAmount_), generalMaxSpeed(generalMaxSpeed_),
	generalRelativeSpeed(generalRelativeSpeed_), mode(mode_), foodCount(foodCount_){

	graphicControl = nullptr;
	timeControl = nullptr;
	eventControl = nullptr;
}

bool Simulation::initializeAll(void) {
	bool result = true;

	/*Allegro initialization error check*/
	if (!setAllegro()) {
		cout << "Failed to initialize Allegro.\n";
		result = false;
	}

	/*Initialization of graphic resources. Checks for errors.
	True parameter indicates to create a new display. */
	else if (!setSimulation(true)) {
		cout << "Failed to set simulation.\n";
		result = false;
	}

	thisGUI.GUI_setUp();
	thisGUI.setDependencies();
	
	return result;
}

/*Initializes Allegro and its addons. Return false and prints
the correspondent message if any process failed. */
bool Simulation::setAllegro(void) {
	bool result = true;
	if (!al_init()) {
		result = false;
	}

	//Attempts to initialize image addon.
	else if (!al_init_image_addon()) {
		result = false;
	}
	else if (!al_init_primitives_addon()) {
		result = false;
	}
	else if (!al_install_mouse()) {
		result = false;
	}

	else if (!al_install_keyboard()) {
		result = false;
	}

	return result;
}

/*Creates needed resources and checks for initialization errors.
If there's been one, returns false. If not, returns true. 
If displayCreation is true, it creates a new display.*/
bool Simulation::setSimulation(bool displayCreation) {

	bool result = true;

	graphicControl = new (nothrow) GraphicClass(width, height);
	eventControl = new (nothrow) EventClass();
	timeControl = new (nothrow) TimeClass();
	if (!graphicControl) {
		result = false;
	}
	else if (!eventControl) {
		result = false;
	}
	else if (!timeControl) {
		result = false;
	}
	//Attempts to create event queue.
	else if (!eventControl->createEventQueue()) {
		result = false;
	}

	//Attempts to create timer.
	else if (!timeControl->createTimer(FPS)) {
		result = false;
	}

	//Attempts to create display (if requested).
	else if (displayCreation && !graphicControl->createDisplay()) {
		result = false;
	}
	//Attempts to create bitmaps.
	else if (!graphicControl->initializeBitmaps(width, height)) {
		result = false;
	}

	//Sets event source for timer and shows drawings.
	if (result) {
		al_register_event_source(eventControl->getQueue(), al_get_timer_event_source(timeControl->getTimer()));
		al_register_event_source(eventControl->getQueue(), al_get_display_event_source(graphicControl->getDisplay()));
	}
	return result;
}

//Class getters.
GraphicClass* Simulation::getGraphicControl(void) { return graphicControl; }
TimeClass* Simulation::getTimeControl(void) { return timeControl; }
EventClass* Simulation::getEventControl(void) { return eventControl; }
unsigned int Simulation::getBlobAmount(void) { return blobAmount; }
Blob** Simulation::getAllBlobs(void) { return allBlobs; }

GUI* Simulation::getGUI(void) { return &thisGUI; }

//Creates food, loads bitmaps and draws them.
bool Simulation::initializeFood (int foodCount_){

	bool result = true;
	for (int i = 0; i < foodCount_; i++) {
		if (!(allFoods[foodCount+i] = new (nothrow) Food(width, height)))
			result = false;
	}
	return result;
}

//Creates blobs, loads bitmaps and draws them.
bool Simulation::initializeBlob() {
	float xPos, yPos;
	bool result = true;
	float maxSpeed_ = 0;
	for (int i = 0; i < blobAmount; i++) {
		
		if (thisGUI.mode)
			maxSpeed_ = static_cast <double> ((rand()) / (static_cast <double> (RAND_MAX)) * (thisGUI.maxSpeed));
		else
			maxSpeed_ = thisGUI.maxSpeed;
		if (!(allBlobs[i] = new (nothrow) BabyBlob(width, height, maxSpeed_,
			0, thisGUI.smellRadius, thisGUI.babyDeathProb)))
			result = false;
	}
	return result;
}

bool Simulation::moveCycle(void) {

	float xPos, yPos;

	int hasBeenEaten;

	newBirth newBaby;

	al_set_target_backbuffer(graphicControl->getDisplay());


	//First, it checks for blobDeaths and adjusts blobAmount accordingly.
	for (int i = 0; i < blobAmount; i++) {
		if (allBlobs[i]->checkBlobDeath()) {
			blobDeath(i);
			i--;
		}
	}

	//Then, every blob smells for food (and adjusts angles).
	for (int i = 0; i < blobAmount; i++) 
		allBlobs[i]->blobSmell(allFoods, foodCount);

	//Separately, so as to first finish calculations, the blobs move.
	for (int i = 0; i < blobAmount; i++) {
		allBlobs[i]->blobMove(width, height);

		//Checks for eaten food.
		hasBeenEaten = allBlobs[i]->blobFeeding(allFoods, foodCount,&newBaby);
		if ( hasBeenEaten != -1)
			allFoods[hasBeenEaten]->NewPosition(width, height);

		//Checks for potential blobBirth.
		if (newBaby.birthFlag) {
			if (!blobBirth(newBaby))
				return false;
		}
	}

	//Checks for merges and does the necessary changes.
	Merges();

	drawItAll();

	al_flip_display();
	return true;
}

void Simulation::drawAccordingBitmap(Blob* thisBlob) {
	float xPos = thisBlob->getBlobPosition()->x;
	float yPos = thisBlob->getBlobPosition()->y;

	float typeID = typeid (*thisBlob).hash_code();

	//If it's a BabyBlob, it draws the babyBit. 
	if (typeID == typeid (BabyBlob).hash_code()) {
		graphicControl->drawBitmap(graphicControl->getBabyBit(), xPos, yPos);
	}

	//If it's a GrownBlob, it draws the grownBit.
	else if (typeID == typeid (GrownBlob).hash_code()) {
		graphicControl->drawBitmap(graphicControl->getGrownBit(),xPos, yPos);
	}

	//If it's a GoodOldBlob, it draws the goodBit.
	else if (typeID == typeid (GoodOldBlob).hash_code()) {
		graphicControl->drawBitmap(graphicControl->getGoodBit(),xPos, yPos);
	}
}


//Checks for overlapped same-age-group blobs and blobMerges them.
void Simulation::Merges() {

	float xPos1, xPos2;
	float yPos1, yPos2;

	float typeID, randomJ;
	float xDist, yDist;
	int thisMerge;
	if (blobAmount) {
		for (int i = 0; i < blobAmount - 1; i++) {

			typeID = typeid (*allBlobs[i]).hash_code();		

			//First, checks if allBlobs{i] can actually merge (is not a GoodOldBlob).
			if (typeID != typeid (GoodOldBlob).hash_code()) {

				//xPos1 and yPos1 are the coordenates of allBlobs[i]-
				xPos1 = allBlobs[i]->getBlobPosition()->x;
				yPos1 = allBlobs[i]->getBlobPosition()->y;

				//Each iteration has a thisMerge parameter, which serves to do the final speed and direction average.
				thisMerge = 1;

				//For every blob in the array, the loop iterates through the rest of the array (going forward).
				for (int j = i + 1; j < blobAmount; j++) {

					//Coordenates of another (j>i) blob.
					xPos2 = allBlobs[j]->getBlobPosition()->x;
					yPos2 = allBlobs[j]->getBlobPosition()->y;

					xDist = xPos2 - xPos1;
					yDist = yPos2 - yPos1;

					//If coordenates and type match, they have to merge.
					if (abs(xDist) < allBlobs[i]->getBitmapWidth() && abs(yDist) < allBlobs[i]->getBitmapWidth()
						&& typeID == typeid(*allBlobs[j]).hash_code()) {

						//Adds to allBlobs[i] the speed and direction of allBlobs[j].
						allBlobs[i]->willMerge(allBlobs[j]);

						//It treats the blobMerge as the death of allBlobs[j] (read specifications above).
						blobDeath(j);
						j--;

						//Increments the thisMerge parameter associated to allBlobs[i].
						thisMerge++;
					}
				}

				if (thisMerge > 1) {
					/*Once allBlobs[i] has the added speeds and directions of all the blobs with which it merged,
					hasMerged divides everything by thisMerge to obtain the mean. */
					allBlobs[i]->hasMerged(thisMerge);

					float randomJL = static_cast <double> ((rand()) / (static_cast <double> (RAND_MAX)) * (thisGUI.randomJL));
					//Finally, allBlobs[i] evolves (distinguish between cases).
					if (typeID == typeid(BabyBlob).hash_code()) {
						GrownBlob tempBlob(allBlobs[i], randomJL);

						delete allBlobs[i];
						allBlobs[i] = new (nothrow) GrownBlob(tempBlob);
					}

					else {
						GoodOldBlob tempBlob(allBlobs[i], randomJL);

						delete allBlobs[i];
						allBlobs[i] = new (nothrow) GoodOldBlob(tempBlob);
					}
				}
			}
		}
	}
}

//Attempts to create a new BabyBlob and appends it to the allBlobs array and increment blobAmount.
bool Simulation::blobBirth(newBirth thisBirth) {
	bool result = true;
	if (!(allBlobs[blobAmount] = new (nothrow) BabyBlob(width, height, generalMaxSpeed,
		generalRelativeSpeed, thisGUI.smellRadius, thisGUI.babyDeathProb)))
		result = false;
	
	if (result) {
		//allBlobs[blobAmount]->setPosition(thisBirth);
		blobAmount++;
	}
	return result;
}

//Deletes the dead blob and moves all subsecuent blobs to adjust the array.
void Simulation::blobDeath(int index) {
	delete allBlobs[index];

	for (int i = index; i < blobAmount-1; i++) {
		allBlobs[i] = allBlobs[i + 1];
	}

	blobAmount--;
}

//Draws background, blobs and food.
void Simulation::drawItAll(void) {
	
	//Draws background.
	graphicControl->drawBitmap(graphicControl->getBackgrBit(), 0, 0);

	//Draws blobs.
	for (int i = 0; i < blobAmount; i++) {
		drawAccordingBitmap(allBlobs[i]);
	}

	//Draws food.
	for (int i = 0; i < foodCount; i++) {
		graphicControl->drawBitmap(graphicControl->getFoodBit(), allFoods[i]->getXPosit(), 
			allFoods[i]->getYPosit());
	}
}

//Destructor. Frees memory.
Simulation::~Simulation() {
	
	delete graphicControl;
	delete timeControl;
	delete eventControl;

	deleteArray < Blob* >(allBlobs, blobAmount);

	deleteArray < Food* >(allFoods, foodCount);
}

//Gets data from GUI and updates simulation and blobs parameters.
void Simulation::getData(void) {

	float typeID;
	for (int i = 0; i < blobAmount; i++) {
		typeID = typeid(*allBlobs[i]).hash_code();

		if (typeID == typeid(BabyBlob).hash_code())
			allBlobs[i]->setDeathProb(thisGUI.babyDeathProb);

		if (typeID == typeid(GrownBlob).hash_code())
			allBlobs[i]->setDeathProb(thisGUI.grownDeathProb);

		if (typeID == typeid(GoodOldBlob).hash_code())
			allBlobs[i]->setDeathProb(thisGUI.goodOldDeathProb);

		allBlobs[i]->setNewData(thisGUI.maxSpeed, thisGUI.relSpeed, thisGUI.smellRadius);
	}

	if (thisGUI.foodCount >= foodCount) {
		initializeFood(thisGUI.foodCount - foodCount);
	}
	else{
		deleteFood(foodCount - thisGUI.foodCount);
	}
	foodCount = thisGUI.foodCount;
	
}

//Gets first set of data to initialize blobs and food.
bool Simulation::getFirstData(void) {
	
	bool result = true;
	blobAmount = thisGUI.blobCount;
	foodCount = thisGUI.foodCount;
	mode = thisGUI.mode;
	
	
	//Attempts to Initialize allBlobs to default values (for now) and create bitmaps.
	if (!(initializeBlob())) {
		result = false;
	}
	//Attempts to Initialize allFoods to default values (for now) and create bitmaps.
	else if (!initializeFood(foodCount)) {
		result = false;
	}
	return result;
}

//Deletes food when the user changed from original amount to (<) new amount.
void Simulation::deleteFood(int amount) {
	int temp = foodCount;
	for (int i = foodCount - amount; i < foodCount; i++) {
		delete allFoods[i];
		temp--;
	}
	foodCount = temp;
}