#pragma once
#include "basicLCD.h"
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
	virtual void updateCursor();
};
