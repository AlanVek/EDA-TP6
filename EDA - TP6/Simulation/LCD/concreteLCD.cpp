#include "concreteLCD.h"
#include "AllegroError.h"
#include <allegro5/allegro_primitives.h>

// Namespace with constants to use during program.
namespace LCD_data {
	const unsigned int	width = 800;
	const unsigned int	height = 192;
	const unsigned int	lcdWidth = 16;
	const unsigned int	lcdHeight = 2;
	const unsigned int	letterWidth = width / lcdWidth;
	const unsigned int	letterHeight = height / lcdHeight;
	const unsigned int	spaceASCII = 32;
	const unsigned int	lineWidth = width / 160;
	const unsigned int	maxRanges = 50;

	const char* fontName = "Fonts/LCD-6.ttf";
}

// Namespace with error codes and meanings.
namespace errors {
	const char* al_fail_str = "Failed while attempting to initialize Allegro.";
	const char* al_mouse_fail_str = "Failed while attempting to initialize mouse addon.";
	const char* al_keyboard_fail_str = "Failed while attempting to initialize keyboard addon.";
	const char* al_primitives_fail_str = "Failed while attempting to initialize primitives addon.";
	const char* al_ttf_fail_str = "Failed while attempting to initialize ttf addon.";
	const char* al_font_load_fail_str = "Failed while attempting to load font.";
	const char* al_display_fail_str = "Failed while attempting to create display";
	const char* al_color_fail_str = "Failed while attempting clear_to_color.";
	const char* clear_EOL_fail_str = "Failed while attempting to clear to EOL.";
	const char* write_fail_str = "Failed while attempting to write character.";
	const char* paint_cursor_fail_str = "Failed while attempting to erase or show cursor.";
	const char* erase_fail_str = "Failed while attempting to erase letter.";
	const char* ranges_fail_str = "Failed while attempting to validate char in font.";
	const char* unknown_fail_str = "Unknown error.";

