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

	const char* fontName = "LCD-6.ttf";

	const int spaceASCII = 32;

	const unsigned int lineWidth = width / 160;

	ALLEGRO_COLOR white;
	ALLEGRO_COLOR black;
}

namespace errors {
	const unsigned int al_fail_code = 1;
	const char* al_fail_str = "Failed while attempting to initialize Allegro.";

	const unsigned int al_mouse_fail_code = 2;
	const char* al_mouse_fail_str = "Failed while attempting to initialize mouse addon.";

	const unsigned int al_keyboard_fail_code = 3;
	const char* al_keyboard_fail_str = "Failed while attempting to initialize keyboard addon.";

	const unsigned int al_primitives_fail_code = 4;
	const char* al_primitives_fail_str = "Failed while attempting to initialize primitives addon.";

	const unsigned int al_ttf_fail_code = 5;
	const char* al_ttf_fail_str = "Failed while attempting to initialize ttf addon.";

	const unsigned int al_font_load_fail_code = 6;
	const char* al_font_load_fail_str = "Failed while attempting to load font.";

	const unsigned int al_display_fail_code = 7;
	const char* al_display_fail_str = "Failed while attempting to create display";

	const unsigned int al_color_fail_code = 8;
	const char* al_color_fail_str = "Failed while attempting clear_to_color.";

	const unsigned int clear_EOL_fail_code = 9;
	const char* clear_EOL_fail_str = "Failed while attempting to clear to EOL.";

	const unsigned int write_fail_code = 10;
	const char* write_fail_str = "Failed while attempting to write character.";

	const unsigned int paint_cursor_fail_code = 11;
	const char* paint_cursor_fail_str = "Failed while attempting to erase or show cursor.";

	const unsigned int erase_fail_code = 12;
	const char* erase_fail_str = "Failed while attempting to erase letter.";

	const unsigned int unknown_fail_code = 13;
	const char* unknown_fail_str = "Unknown error.";
}

concreteLCD::concreteLCD() : cadd(1), lastCadd(1), initOk(false) {
	display = nullptr;
	try {
		setAllegro();
		initOk = true;
	}
	catch (AllegroError& e) {
		errorCode = e.code();
	}
	catch (std::exception& e) {
		errorCode = errors::unknown_fail_code;
	}
};

bool concreteLCD::lcdInitOk() { return initOk; }

