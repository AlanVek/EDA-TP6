#pragma once
#include <iostream>
#include <allegro5/allegro.h>

class GraphicClass {
public:

	//GraphicClass constructor.
	GraphicClass(unsigned int width, unsigned int height);

	bool createDisplay(void);

	ALLEGRO_DISPLAY*& getDisplay(void);

	~GraphicClass();

	bool initializeBitmaps(unsigned int width_, unsigned int height_);

	bool createBitmap(unsigned int W, unsigned int H, const char* bitName, ALLEGRO_BITMAP*& thisBit);

	void drawBitmap(ALLEGRO_BITMAP*& thisBit, float X, float Y);

	ALLEGRO_BITMAP*& getBabyBit(void);

	ALLEGRO_BITMAP*& getGrownBit(void);

	ALLEGRO_BITMAP*& getGoodBit(void);

	ALLEGRO_BITMAP*& getBackgrBit(void);

	ALLEGRO_BITMAP*& getFoodBit(void);

private:

	ALLEGRO_DISPLAY* display;
	unsigned int width;
	unsigned int height;
	ALLEGRO_BITMAP* babyBit, * grownBit, * goodOldBit, * backgroundBit, *foodBit;
};