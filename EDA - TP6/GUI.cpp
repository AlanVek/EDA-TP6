#include "GUI.h"


//GUI constructor.
GUI::GUI(void) {

    //Allegro init
    al_init();
    al_install_keyboard();
    al_install_mouse();
    al_init_primitives_addon();
    al_set_new_display_flags(ALLEGRO_RESIZABLE);

    //Display init
    display = al_create_display(1280, 720);
    al_set_window_title(display, "Dear ImGui Allegro 5 example");

    //Queue init
    queue = al_create_event_queue();
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_mouse_event_source());

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplAllegro5_Init(display);

}

//Main loop.
bool GUI::GUI_Loop(void) {

    static float f = 0.0f;
    static int counter = 0;
    static char buf[50];
    for (int i = 0; i < 50; i++)
        buf[i] = 0;

    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    backgroundColor = al_map_rgb(20, 20, 20);
    // Main loop
    bool running = true;
    while (running)
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        while (al_get_next_event(queue, &ev))
        {
            ImGui_ImplAllegro5_ProcessEvent(&ev);
            if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
                running = false;
            if (ev.type == ALLEGRO_EVENT_DISPLAY_RESIZE)
            {
                ImGui_ImplAllegro5_InvalidateDeviceObjects();
                al_acknowledge_resize(display);
                ImGui_ImplAllegro5_CreateDeviceObjects();
            }
        }

        // Start the Dear ImGui frame
        ImGui_ImplAllegro5_NewFrame();
        ImGui::NewFrame();

        {
            ImGui::Begin("EDA - TP6");                          

            
            ImGui::TextWrapped("Twitter account: @");
            float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
            ImGui::SameLine(0.0f, spacing);
            ImGui::InputText(" ",buf, sizeof(buf));
            //ImGui::PushAllowKeyboardFocus(false);
            //printf("%s", buf);

            //ImGui::Dummy(ImVec2(0.0f, 10.0f));
            ImGui::TextWrapped("Amount of tweets:");
            
            ImGui::InputInt(" ",&counter,1,100,0);
            //printf("%f", f);

            //ImGui::Dummy(ImVec2(0.0f, 10.0f));
            ImGui::AlignTextToFramePadding();
            ImGui::Text("Tweets pointer");
            ImGui::SameLine();
            ImGui::PushButtonRepeat(true);
            if (ImGui::ArrowButton("##left", ImGuiDir_Left)){
                //ACA VA GET PREVIOUS TWEET
            }
            ImGui::SameLine(0.0f, spacing);
            if (ImGui::ArrowButton("##right", ImGuiDir_Right)) {
                //ACA VA GET NEXT TWEET
            }
            ImGui::PopButtonRepeat();

            ImGui::End();
        }


        // Rendering
        ImGui::Render();
        ImGui_ImplAllegro5_RenderDrawData(ImGui::GetDrawData());
        al_flip_display();
        al_clear_to_color(backgroundColor);
    }

    // Cleanup
    ImGui_ImplAllegro5_Shutdown();
    ImGui::DestroyContext();
    al_destroy_event_queue(queue);
    al_destroy_display(display);

    return 0;
}

GUI::~GUI() {
    // Cleanup
    ImGui_ImplAllegro5_Shutdown();
    ImGui::DestroyContext();
    al_destroy_event_queue(queue);
    al_destroy_display(display);
}