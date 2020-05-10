#pragma once

struct cursorPosition {
	int row;
	int column;
};

class basicLCD
{
public:
	basicLCD();
	~basicLCD();

	virtual bool lcdInitOk() = 0;
	virtual int lcdGetError() = 0;
	virtual bool lcdClear() = 0;
	virtual bool lcdClearToEOL() = 0;

	virtual basicLCD& operator << (const unsigned char c) = 0;
	virtual basicLCD& operator << (const unsigned char* c) = 0;

	virtual bool lcdMoveCursorUp() = 0;
	virtual bool lcdMoveCursorDown() = 0;
	virtual bool lcdMoveCursorRight() = 0;
	virtual bool lcdMoveCursorLeft() = 0;

	virtual bool lcdSetCursorPosition(const cursorPosition) = 0;
	virtual bool lcdGetCursorPosition() = 0;
protected:
	int cadd;
};
