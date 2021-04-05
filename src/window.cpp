#include "window.h"
#include "simulation.h"

Window::Window()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
    }

    const char* glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    window = SDL_CreateWindow("nandy", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
    gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    bool err = gladLoadGL() == 0;
    if (err)
    {
        printf("Failed to initialize OpenGL loader!\n");
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    CherryTheme();

    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);
}

Window::~Window()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Window::HandleEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL2_ProcessEvent(&event);
        if (event.type == SDL_QUIT)
        {
            done = true;
        }
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
        {
            done = true;
        }
        if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(GetSDLWindow()))
        {
            done = true;
        }
    }
}

void Window::Draw(Simulation& sim)
{
    Frame_Prepare();

    // TODO: Don't hardcode sizes
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(1280, 720));
    ImGui::Begin("MAIN_WINDOW", 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_MenuBar);
    {
        Frame_Toolbar(sim);

        if (ImGui::Button("Step"))
        {
            sim.Step();
        }
        ImGui::SameLine();
        ImGui::Text("Step Count: %d", sim.step_count);

        // TODO: Extract this stuff somewhere else
        static float sz = 64.0f;
        static float thickness = 2.0f;
        const auto p = ImGui::GetCursorPos();
        float root_x = p.x + 10.0f;
        float root_y = p.y;
        float spacing = 8.0f;

        // TODO: Globalize this
        const ImU32 red = ImColor(ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
        const ImU32 yellow = ImColor(ImVec4(1.0f, 1.0f, 0.4f, 1.0f));
        const ImU32 grey = ImColor(ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
        const ImU32 green = ImColor(ImVec4(0.0f, 1.0f, 0.0f, 1.0f));

        // TODO: Define "draw primitives" somewhere else
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        auto draw_nand = [&](float x, float y) {
            // Legs
            draw_list->AddLine(ImVec2(x + sz * 0.0f, y + sz * 0.3f), ImVec2(x + sz * 0.2f, y + sz * 0.3f), grey, thickness);
            draw_list->AddLine(ImVec2(x + sz * 0.0f, y + sz * 0.7f), ImVec2(x + sz * 0.2f, y + sz * 0.7f), grey, thickness);
            draw_list->AddLine(ImVec2(x + sz * 0.8f, y + sz * 0.5f), ImVec2(x + sz * 1.0f, y + sz * 0.5f), grey, thickness);

            // Body
            draw_list->AddTriangle(ImVec2(x + sz * 0.2f, y + sz * 0.2f), ImVec2(x + sz * 0.2f, y + sz * 0.8f), ImVec2(x + sz * 0.8f, y + sz * 0.5f), yellow, thickness);
            draw_list->AddCircle(ImVec2(x + sz * 0.8f, y + sz * 0.5f), sz * 0.05f, yellow, 8, thickness);
        };

        // TODO: Split out "Drag 'n Drop Toolbar" into own method
        ImGui::Button("##", ImVec2(84, 64));
        if (ImGui::BeginDragDropSource())
        {
            auto pos = ImGui::GetMousePos();
            draw_nand(pos.x, pos.y);

            int gate_type = 0; // TODO: Enum
            ImGui::SetDragDropPayload("DND_PAYLOAD", &gate_type, sizeof(gate_type));

            ImGui::EndDragDropSource();
        }
        draw_nand(root_x, root_y);

        // Drawing/Canvas area
        // Data
        static std::vector<ImVec2> vec;
        
        // Receive DND
        ImVec2 canvas_pos = ImGui::GetCursorScreenPos();     // ImDrawList API uses screen coordinates!
        ImVec2 canvas_size = ImGui::GetContentRegionAvail(); // Resize canvas to what's available
        draw_list->AddRect(canvas_pos, ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y), IM_COL32(255, 255, 255, 255)); 
        ImGui::InvisibleButton("CANVAS", canvas_size);
        if (ImGui::BeginDragDropTarget())
        {
            ImGuiDragDropFlags target_flags = 0;
            target_flags |= ImGuiDragDropFlags_AcceptNoDrawDefaultRect; // Don't display the yellow rectangle
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_PAYLOAD", target_flags))
            {
                auto pos = ImGui::GetMousePos();
                int gate_type = *reinterpret_cast<int*>(payload->Data);
                vec.emplace_back(pos);
            }
            ImGui::EndDragDropTarget();
        }

        // Draw data
        for (auto& item : vec)
        {
            draw_nand(item.x, item.y);
        }
    }
    ImGui::End();

    Frame_Submit();
}