	enum {
		al_fail_code,
		al_mouse_fail_code,
		al_keyboard_fail_code,
		al_primitives_fail_code,
		al_ttf_fail_code,
		al_font_load_fail_code,
		al_display_fail_code,
		al_color_fail_code,
		clear_EOL_fail_code,
		write_fail_code,
		paint_cursor_fail_code,
		erase_fail_code,
		unknown_fail_code,
		ranges_fail_code
	};
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
	catch (std::exception&) {
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
	al_set_target_backbuffer(display);

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
	catch (std::exception&) {
		errorCode = errors::al_color_fail_code;
	}

	return result;
};

//Clears from cursor to end of line.
bool concreteLCD::lcdClearToEOL() {
	al_set_target_backbuffer(display);

	bool result = false;
	int aux = cadd;

	try {
		//While still inside LCD range...
		int col = lcdGetCursorPosition().column;
		while (col < LCD_data::lcdWidth - 1) {
			//Erases letter and moves cursor.
			eraseLetter();
			col = lcdGetCursorPosition().column;
			cadd++;
		}
		cadd = aux;
		result = true;
		al_flip_display();
	}
	catch (std::exception&) {
		errorCode = errors::clear_EOL_fail_code;
		cadd = aux;
	}

	return result;
};

//Prints a letter in LCD.
basicLCD& concreteLCD::operator << (const unsigned char c) {
	al_set_target_backbuffer(display);

	//Sets letter's position in X and Y.
	long int posX = LCD_data::letterWidth * ((cadd - 1) % LCD_data::lcdWidth);
	long int posY = LCD_data::letterHeight * ((cadd - 1) / LCD_data::lcdWidth);

	try {
		//If it's not a space...
		if (c != LCD_data::spaceASCII) {
			//If character is not supported, it skips it.
			if (supportedChars.find(c) == std::string::npos && !isCharSupported(c))
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
		if (isChar)
			lcdUpdateCursor();
	}
	catch (AllegroError& e) {
		errorCode = e.code();
	}
	catch (std::exception&) {
		errorCode = errors::write_fail_code;
	}
	return *this;
}
//Prints array of chars in LCD.
basicLCD& concreteLCD::operator << (const unsigned char* c) {
	unsigned int pos = 0;

	isChar = false;

	//Prints each character in the given array.
	while (pos < strlen((char*)c) && cadd <= LCD_data::lcdWidth * LCD_data::lcdHeight) {
		/*if (cadd == LCD_data::lcdWidth)
			*this << (unsigned char)'-';
		else {*/
		*this << c[pos];
		pos++;
		//}
	}
	try {
		lcdUpdateCursor();
	}
	catch (AllegroError& e) {
		errorCode = e.code();
	}
	catch (std::exception&) {
		errorCode = errors::write_fail_code;
	}
	isChar = true;
	return *this;
};

//Moves cursor up with error checking.
bool concreteLCD::lcdMoveCursorUp() {
	al_set_target_backbuffer(display);

	if (cadd > LCD_data::lcdWidth) {
		lastCadd = cadd;
		cadd -= LCD_data::lcdWidth;
	}
	bool result = false;
	try {
		lcdUpdateCursor();
		result = true;
	}
	catch (AllegroError& e) {
		errorCode = e.code();
	}
	catch (std::exception&) {
		errorCode = errors::unknown_fail_code;
	}
	return result;
};

//Moves cursor down with error checking.
bool concreteLCD::lcdMoveCursorDown() {
	al_set_target_backbuffer(display);

	if (cadd <= LCD_data::lcdWidth) {
		lastCadd = cadd;
		cadd += LCD_data::lcdWidth;
	}

	bool result = false;
	try {
		lcdUpdateCursor();
		result = true;
	}
	catch (AllegroError& e) {
		errorCode = e.code();
	}
	catch (std::exception&) {
		errorCode = errors::unknown_fail_code;
	}
	return result;
};

//Moves cursor right with error checking.
bool concreteLCD::lcdMoveCursorRight() {
	al_set_target_backbuffer(display);

	if (cadd < LCD_data::lcdWidth * LCD_data::lcdHeight) {
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
	catch (std::exception&) {
		errorCode = errors::unknown_fail_code;
	}
	return result;
};

//Moves cursor left with error checking.
bool concreteLCD::lcdMoveCursorLeft() {
	al_set_target_backbuffer(display);

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
	catch (std::exception&) {
		errorCode = errors::unknown_fail_code;
	}
	return result;
};

//Sets cursor position with error checking and draws it.
bool concreteLCD::lcdSetCursorPosition(const cursorPosition pos) {
	al_set_target_backbuffer(display);

	if (pos.column < LCD_data::lcdWidth && pos.row < LCD_data::height) {
		lastCadd = cadd;
		cadd = pos.row * LCD_data::lcdWidth + pos.column + 1;
	}
	bool result = false;
	try {
		lcdUpdateCursor();
		result = true;
	}
	catch (AllegroError& e) {
		errorCode = e.code();
	}
	catch (std::exception&) {
		errorCode = errors::unknown_fail_code;
	}
	return result;
};

//Returns cursor position.
cursorPosition concreteLCD::lcdGetCursorPosition() {
	al_set_target_backbuffer(display);

	cursorPosition temp;

	temp.row = ((cadd - 1) / LCD_data::lcdWidth);
	temp.column = (cadd - 1) % LCD_data::lcdWidth;

	return temp;
};

//Erases previous cursor and draws new one.
void concreteLCD::lcdUpdateCursor() {
	al_set_target_backbuffer(display);

	paintCursor(false);

	paintCursor(true);

	al_flip_display();
};

/*According to the 'show' bool, it either draws the
new cursor or erases the old one. */
void concreteLCD::paintCursor(bool show) const {
	al_set_target_backbuffer(display);

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
		long int posX_init = LCD_data::letterWidth * ((tempPos - 1) % LCD_data::lcdWidth);
		long int posY_init = LCD_data::letterHeight * ((tempPos - 1) / LCD_data::lcdWidth + 1) - LCD_data::lineWidth;
		long int posX_fin = posX_init + LCD_data::letterWidth - LCD_data::lineWidth;

		//Draws cursor.
		al_draw_line(posX_init, posY_init, posX_fin, posY_init, tempColor, LCD_data::lineWidth);
	}
	catch (std::exception&) {
		throw AllegroError(errors::paint_cursor_fail_str, errors::paint_cursor_fail_code);
	}
}

/*Attempts to initialize Allegro and its addons.*/
void concreteLCD::setAllegro(void) {
	if (!al_init()) {
		throw AllegroError(errors::al_fail_str, errors::al_fail_code);
	}
	else if (!al_init_primitives_addon()) {
		throw AllegroError(errors::al_primitives_fail_str, errors::al_primitives_fail_code);
	}
	else if (!al_init_ttf_addon()) {
		throw AllegroError(errors::al_ttf_fail_str, errors::al_ttf_fail_code);
	}
	else if (!(display = al_create_display(LCD_data::width, LCD_data::height))) {
		throw AllegroError(errors::al_display_fail_str, errors::al_display_fail_code);
	}
	else if (!(font = al_load_ttf_font(LCD_data::fontName, LCD_data::letterHeight, 0))) {
		throw AllegroError(errors::al_font_load_fail_str, errors::al_font_load_fail_code);
	}
	background = al_map_rgb(0, 0, 0);
	fontColor = al_map_rgb(255, 255, 255);

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
void concreteLCD::eraseLetter() const {
	al_set_target_backbuffer(display);

	try {
		//Sets position in X and Y.
		long int posX_init = LCD_data::letterWidth * ((cadd - 1) % LCD_data::lcdWidth);
		long int posY_init = LCD_data::letterHeight * ((cadd - 1) / LCD_data::lcdWidth);
		long int posX_fin = posX_init + LCD_data::letterWidth;
		long int posY_fin = posY_init + LCD_data::letterHeight;

		//Draws rectangle over letter.
		al_draw_filled_rectangle(posX_init, posY_init, posX_fin, posY_fin, background);
	}
	catch (std::exception&) {
		throw AllegroError(errors::erase_fail_str, errors::erase_fail_code);
	}
}
//Checks if character is supported by font.
bool concreteLCD::isCharSupported(const char c) {
	try {
		int ranges[LCD_data::maxRanges];

		//Gets font's ranges.
		int allRanges = al_get_font_ranges(font, LCD_data::maxRanges, ranges);
		bool supported = false;

		if (allRanges != -1) {
			//Checks for character in each range.
			for (int i = 0; i < (allRanges / 2) && (supported == false); i++) {
				if (c > ranges[2 * i] && c < ranges[2 * i + 1]) {
					supported = true;
					supportedChars.append(1, c);
				}
			}
		}
		return supported;
	}
	catch (std::exception&) {
		throw AllegroError(errors::ranges_fail_str, errors::ranges_fail_code);
	}
}