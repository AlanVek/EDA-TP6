#include "imgui/imgui.h"
#include "imgui/imgui_impl_allegro5.h"
#include "imgui/imgui_stdlib.h"
#include "GUI.h"
#include <exception>

namespace GUI_data {
	const unsigned int width = 750;
	const unsigned int height = 400;
	const int MAXTWEETS = 50;

	const double maxSpeed = 2.0;
	const double minSpeed = 0.5;
	const double speedStep = 0.05;
	const double speedStepFast = 0.1;
}

//GUI constructor.
GUI::GUI(void) :usernameOk(false), tweetCount(0), speed(1.0) {
	username.clear();

	//Initializes Allegro resources.
	if (!(al_install_keyboard()))
		throw std::exception("Failed to initialize keyboard addon.");
	if (!(al_install_mouse()))
		throw std::exception("Failed to initialize mouse addon.");

	if (!(guiQueue = al_create_event_queue()))
		throw std::exception("Failed to create event queue.");

	if (!(guiDisp = al_create_display(GUI_data::width, GUI_data::height)))
		throw std::exception("Failed to create display.");

	al_register_event_source(guiQueue, al_get_keyboard_event_source());
	al_register_event_source(guiQueue, al_get_mouse_event_source());
	al_register_event_source(guiQueue, al_get_display_event_source(guiDisp));

	initialSetup();
}
//Set up for GUI with ImGUI.
void GUI::initialSetup(void) const {
	al_set_target_backbuffer(guiDisp);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	al_set_new_display_flags(ALLEGRO_RESIZABLE);

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer bindings
	ImGui_ImplAllegro5_Init(guiDisp);

	//Sets screen to black.
	al_clear_to_color(al_map_rgb(0, 0, 0));
}

#include <iostream>
//First GUI run. Loops until a username has been given.
bool GUI::firstRun(void) {
	bool endOfSetUp = false;
	bool result = true;

	al_set_target_backbuffer(guiDisp);

	//Drawing loop.
	while (!endOfSetUp) {
		//Clears event queue and checks if user pressed ESC or closed display.
		if (checkGUIEvents()) {
			endOfSetUp = true;
			result = false;
		}
		else {
			//Starts the Dear ImGui frame.
			ImGui_ImplAllegro5_NewFrame();
			ImGui::NewFrame();

			//Sets window.
			ImGui::SetNextWindowSize(ImVec2(GUI_data::width, GUI_data::height / 2));
			ImGui::SetNextWindowPos(ImVec2(0, 0));
			ImGui::Begin("Initial Setup");

			ImGui::NewLine();

			//Sets username text input.
			ImGui::Text("Username: ");
			ImGui::SameLine();
			if (ImGui::InputText(" ~ ", &username)) {
				usernameOk = username.length();
			}

			//Sets tweet number int input.
			ImGui::Text("Tweets:   ");
			ImGui::SameLine();
			if (ImGui::InputInt(" - ", &tweetCount)) {
				if (tweetCount < 0)
					tweetCount = 0;
				if (tweetCount > GUI_data::MAXTWEETS)
					tweetCount = GUI_data::MAXTWEETS;
			}
			ImGui::NewLine();

			//Sets "request" button.
			if (ImGui::Button("Request tweets")) {
				endOfSetUp = usernameOk;
			}
			ImGui::NewLine();
			ImGui::NewLine();
			ImGui::NewLine();

			//Sets "exit" button.
			if (ImGui::Button("Exit")) {
				endOfSetUp = true;
				result = false;
			}

			ImGui::End();

			//Rendering.
			ImGui::Render();
			al_clear_to_color(al_map_rgb(0, 0, 0));
			ImGui_ImplAllegro5_RenderDrawData(ImGui::GetDrawData());

			al_flip_display();
		}
	}
	return result;
}
/*Checks if user pressed ESC or closed display.
It also deals with display resizing.*/
bool GUI::checkGUIEvents(void) {
	bool result = false;

	//Gets events.
	while ((al_get_next_event(guiQueue, &guiEvent)))
	{
		ImGui_ImplAllegro5_ProcessEvent(&guiEvent);

		/*If the display has been closed or if the user has pressed ESC, return true. */
		if (guiEvent.type == ALLEGRO_EVENT_DISPLAY_CLOSE || (guiEvent.type == ALLEGRO_EVENT_KEY_DOWN &&
			guiEvent.keyboard.keycode == ALLEGRO_KEY_ESCAPE))
			result = true;

		//If the display has been resized, it tells ImGUI to take care of it.
		else if (guiEvent.type == ALLEGRO_EVENT_DISPLAY_RESIZE)
		{
			ImGui_ImplAllegro5_InvalidateDeviceObjects();
			al_acknowledge_resize(guiDisp);
			ImGui_ImplAllegro5_CreateDeviceObjects();
		}
	}
	return result;
}