int concreteLCD::lcdGetError() { return errorCode; };
bool concreteLCD::lcdClear() {
	bool result = false;
	try {
		al_clear_to_color(background);
		lcdUpdateCursor();
		result = true;
		cadd = 1;
		lastCadd = 1;
	}
	catch (AllegroError& e) {
		errorCode = e.code();
	}
	catch (std::exception& e) {
		errorCode = errors::al_color_fail_code;
	}

	return result;
};
bool concreteLCD::lcdClearToEOL() {
	int aux = cadd;
	bool result = false;
	try {
		while ((cadd - 1) % lcdWidth) {
			eraseLetter();
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
	long int posX = letterWidth * ((cadd - 1) % lcdWidth);
	long int posY = letterHeight * ((cadd - 1) / lcdWidth);

	try {
		if (!posX && c == spaceASCII)
			return *this;

		eraseLetter();
		char aux = tolower((char)c);

		al_draw_text(font, al_map_rgb(0, 0, 0), posX, posY, 0, (char*)&aux);

		lastCadd = cadd;
		cadd++;
		lcdUpdateCursor();
	}
	catch (AllegroError& e) {
		errorCode = e.code();
	}
	catch (std::exception& e) {
		errorCode = errors::write_fail_code;
	}
	return *this;
}
basicLCD& concreteLCD::operator << (const unsigned char* c) {
	int pos = 0;
	if (strlen((char*)c) > lcdWidth * lcdHeight) {
		pos = strlen((char*)c) - lcdWidth * lcdHeight;
	}

	while (pos < strlen((char*)c) && cadd <= lcdWidth * lcdHeight) {
		*this << c[pos];
		pos++;
	}
	return *this;
};

bool concreteLCD::lcdMoveCursorUp() {
	if (cadd > lcdWidth) {
		lastCadd = cadd;
		cadd -= lcdWidth;
	}
	bool result = false;
	try {
		lcdUpdateCursor();
		result = true;
	}
	catch (AllegroError& e) {
		errorCode = e.code();
	}
	catch (std::exception& e) {
		errorCode = errors::unknown_fail_code;
	}
	return result;
};
bool concreteLCD::lcdMoveCursorDown() {
	if (cadd <= lcdWidth) {
		lastCadd = cadd;
		cadd += lcdWidth;
	}

	bool result = false;
	try {
		lcdUpdateCursor();
		result = true;
	}
	catch (AllegroError& e) {
		errorCode = e.code();
	}
	catch (std::exception& e) {
		errorCode = errors::unknown_fail_code;
	}
	return result;
};
bool concreteLCD::lcdMoveCursorRight() {
	if (cadd < lcdWidth * lcdHeight) {
		lastCadd = cadd;
		cadd++;
	}

	bool result = false;
	try {
		lcdUpdateCursor();
		result = true;
	}
	catch (AllegroError& e) {
		errorCode = e.code();
	}
	catch (std::exception& e) {
		errorCode = errors::unknown_fail_code;
	}
	return result;
};
bool concreteLCD::lcdMoveCursorLeft() {
	if (cadd > 1) {
		lastCadd = cadd;
		cadd--;
	}
	bool result = false;
	try {
		lcdUpdateCursor();
		result = true;
	}
	catch (AllegroError& e) {
		errorCode = e.code();
	}
	catch (std::exception& e) {
		errorCode = errors::unknown_fail_code;
	}
	return result;
};

bool concreteLCD::lcdSetCursorPosition(const cursorPosition pos) {
	if (pos.column < lcdWidth && pos.row < height) {
		lastCadd = cadd;
		cadd = pos.row * lcdWidth + pos.column + 1;
	}
	bool result = false;
	try {
		lcdUpdateCursor();
		result = true;
	}
	catch (AllegroError& e) {
		errorCode = e.code();
	}
	catch (std::exception& e) {
		errorCode = errors::unknown_fail_code;
	}
	return result;
};
cursorPosition concreteLCD::lcdGetCursorPosition() {
	cursorPosition temp;

	temp.row = ((cadd - 1) / lcdWidth);
	temp.column = (cadd - 1) % lcdWidth;

	return temp;
};

void concreteLCD::lcdUpdateCursor() {
	paintCursor(false);

	paintCursor(true);

	al_flip_display();
};

void concreteLCD::paintCursor(bool show) {
	try {
		ALLEGRO_COLOR tempColor;
		int tempPos;
		if (show) {
			tempPos = cadd;
			tempColor = fontColor;
		}
		else {
			tempPos = lastCadd;
			tempColor = background;
		}

		long int posX_init = letterWidth * ((tempPos - 1) % lcdWidth);
		long int posY_init = letterHeight * ((tempPos - 1) / lcdWidth + 1) - lineWidth;
		long int posX_fin = posX_init + letterWidth - lineWidth;

		al_draw_line(posX_init, posY_init, posX_fin, posY_init, tempColor, lineWidth);
	}
	catch (std::exception& e) {
		throw AllegroError(errors::paint_cursor_fail_str, errors::paint_cursor_fail_code);
	}
}

/*Attempts to initialize Allegro and its addons.*/
void concreteLCD::setAllegro(void) {
	if (!al_init()) {
		throw AllegroError(errors::al_fail_str, errors::al_fail_code);
	}

	/*else if (!al_install_mouse()) {
		throw AllegroError(errors::al_mouse_fail_str, errors::al_mouse_fail_code);
	}

	else if (!al_install_keyboard()) {
		throw AllegroError(errors::al_keyboard_fail_str, errors::al_keyboard_fail_code);
	}*/
	else if (!al_init_primitives_addon()) {
		throw AllegroError(errors::al_primitives_fail_str, errors::al_primitives_fail_code);
	}
	else if (!al_init_ttf_addon()) {
		throw AllegroError(errors::al_ttf_fail_str, errors::al_ttf_fail_code);
	}
	else if (!(display = al_create_display(width, height))) {
		throw AllegroError(errors::al_display_fail_str, errors::al_display_fail_code);
	}
	else if (!(font = al_load_ttf_font(fontName, letterHeight, 0))) {
		throw AllegroError(errors::al_font_load_fail_str, errors::al_font_load_fail_code);
	}

	white = al_map_rgb(255, 255, 255);
	black = al_map_rgb(0, 0, 0);

	background = white;
	fontColor = black;

	al_clear_to_color(background);

	lcdUpdateCursor();
}

concreteLCD::~concreteLCD() {
	if (display)
		al_destroy_display(display);
	if (font)
		al_destroy_font(font);
}

void concreteLCD::eraseLetter() {
	try {
		long int posX_init = letterWidth * ((cadd - 1) % lcdWidth);
		long int posY_init = letterHeight * ((cadd - 1) / lcdWidth);
		long int posX_fin = posX_init + letterWidth;
		long int posY_fin = posY_init + letterHeight - 2 * lineWidth;

		al_draw_filled_rectangle(posX_init, posY_init, posX_fin, posY_fin, background);
	}
	catch (std::exception& e) {
		throw AllegroError(errors::erase_fail_str, errors::erase_fail_code);
	}
}