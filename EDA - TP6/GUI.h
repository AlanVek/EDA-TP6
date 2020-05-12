#include <stdint.h>
#include <stdio.h>
#include "imgui/imgui.h"
#include "imgui/imstb_textedit.h"
#include "imgui/imgui_impl_allegro5.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

class GUI {

public:

	GUI();

	~GUI();

	bool GUI_Loop(void);


protected:
	ALLEGRO_DISPLAY* display;
	ALLEGRO_EVENT_QUEUE* queue;
	ALLEGRO_EVENT ev;
	ALLEGRO_COLOR backgroundColor;
};

