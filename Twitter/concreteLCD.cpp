#include "concreteLCD.h"

concreteLCD::concreteLCD() : cadd(1) {};

bool concreteLCD::lcdInitOk() {};

int concreteLCD::lcdGetError() {};
bool concreteLCD::lcdClear() {};
bool concreteLCD::lcdClearToEOL() {};

basicLCD& concreteLCD::operator << (const unsigned char c) {};
basicLCD& concreteLCD::operator << (const unsigned char* c) {};

bool concreteLCD::lcdMoveCursorUp() {};
bool concreteLCD::lcdMoveCursorDown() {};
bool concreteLCD::lcdMoveCursorRight() {};
bool concreteLCD::lcdMoveCursorLeft() {};

bool lcdSetCursorPosition(const cursorPosition) {};
cursorPosition lcdGetCursorPosition() {};

void concreteLCD::updateCursor() {};