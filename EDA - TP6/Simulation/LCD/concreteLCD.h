#pragma once
#include "basicLCD.h"
#include "allegro5/allegro.h"
#include "allegro5/allegro_ttf.h"
#include <string>

class concreteLCD : public basicLCD {
public:
	concreteLCD();
	~concreteLCD();
	virtual bool lcdInitOk();
	virtual int lcdGetError();
	virtual bool lcdClear();
	virtual bool lcdClearToEOL();

	virtual basicLCD& operator << (const unsigned char c);
	virtual basicLCD& operator << (const unsigned char* c);

	virtual bool lcdMoveCursorUp();
	virtual bool lcdMoveCursorDown();
	virtual bool lcdMoveCursorRight();
	virtual bool lcdMoveCursorLeft();

	virtual bool lcdSetCursorPosition(const cursorPosition);
	virtual cursorPosition lcdGetCursorPosition();

protected:
	int cadd;
	virtual void lcdUpdateCursor();

private:
	void paintCursor(bool);
	//int updatedPosition(const unsigned char*);
	int lastCadd;
	void setAllegro(void);
	void eraseLetter();

	bool isCharSupported(const char);

	ALLEGRO_DISPLAY* display;
	bool initOk;
	int errorCode;

	ALLEGRO_FONT* font;

	ALLEGRO_COLOR background;
	ALLEGRO_COLOR fontColor;

	std::string supportedChars;
};
