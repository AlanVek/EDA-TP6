#include "concreteLCD.h"
#include "MyErrors.h"
namespace {
	const unsigned int width = 400;
	const unsigned int height = 150;
	const unsigned int lcdWidth = 16;
	const unsigned int lcdHeight = 2;
	const ALLEGRO_COLOR white = al_map_rgb(255, 255, 255);
	const ALLEGRO_COLOR green = al_map_rgb(0, 255, 0);
}

namespace errors {
	const unsigned int al_fail_code = 1;
	const char* al_fail_str = "Failed to initialize Allegro.";

	const unsigned int al_mouse_fail_code = 2;
	const char* al_mouse_fail_str = "Failed to initialize mouse addon.";

	const unsigned int al_keyboard_fail_code = 3;
	const char* al_keyboard_fail_str = "Failed to initialize keyboard addon.";

	const unsigned int al_display_fail_code = 4;
	const char* al_display_fail_str = "Failed to create display";

	const unsigned int al_color_fail_code = 5;
	const char* al_color_fail_str = "Failed while attempting clear_to_color.";

	const unsigned int clear_EOL_fail_code = 5;
	const char* clear_EOL_fail_str = "Failed while attempting clear_to_color.";
}

concreteLCD::concreteLCD() : cadd(1), initOk(false) {
	display = nullptr;

	try {
		setAllegro();
		initOk = true;
	}
	catch (AllegroError& e) {
		errorCode = e.code();
	}
};

bool concreteLCD::lcdInitOk() { return initOk; }

int concreteLCD::lcdGetError() { return errorCode; };
bool concreteLCD::lcdClear() {
	bool result = false;
	try {
		al_clear_to_color(white);
		result = true;
		cadd = 1;
	}
	catch (std::exception& e) {
		errorCode = errors::al_color_fail_code;
	}

	return result;
};
bool concreteLCD::lcdClearToEOL() {
	int aux = cadd;
	char c = 0;
	bool result = false;
	try {
		while (cadd % width) {
			*this << c;
			cadd++;
		}
		cadd = aux;
		result = true;
	}
	catch (std::exception& e) {
		errorCode = errors::clear_EOL_fail_code;
		cadd = aux;
	}

	return result;
};

basicLCD& concreteLCD::operator << (const unsigned char c) {};
basicLCD& concreteLCD::operator << (const unsigned char* c) {};

bool concreteLCD::lcdMoveCursorUp() {
	if (cadd > lcdWidth)
		cadd -= lcdWidth;

	return attemptUpdate();
};
bool concreteLCD::lcdMoveCursorDown() {
	if (cadd <= lcdWidth)
		cadd += lcdWidth;

	return attemptUpdate();
};
bool concreteLCD::lcdMoveCursorRight() {
	if (cadd < lcdWidth * lcdHeight)
		cadd++;

	return attemptUpdate();
};
bool concreteLCD::lcdMoveCursorLeft() {
	if (cadd > 1)
		cadd--;
	return attemptUpdate();
};

bool concreteLCD::lcdSetCursorPosition(const cursorPosition pos) {
	if (pos.column < lcdWidth && pos.row < height)
		cadd = pos.row * lcdWidth + pos.column + 1;
	updateCursor();
};
cursorPosition concreteLCD::lcdGetCursorPosition() {
	cursorPosition temp;

	temp.row = (int)(cadd / lcdWidth);
	temp.column = cadd % lcdWidth;

	return temp;
};

void concreteLCD::updateCursor() {};

/*Attempts to initialize Allegro and its addons.*/
void concreteLCD::setAllegro(void) {
	if (!al_init()) {
		throw AllegroError(errors::al_fail_str, errors::al_fail_code);
	}

	else if (!al_install_mouse()) {
		throw AllegroError(errors::al_mouse_fail_str, errors::al_mouse_fail_code);
	}

	else if (!al_install_keyboard()) {
		throw AllegroError(errors::al_keyboard_fail_str, errors::al_keyboard_fail_code);
	}
	else if (!(display = al_create_display(width, height))) {
		throw AllegroError(errors::al_display_fail_str, errors::al_display_fail_code);
	}
}

bool concreteLCD::attemptUpdate() {
	bool result = false;

	try {
		updateCursor();
		result = true;
	}
	catch (AllegroError& e) {
		errorCode = e.code();
	}
	return result;
}