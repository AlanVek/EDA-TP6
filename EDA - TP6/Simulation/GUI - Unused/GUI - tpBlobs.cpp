#include <allegro5/allegro_primitives.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_allegro5.h"
#include <stdint.h>
#include "GUI - tpBlobs.h"
#include <exception>

namespace {
	const unsigned int width = 750;
	const unsigned int height = 400;

	const int NOTHING = 0;
	const int USERNAMEOK = 1;
	const int MAXTWEETS = 50;
}

//#define minRelSpeedLimit 0.0f
//#define maxRelSpeedLimit 1.0f
//#define minMaxSpeedLimit 0.0f
//#define maxMaxSpeedLimit 30.0f
//#define minBlobCount 0
//#define maxBlobCount 50
//#define minRandomJiggleLimit 0.001
//#define maxRandomJiggleLimit 0.5
//#define minSmellRadiusLimit 0.0f
//#define maxSmellRadiusLimit 200.0f
//#define minFoodCountLimit 0
//#define maxFoodCountLimit 50
//#define GUIHeight 400
//#define GUIWidth 750
//#define MAXDEATHPROB 0.01f

//GUI constructor.
GUI::GUI(void) {
	/*relSpeed = 0.0f;
	maxSpeed = 0.0f;
	randomJL = 0.0f;
	babyDeathProb = 0.0f;
	grownDeathProb = 0.0f;
	goodOldDeathProb = 0.0f;
	mode = false;
	blobCount = 0;
	foodCount = 0;
	smellRadius = 0.0f;
	pause = false;*/

	dataEntered = NOTHING;
	request = false;
	tweetCount = 0;
	for (int i = 0; i < MAXUSERNAME; i++)
		username[i] = 0;

	if (!(al_install_keyboard()))
		throw std::exception("Failed to initialize keyboard addon.");
	if (!(al_install_mouse()))
		throw std::exception("Failed to initialize mouse addon.");

	if (!(guiQueue = al_create_event_queue()))
		throw std::exception("Failed to create event queue.");

	if (!(guiDisp = al_create_display(width, height)))
		throw std::exception("Failed to create display.");

	al_register_event_source(guiQueue, al_get_keyboard_event_source());
	al_register_event_source(guiQueue, al_get_mouse_event_source());
	al_register_event_source(guiQueue, al_get_display_event_source(guiDisp));

	GUI_setUp();
}
//Set up for GUI.
void GUI::GUI_setUp(void) {
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

//Starting GUI setup, asking for amount of blobs, mode and speed (depending on mode).
bool GUI::GUI_firstLoop(void) {
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

		//Starts the Dear ImGui frame.
		ImGui_ImplAllegro5_NewFrame();
		ImGui::NewFrame();

		//Sets window.
		ImGui::SetNextWindowSize(ImVec2(width, height));
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::Begin("EDA - TP6");
		ImGui::Text("Initial Set up: ");

		//Button to select mode.
		if (ImGui::InputText("Username ", username, MAXUSERNAME)) {
			dataEntered = USERNAMEOK;
		}
		if (ImGui::InputInt("Tweet amount ", &tweetCount)) {
			if (tweetCount < 0)
				tweetCount = 0;
			if (tweetCount > MAXTWEETS)
				tweetCount = MAXTWEETS;
		}

		if (ImGui::Button("Start request "))
			endOfSetUp = true;

		ImGui::SameLine();
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
	return result;
}

bool GUI::checkGUIEvents(void) {
	bool result = false;

	//Gets events.
	while ((al_get_next_event(guiQueue, &guiEvent)))
	{
		ImGui_ImplAllegro5_ProcessEvent(&guiEvent);

		/*If the display has been closed or if the user has pressed ESC,
		 the program ends. */
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
bool GUI::GUI_Game_Loop(void) {
	bool keepGoing = true;
	bool result = true;

	al_set_target_backbuffer(guiDisp);
	if (checkGUIEvents()) {
		result = false;
	}
	ImGui::SetNextWindowSize(ImVec2(width, height));
	ImGui::SetNextWindowPos(ImVec2(0, 0));

	ImGui_ImplAllegro5_NewFrame();
	ImGui::NewFrame();
	ImGui::SetNextWindowSize(ImVec2(width, height));
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::Begin("EDA - TP6");

	////Sets relative speed slider (both modes let the user define this parameter on the go).
	//ImGui::SliderFloat("General relative speed", &relSpeed, minRelSpeedLimit, maxRelSpeedLimit);

	////Sets sliders for death probabilities.
	//ImGui::SliderFloat("Death probability - Baby Blob", &babyDeathProb, 0.0f, MAXDEATHPROB);
	//ImGui::SliderFloat("Death probability - Grown Blob", &grownDeathProb, 0.0f, MAXDEATHPROB);
	//ImGui::SliderFloat("Death probability - Good Old Blob", &goodOldDeathProb, 0.0f, MAXDEATHPROB);
	//ImGui::SliderFloat("Random Jiggle Limit", &randomJL, minRandomJiggleLimit, maxRandomJiggleLimit);

	////Sets input bars for smell radius(float), random jiggle limit(float) and food count (int).
	//if (ImGui::InputInt("Smell radius", &smellRadius)) {
	//	if (smellRadius < minSmellRadiusLimit)
	//		smellRadius = minSmellRadiusLimit;
	//	else if (smellRadius > maxSmellRadiusLimit)
	//		smellRadius = maxSmellRadiusLimit;
	//}
	//if (ImGui::InputInt("Food count", &foodCount)) {
	//	if (foodCount < minFoodCountLimit)
	//		foodCount = minFoodCountLimit;
	//	else if (foodCount > maxFoodCountLimit)
	//		foodCount = maxFoodCountLimit;
	//}

	////Sets pause button.
	//if (ImGui::Button("Pause"))
	//	pause = !pause;

	////Shows current state next to button.
	//ImGui::SameLine();
	//ImGui::Text("%s", pause ? "Paused" : "Unpaused");

	//ImGui::Text("Game will end when either user presses ESC or all blobs die. :)");

	ImGui::End();

	ImGui::Render();
	al_clear_to_color(al_map_rgb(0, 0, 0));
	ImGui_ImplAllegro5_RenderDrawData(ImGui::GetDrawData());

	al_flip_display();
	return result;
}

GUI::~GUI() {
	//Cleanup.
	ImGui_ImplAllegro5_Shutdown();
	ImGui::DestroyContext();
	if (guiDisp)
		al_destroy_display(guiDisp);
	if (guiQueue)
		al_destroy_event_queue(guiQueue);
}