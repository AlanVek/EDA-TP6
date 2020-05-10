#pragma once
#include "EventClass.h"
#include "TimeClass.h"
#include "GraphicClass.h"
#include "Blob.h"
#include "Food.h"
#include "GUI.h"

#define MAXBLOBAMOUNT 10000
#define FOODAMOUNT 500
//Temporary values to check correct initialization.
/**************************************************/
#define defaultDisplay true

#define defaultWidth 900
#define defaultHeight 470
#define defaultFPS 11.0
#define defaultBlobAmount 1
#define defaultMaxSpeed 0
#define defaultRelativeSpeed 0
#define defaultMode 1
#define defaultFoodAmount 0
#define defaultSmellRadius 0
#define defaultDeathProb 0.0
#define randomJiggleLimit 0
/**************************************************/
class Simulation {
public:

	//Simulation constructor.
	Simulation(unsigned int width_ = defaultWidth, unsigned int height_ = defaultHeight, double FPS_ = defaultFPS,
		unsigned int blobAmount_ = defaultBlobAmount, unsigned int generalMaxSpeed_ = defaultMaxSpeed,
		float generalRelativeSpeed_ = defaultRelativeSpeed, int mode_ = defaultMode, int foodAmount_ = defaultFoodAmount);

	bool setSimulation(bool displayCreation = defaultDisplay);

	bool setAllegro(void);

	GraphicClass* getGraphicControl(void);
	TimeClass* getTimeControl(void);
	EventClass* getEventControl(void);

	bool initializeAll(void);

	unsigned int getBlobAmount(void);

	Blob** getAllBlobs(void);

	bool initializeFood(int foodCount_);
	bool initializeBlob(void);

	void drawAccordingBitmap(Blob* thisBlob);

	bool moveCycle(void);

	bool blobBirth(newBirth thisBirth);
	void blobDeath(int index);

	void drawItAll(void);

	void Merges(void);

	void getData(void);

	bool getFirstData(void);

	~Simulation();

	GUI* getGUI(void);

	void deleteFood(int amount);

private:

	//Datos miembro de Simulation.
	GraphicClass* graphicControl;
	TimeClass* timeControl;
	EventClass* eventControl;
	Blob* allBlobs[MAXBLOBAMOUNT];
	Food* allFoods[FOODAMOUNT];

	unsigned int blobAmount, foodCount;
	unsigned int width, height;
	int mode;

	//For mode 1.
	unsigned int generalMaxSpeed;

	//For mode 2.
	float generalRelativeSpeed;

	double FPS;

	GUI thisGUI;
};