void Window::Frame_Prepare()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(window);
    ImGui::NewFrame();
}

void Window::Frame_Toolbar(Simulation& sim)
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            ImGui::Separator();
            if (ImGui::MenuItem("Exit"))
            {
                done = true;
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Simulation"))
        {
            ImGui::Separator();
            if (ImGui::MenuItem("Step"))
            {
                sim.Step();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
}

void Window::Frame_Submit()
{
    ImGui::Render();
    ImGuiIO& io = ImGui::GetIO();
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(window);
}

void Window::CherryTheme()
{
    // cherry colors, 3 intensities
#define HI(v) ImVec4(0.502f, 0.075f, 0.256f, v)
#define MED(v) ImVec4(0.455f, 0.198f, 0.301f, v)
#define LOW(v) ImVec4(0.232f, 0.201f, 0.271f, v)
// backgrounds (@todo: complete with BG_MED, BG_LOW)
#define BG(v) ImVec4(0.200f, 0.220f, 0.270f, v)
// text
#define TEXT(v) ImVec4(0.860f, 0.930f, 0.890f, v)

    auto& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_Text] = TEXT(0.78f);
    style.Colors[ImGuiCol_TextDisabled] = TEXT(0.28f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.17f, 1.00f);
    // style.Colors[ImGuiCol_ChildWindowBg] = BG(0.58f);
    style.Colors[ImGuiCol_PopupBg] = BG(0.9f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.31f, 0.31f, 1.00f, 0.00f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg] = BG(1.00f);
    style.Colors[ImGuiCol_FrameBgHovered] = MED(0.78f);
    style.Colors[ImGuiCol_FrameBgActive] = MED(1.00f);
    style.Colors[ImGuiCol_TitleBg] = LOW(1.00f);
    style.Colors[ImGuiCol_TitleBgActive] = HI(1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = BG(0.75f);
    style.Colors[ImGuiCol_MenuBarBg] = BG(0.47f);
    style.Colors[ImGuiCol_ScrollbarBg] = BG(1.00f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.09f, 0.15f, 0.16f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = MED(0.78f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = MED(1.00f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.71f, 0.22f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.71f, 0.22f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
    style.Colors[ImGuiCol_ButtonHovered] = MED(0.86f);
    style.Colors[ImGuiCol_ButtonActive] = MED(1.00f);
    style.Colors[ImGuiCol_Header] = MED(0.76f);
    style.Colors[ImGuiCol_HeaderHovered] = MED(0.86f);
    style.Colors[ImGuiCol_HeaderActive] = HI(1.00f);
    // style.Colors[ImGuiCol_Column] = ImVec4(0.14f, 0.16f, 0.19f, 1.00f);
    // style.Colors[ImGuiCol_ColumnHovered] = MED(0.78f);
    // style.Colors[ImGuiCol_ColumnActive] = MED(1.00f);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.47f, 0.77f, 0.83f, 0.04f);
    style.Colors[ImGuiCol_ResizeGripHovered] = MED(0.78f);
    style.Colors[ImGuiCol_ResizeGripActive] = MED(1.00f);
    style.Colors[ImGuiCol_PlotLines] = TEXT(0.63f);
    style.Colors[ImGuiCol_PlotLinesHovered] = MED(1.00f);
    style.Colors[ImGuiCol_PlotHistogram] = TEXT(0.63f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = MED(1.00f);
    style.Colors[ImGuiCol_TextSelectedBg] = MED(0.43f);
    // [...]
    style.Colors[ImGuiCol_ModalWindowDarkening] = BG(0.73f);

    style.WindowPadding = ImVec2(6, 4);
    style.WindowRounding = 0.0f;
    style.FramePadding = ImVec2(5, 2);
    style.FrameRounding = 3.0f;
    style.ItemSpacing = ImVec2(7, 1);
    style.ItemInnerSpacing = ImVec2(1, 1);
    style.TouchExtraPadding = ImVec2(0, 0);
    style.IndentSpacing = 6.0f;
    style.ScrollbarSize = 12.0f;
    style.ScrollbarRounding = 16.0f;
    style.GrabMinSize = 20.0f;
    style.GrabRounding = 2.0f;

    style.WindowTitleAlign.x = 0.50f;

    style.Colors[ImGuiCol_Border] = ImVec4(0.539f, 0.479f, 0.255f, 0.162f);
    style.FrameBorderSize = 0.0f;
    style.WindowBorderSize = 1.0f;
}
