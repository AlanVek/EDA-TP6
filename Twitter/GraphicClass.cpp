#include "GraphicClass.h"
//#include <allegro5/allegro_primitives.h>
//#include <allegro5/allegro_ttf.h>


#define babyBitmap "babyblob.png"
#define grownBitmap "grownblob.png"
#define goodOldBitmap "goodoldblob.png"
#define foodBitmap "food.png"
#define backgrBitmap "background.jpg"



//GraphicClass constructor.
GraphicClass::GraphicClass(unsigned int width_, unsigned int height_) :
	
    height(height_), width(width_){

    display = nullptr;
    backgroundBit = nullptr;

    babyBit = nullptr;
    grownBit = nullptr;

    goodOldBit= nullptr;
    foodBit = nullptr;
    //font = nullptr;
}

//Attempts to create display. Returns true if successful. 
bool GraphicClass::createDisplay(void) {
    return (display = al_create_display(width, height));
}

//Sets font name.
//void GraphicClass::setFontName(const char* fontName_) { fontName = fontName_; }

//Gets display.
ALLEGRO_DISPLAY*& GraphicClass::getDisplay(void) { return display; }

//Frees memory.
GraphicClass::~GraphicClass(void) {
    if (display)
        al_destroy_display(display);
    if (backgroundBit)
        al_destroy_bitmap(backgroundBit);

    if (babyBit)
        al_destroy_bitmap(babyBit);
    if (grownBit)
        al_destroy_bitmap(grownBit);
    if (goodOldBit)
        al_destroy_bitmap(goodOldBit);
    if (foodBit)
        al_destroy_bitmap(foodBit);
}

/*//Attempts to load font. Returns false if unsuccessful.
bool GraphicClass::loadFont(void) {  
    return (font = al_load_ttf_font(fontName, FONTSIZE, 0));
}*/

//Creates a bitmap, stores it in thisBit and loads it with bitName.
bool GraphicClass::createBitmap(unsigned int W, unsigned int H, const char* bitName, ALLEGRO_BITMAP*& thisBit){
    bool result = true;

    if (!(thisBit = al_create_bitmap(W, H)))
        result = false;
    else if (!(thisBit = al_load_bitmap(bitName)))
        result = false;
    return result;
}

//Draw a bitmap at position (x,y).
void GraphicClass::drawBitmap(ALLEGRO_BITMAP*& thisBit, float X, float Y) { al_draw_bitmap(thisBit,X,Y, 0); }


//Bitmap getters.
ALLEGRO_BITMAP*& GraphicClass::getBabyBit(void) { return babyBit; }
ALLEGRO_BITMAP*& GraphicClass::getGrownBit(void) { return grownBit; }
ALLEGRO_BITMAP*& GraphicClass::getGoodBit(void) { return goodOldBit; }
ALLEGRO_BITMAP*& GraphicClass::getBackgrBit(void) { return backgroundBit; }
ALLEGRO_BITMAP*& GraphicClass::getFoodBit(void) { return foodBit; }


//Creates and loads all bitmaps.
bool GraphicClass::initializeBitmaps(unsigned int width_, unsigned int height_) {
    bool result = true;
    
    if (!createBitmap(width_, height_, babyBitmap, babyBit))
        result = false;
    else if (!createBitmap(width_, height_, grownBitmap, grownBit))
        result = false;
    else if (!(createBitmap(width_, height_, goodOldBitmap, goodOldBit)))
        result = false;
    else if (!(createBitmap(width_, height_, foodBitmap, foodBit)))
        result = false;
    else if (!(createBitmap(width_, height_, backgrBitmap,  backgroundBit)))
        result = false;

    return result;
}