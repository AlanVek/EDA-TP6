#include "concreteLCD.h"
#include "MyErrors.h"
namespace {
	const unsigned int width = 400;
	const unsigned int height = 150;
	const unsigned int lcdWidth = 16;
	const unsigned int lcdHeight = 2;
}

concreteLCD::concreteLCD() : cadd(1), initOk(false) {
	display = nullptr;

	try {
		setAllegro();
		initOk = true;
	}
	catch (AllegroError& e) {
		errorStr = e.what();
		errorCode = e.code();
	}
};

bool concreteLCD::lcdInitOk() { return initOk; }

int concreteLCD::lcdGetError() { return errorCode; };
bool concreteLCD::lcdClear() {};
bool concreteLCD::lcdClearToEOL() {};

basicLCD& concreteLCD::operator << (const unsigned char c) {};
basicLCD& concreteLCD::operator << (const unsigned char* c) {};

bool concreteLCD::lcdMoveCursorUp() { if (cadd > lcdWidth) cadd -= width; };
bool concreteLCD::lcdMoveCursorDown() { if (cadd <= lcdWidth) cadd += width; };
bool concreteLCD::lcdMoveCursorRight() { if (cadd < lcdWidth * lcdHeight) cadd++; };
bool concreteLCD::lcdMoveCursorLeft() { if (cadd > 0) cadd--; };

bool concreteLCD::lcdSetCursorPosition(const cursorPosition pos) {
	if (pos.column < width && pos.row < height)
		cadd = pos.column * pos.row + 1;
	updateCursor();
};
cursorPosition concreteLCD::lcdGetCursorPosition() {
	cursorPosition temp;

	temp.row = (int)cadd / width;
	temp.column = cadd % width;

	return temp;
};

void concreteLCD::updateCursor() {};

/*Attempts to initialize Allegro and its addons.*/
void concreteLCD::setAllegro(void) {
	if (!al_init()) {
		throw AllegroError("Failed to initialize Allegro.", 1);
	}

	else if (!al_install_mouse()) {
		throw AllegroError("Failed to initialize mouse addon.", 2);
	}

	else if (!al_install_keyboard()) {
		throw AllegroError("Failed to initialize keyboard addon.", 3);
	}
	else if (!(display = al_create_display(width, height))) {
		throw AllegroError("Failed to create display.", 4);
	}
}