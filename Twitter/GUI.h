#pragma once

#include <allegro5/allegro.h>

class GUI{

public:

	GUI();

	~GUI();

	bool GUI_Game_Loop(void);

	void GUI_setUp(void);

	bool GUI_firstLoop(void);

	void setDependencies(void);

	float relSpeed, maxSpeed;
	float randomJL, babyDeathProb, grownDeathProb, goodOldDeathProb;
	int foodCount, blobCount, smellRadius;
	bool mode, pause;

protected:
	ALLEGRO_DISPLAY* guiDisp;
	ALLEGRO_EVENT_QUEUE* guiQueue;
	ALLEGRO_EVENT guiEvent;
	bool checkGUIEvents(void);
};

