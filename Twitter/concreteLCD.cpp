#include "concreteLCD.h"
#include "MyErrors.h"
#include <allegro5/allegro_primitives.h>
namespace {
	const unsigned int width = 800;
	const unsigned int height = 192;
	const unsigned int lcdWidth = 16;
	const unsigned int lcdHeight = 2;
	const unsigned int letterWidth = width / lcdWidth;
	const unsigned int letterHeight = height / lcdHeight;

	const char* fontName = "LCD-N.ttf";
}

namespace errors {
	const unsigned int al_fail_code = 1;
	const char* al_fail_str = "Failed to initialize Allegro.";

	const unsigned int al_mouse_fail_code = 2;
	const char* al_mouse_fail_str = "Failed to initialize mouse addon.";

	const unsigned int al_keyboard_fail_code = 3;
	const char* al_keyboard_fail_str = "Failed to initialize keyboard addon.";

	const unsigned int al_primitives_fail_code = 4;
	const char* al_primitives_fail_str = "Failed to initialize primitives addon.";

	const unsigned int al_ttf_fail_code = 5;
	const char* al_ttf_fail_str = "Failed to initialize ttf addon.";

	const unsigned int al_font_load_fail_code = 6;
	const char* al_font_load_fail_str = "Failed to load font.";

	const unsigned int al_display_fail_code = 7;
	const char* al_display_fail_str = "Failed to create display";

	const unsigned int al_color_fail_code = 8;
	const char* al_color_fail_str = "Failed while attempting clear_to_color.";

	const unsigned int clear_EOL_fail_code = 9;
	const char* clear_EOL_fail_str = "Failed while attempting to clear to EOL.";
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
		al_clear_to_color(al_map_rgb(255, 255, 255));
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

basicLCD& concreteLCD::operator << (const unsigned char c) {
	double posX = letterWidth * (cadd % lcdWidth);
	double posY = letterHeight * (cadd / lcdWidth);

	al_draw_text(font, al_map_rgb(0, 0, 0), posX, posY, 0, (char*)&c);

	al_flip_display();

	return *this;
}
basicLCD& concreteLCD::operator << (const unsigned char* c) { return *this; };

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
	return attemptUpdate();
};
cursorPosition concreteLCD::lcdGetCursorPosition() {
	cursorPosition temp;

	temp.row = (cadd / lcdWidth);
	temp.column = cadd % lcdWidth;

	return temp;
};

void concreteLCD::updateCursor() { int a = 1; };

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
	else if (!al_init_primitives_addon()) {
		throw AllegroError(errors::al_primitives_fail_str, errors::al_primitives_fail_code);
	}
	else if (!al_init_ttf_addon()) {
		throw AllegroError(errors::al_ttf_fail_str, errors::al_ttf_fail_code);
	}
	else if (!(display = al_create_display(width, height))) {
		throw AllegroError(errors::al_display_fail_str, errors::al_display_fail_code);
	}
	else if (!(font = al_load_ttf_font("LCD-N.ttf", letterHeight, 0))) {
		throw AllegroError(errors::al_font_load_fail_str, errors::al_font_load_fail_code);
	}

	al_clear_to_color(al_map_rgb(255, 255, 255));
	al_flip_display();
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

concreteLCD::~concreteLCD() {
	if (display)
		al_destroy_display(display);
}