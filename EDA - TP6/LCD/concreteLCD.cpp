#include "concreteLCD.h"
#include "AllegroError.h"
#include <allegro5/allegro_primitives.h>

// Namespace with constants to use during program.
namespace {
	const unsigned int width = 800;
	const unsigned int height = 192;
	const unsigned int lcdWidth = 16;
	const unsigned int lcdHeight = 2;
	const unsigned int letterWidth = width / lcdWidth;
	const unsigned int letterHeight = height / lcdHeight;

	const char* fontName = "Fonts/LCD-6.ttf";

	const int spaceASCII = 32;

	const unsigned int lineWidth = width / 160;

	const unsigned int maxRanges = 50;
}

// Namespace with error codes and meanings.
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

	const unsigned int ranges_fail_code = 14;
	const char* ranges_fail_str = "Failed while attempting to validate char in font.";
}

//concreteLCD constructor.
concreteLCD::concreteLCD() : cadd(1), lastCadd(1), initOk(false) {
	display = nullptr;
	try {
		//Attempts to initialize Allegro.
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

//Returns initOK.
bool concreteLCD::lcdInitOk() { return initOk; }

//Returns errorCode.
int concreteLCD::lcdGetError() { return errorCode; };

//Clears LCD screen.
bool concreteLCD::lcdClear() {
	bool result = false;
	try {
		//Clears to background color and updates cursor.
		al_clear_to_color(background);
		cadd = 1;
		lastCadd = 1;
		lcdUpdateCursor();
		result = true;
	}
	catch (AllegroError& e) {
		errorCode = e.code();
	}
	catch (std::exception& e) {
		errorCode = errors::al_color_fail_code;
	}

	return result;
};

//Clears from cursor to end of line.
bool concreteLCD::lcdClearToEOL() {
	bool result = false;
	int aux = cadd;

	try {
		//While still inside LCD range...
		int col = lcdGetCursorPosition().column;
		while (col < lcdWidth - 1) {
			//Erases letter and moves cursor.
			eraseLetter();
			col = lcdGetCursorPosition().column;
			cadd++;
		}
		cadd = aux;
		result = true;
		al_flip_display();
	}
	catch (std::exception& e) {
		errorCode = errors::clear_EOL_fail_code;
		cadd = aux;
	}

	return result;
};

//Prints a letter in LCD.
basicLCD& concreteLCD::operator << (const unsigned char c) {
	//Sets letter's position in X and Y.
	long int posX = letterWidth * ((cadd - 1) % lcdWidth);
	long int posY = letterHeight * ((cadd - 1) / lcdWidth);

	try {
		//If it's not a space...
		if (c != spaceASCII) {
			//If character is not supported, it skips it.
			if (!isCharSupported(c))
				return *this;
		}
		//If it's a space in first position, it skips it.
		else if (!posX)
			return *this;

		//Before writing, it erases the spot.
		eraseLetter();

		//Makes letter lower cap.
		char aux = tolower((char)c);

		//Draws letter.
		al_draw_text(font, fontColor, posX, posY, 0, (char*)&aux);

		//Updates cursor and cursor values.
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

//Prints array of chars in LCD.
basicLCD& concreteLCD::operator << (const unsigned char* c) {
	int pos = 0;

	//Sets starting position according to remaining space in LCD.
	if (strlen((char*)c) > (lcdWidth * lcdHeight - cadd + 1)) {
		pos = strlen((char*)c) - lcdWidth * lcdHeight + cadd - 1;
	}

	//Prints each character in the given array.
	while (pos < strlen((char*)c) && cadd <= lcdWidth * lcdHeight) {
		*this << c[pos];
		pos++;
	}
	return *this;
};

//Moves cursor up with error checking.
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

//Moves cursor down with error checking.
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

//Moves cursor right with error checking.
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

//Moves cursor left with error checking.
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

//Sets cursor position with error checking and draws it.
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

//Returns cursor position.
cursorPosition concreteLCD::lcdGetCursorPosition() {
	cursorPosition temp;

	temp.row = ((cadd - 1) / lcdWidth);
	temp.column = (cadd - 1) % lcdWidth;

	return temp;
};

//Erases previous cursor and draws new one.
void concreteLCD::lcdUpdateCursor() {
	paintCursor(false);

	paintCursor(true);

	al_flip_display();
};

/*According to the 'show' bool, it either draws the
new cursor or erases the old one. */
void concreteLCD::paintCursor(bool show) {
	try {
		ALLEGRO_COLOR tempColor;
		int tempPos;

		//If 'show', it sets parameter for new cursor.
		if (show) {
			tempPos = cadd;
			tempColor = fontColor;
		}

		//Otherwise, it sets parameters for old cursor.
		else {
			tempPos = lastCadd;
			tempColor = background;
		}

		//Sets X and Y positions for line drawing.
		long int posX_init = letterWidth * ((tempPos - 1) % lcdWidth);
		long int posY_init = letterHeight * ((tempPos - 1) / lcdWidth + 1) - lineWidth;
		long int posX_fin = posX_init + letterWidth - lineWidth;

		//Draws cursor.
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

	background = al_map_rgb(169, 169, 169);
	fontColor = al_map_rgb(0, 0, 0);

	al_clear_to_color(background);

	lcdUpdateCursor();
}

//Releases used resources.
concreteLCD::~concreteLCD() {
	if (display)
		al_destroy_display(display);
	if (font)
		al_destroy_font(font);
}

//Erases a letter from LCD.
void concreteLCD::eraseLetter() {
	try {
		//Sets position in X and Y.
		long int posX_init = letterWidth * ((cadd - 1) % lcdWidth);
		long int posY_init = letterHeight * ((cadd - 1) / lcdWidth);
		long int posX_fin = posX_init + letterWidth;
		long int posY_fin = posY_init + letterHeight - 2 * lineWidth;

		//Draws rectangle over letter.
		al_draw_filled_rectangle(posX_init, posY_init, posX_fin, posY_fin, background);
	}
	catch (std::exception& e) {
		throw AllegroError(errors::erase_fail_str, errors::erase_fail_code);
	}
}
//Checks if character is supported by font.
bool concreteLCD::isCharSupported(const char c) {
	try {
		int ranges[maxRanges];

		//Gets font's ranges.
		int allRanges = al_get_font_ranges(font, maxRanges, ranges);
		bool supported = false;

		//Checks for character in each range.
		for (int i = 0; i < (allRanges / 2) && (supported == false); i++) {
			if (c > ranges[2 * i] && c < ranges[2 * i + 1])
				supported = true;
		}

		return supported;
	}
	catch (std::exception& e) {
		throw AllegroError(errors::ranges_fail_str, errors::ranges_fail_code);
	}
}