//Cycle that shows menu (called with every iteration).
const codes GUI::checkStatus(void) {
	bool keepGoing = true;

	//By default, it returns codes::NOTHING.
	codes result = codes::NOTHING;

	al_set_target_backbuffer(guiDisp);

	//If user pressed ESC or closed display, returns codes::END.
	if (checkGUIEvents()) {
		result = codes::END;
	}
	else {
		//Sets new ImGUI frame.
		ImGui_ImplAllegro5_NewFrame();
		ImGui::NewFrame();

		//Sets new window.
		ImGui::SetNextWindowSize(ImVec2(GUI_data::width, GUI_data::height));
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::Begin("EDA - TP6");

		//Sets cursor buttons.
		ImGui::Text("Cursor buttons:");
		if (ImGui::Button("Move cursor left"))
			result = codes::LEFT;
		ImGui::SameLine();
		if (ImGui::Button("Move cursor right"))
			result = codes::RIGHT;
		ImGui::SameLine();
		if (ImGui::Button("Move cursor down"))
			result = codes::DOWN;
		ImGui::SameLine();
		if (ImGui::Button("Move cursor up"))
			result = codes::UP;

		//Sets clear buttons.
		ImGui::NewLine();
		ImGui::Text("Clear buttons:");
		if (ImGui::Button("Clear LCD"))
			result = codes::CLEARALL;
		ImGui::SameLine();
		if (ImGui::Button("Clear to EOL"))
			result = codes::CLEAREOL;

		//Sets tweet handling buttons.
		ImGui::NewLine();
		ImGui::Text("Tweet buttons:");
		if (ImGui::Button("Previous tweet"))
			result = codes::PREVIOUS;
		ImGui::SameLine();
		if (ImGui::Button("Next tweet"))
			result = codes::NEXT;
		ImGui::SameLine();
		if (ImGui::Button("Reload tweet"))
			result = codes::RELOAD;
		ImGui::NewLine();
		ImGui::Text("Roll speed:  ");
		ImGui::SameLine();
		if (ImGui::InputFloat("", &speed, GUI_data::speedStep, GUI_data::speedStepFast)) {
			if (speed > GUI_data::maxSpeed)
				speed = GUI_data::maxSpeed;
			else if (speed < GUI_data::minSpeed)
				speed = GUI_data::minSpeed;

			result = codes::SPEED;
		}

		//Sets username text input and tweet number int input.
		ImGui::NewLine();
		ImGui::Text("Twitter data input: ");
		ImGui::NewLine();

		ImGui::Text("Username: ");
		ImGui::SameLine();
		if (ImGui::InputText(" ~ ", &username)) {
			usernameOk = username.length();
		}
		ImGui::Text("Tweets:   ");
		ImGui::SameLine();
		if (ImGui::InputInt(" - ", &tweetCount)) {
			if (tweetCount < 0)
				tweetCount = 0;
			if (tweetCount > GUI_data::MAXTWEETS)
				tweetCount = GUI_data::MAXTWEETS;
		}

		//Sets "request" and "cancel request" buttons.
		ImGui::NewLine();
		if (ImGui::Button("Request tweets")) {
			if (usernameOk)
				result = codes::REQUEST;
		}

		ImGui::SameLine();
		if (ImGui::Button("Cancel request")) {
			result = codes::CANCEL;
		}

		ImGui::NewLine();
		//Sets "exit" button.
		if (ImGui::Button("Exit"))
			result = codes::END;
		ImGui::End();

		//Rendering.
		ImGui::Render();
		al_clear_to_color(al_map_rgb(0, 0, 0));
		ImGui_ImplAllegro5_RenderDrawData(ImGui::GetDrawData());

		al_flip_display();
	}
	return result;
}

//Cleanup. Frees resources.
GUI::~GUI() {
	ImGui_ImplAllegro5_Shutdown();
	ImGui::DestroyContext();
	if (guiQueue)
		al_destroy_event_queue(guiQueue);
}

//Getters.
const std::string& GUI::getUsername() const { return username; }
const int GUI::getTweetCount(void) const { return tweetCount; }
const float GUI::getSpeed(void) const { return speed; }