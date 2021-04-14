#include "window.h"
#include "simulation.h"

#include <chrono>
#include <cstdio>
#include <vector>

#include <glad/glad.h>

ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

Window::Window(int w, int h)
: m_width(w)
, m_height(h)
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
    auto window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    window = SDL_CreateWindow("nandy", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_width, m_height, window_flags);
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
    io.IniFilename = nullptr; 
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

void Window::Draw()
{
    ImGuiIO& io = ImGui::GetIO();
    Frame_Prepare();

    // TODO: Don't hardcode sizes
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(m_width, m_height));
    ImGui::Begin("MAIN_WINDOW", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_MenuBar);
    {
        Frame_Toolbar(sim);

        ImGui::Text("Step Count: %ld", sim.step_count);

        ImGui::SameLine();
        ImGui::Text("| Step Time: %lldus", sim.step_time);

        ImGui::SameLine();
        ImGui::Text("| Nodes: %zu", sim.nodes.size());

        ImGui::SameLine();
        ImGui::Text("| NANDs: %zu", sim.nands.size());

        if (ImGui::Button("Step"))
        {
            sim.Step();
        }

        auto define_c_button = [&](const char* name) {
            ImGui::SameLine();
            if (ImGui::Button(name))
            {
                auto c = sim.LookupNode(name);
                if (c)
                {
                    c->active = !c->active;
                }
                else
                {
                    sim.NewNode(100, 200, name);
                }
                sim.Step();
            }
        };
        define_c_button("c0");
        define_c_button("c1");
        define_c_button("c2");
        define_c_button("c3");
        define_c_button("c4");
        ImGui::SameLine();
        if (ImGui::Button("Add node"))
        {
            sim.NewNode(100, 200);
        }

        ImGui::SameLine();
        ImGui::Text("Add label (enter):");
        ImGui::SameLine();
        std::array<char, 20> arr{ { 0 } };
        ImGui::PushItemWidth(128); 
        if (ImGui::InputText("##", arr.data(), arr.size(), ImGuiInputTextFlags_EnterReturnsTrue))
        {
            std::string str(arr.data(), arr.size());
            sim.AddLabel(200, 200, str);
        }

        // TODO: Extract this stuff somewhere else
        static float sz = 64.0f;
        static float thickness = 2.0f;
        const auto p = ImGui::GetCursorPos();

        // TODO: Globalize this
        const ImU32 red = ImColor(ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
        const ImU32 yellow = ImColor(ImVec4(1.0f, 1.0f, 0.4f, 1.0f));
        const ImU32 grey = ImColor(ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
        const ImU32 green = ImColor(ImVec4(0.0f, 1.0f, 0.0f, 1.0f));

        // TODO: Define "draw primitives" somewhere else
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        auto draw_nand = [&](float x, float y, bool a = false, bool b = false, bool o = false) {
            // Legs
            draw_list->AddLine(ImVec2(x + sz * 0.0f, y + sz * 0.3f), ImVec2(x + sz * 0.2f, y + sz * 0.3f), a ? yellow : grey, thickness);
            draw_list->AddLine(ImVec2(x + sz * 0.0f, y + sz * 0.7f), ImVec2(x + sz * 0.2f, y + sz * 0.7f), b ? yellow : grey, thickness);
            draw_list->AddLine(ImVec2(x + sz * 0.8f, y + sz * 0.5f), ImVec2(x + sz * 1.0f, y + sz * 0.5f), o ? yellow : grey, thickness);

            // Body
            draw_list->AddLine(ImVec2(x + sz * 0.2f, y + sz * 0.2f), ImVec2(x + sz * 0.2f, y + sz * 0.8f), yellow, thickness);
            draw_list->AddLine(ImVec2(x + sz * 0.2f, y + sz * 0.2f), ImVec2(x + sz * 0.5f, y + sz * 0.2f), yellow, thickness);
            draw_list->AddLine(ImVec2(x + sz * 0.5f, y + sz * 0.2f), ImVec2(x + sz * 0.7f, y + sz * 0.3f), yellow, thickness);
            draw_list->AddLine(ImVec2(x + sz * 0.7f, y + sz * 0.3f), ImVec2(x + sz * 0.8f, y + sz * 0.5f), yellow, thickness);
            draw_list->AddLine(ImVec2(x + sz * 0.7f, y + sz * 0.7f), ImVec2(x + sz * 0.8f, y + sz * 0.5f), yellow, thickness);
            draw_list->AddLine(ImVec2(x + sz * 0.5f, y + sz * 0.8f), ImVec2(x + sz * 0.7f, y + sz * 0.7f), yellow, thickness);     
            draw_list->AddLine(ImVec2(x + sz * 0.2f, y + sz * 0.8f), ImVec2(x + sz * 0.5f, y + sz * 0.8f), yellow, thickness);
            draw_list->AddCircle(ImVec2(x + sz * 0.8f, y + sz * 0.5f), sz * 0.05f, yellow, 8, thickness * 2);
        };

        auto draw_rect = [&](float x, float y) {
            draw_list->AddRect(ImVec2(x + sz * 0.1f, y + sz * 0.1f), ImVec2(x + sz * 0.9f, y + sz * 0.9f), grey, 10.0f, ImDrawCornerFlags_All, thickness);
        };

        auto draw_constant_zero = [&](float x, float y) { 
            draw_rect(x, y);
            draw_list->AddCircle(ImVec2(x + sz * 0.9f, y + sz * 0.5f), sz * 0.05f, grey, 8, thickness);
            draw_list->AddText(nullptr, 20.0f, ImVec2(x + sz * 0.4f, y + sz * 0.3f), grey, "0");
        };

        auto draw_constant_one = [&](float x, float y) {
            draw_rect(x, y);
            draw_list->AddCircle(ImVec2(x + sz * 0.9f, y + sz * 0.5f), sz * 0.05f, grey, 8, thickness);
            draw_list->AddText(nullptr, 20.0f, ImVec2(x + sz * 0.4f, y + sz * 0.3f), yellow, "1");
        };

        auto draw_probe = [&](float x, float y, bool active) {
            draw_list->AddCircle(ImVec2(x + sz * 0.5f, y + sz * 0.5f), sz * 0.5f, grey, 16, thickness);
            draw_list->AddCircle(ImVec2(x + sz * 0.0f, y + sz * 0.5f), sz * 0.05f, grey, 8, thickness);
            draw_list->AddText(nullptr, 20.0f, ImVec2(x + sz * 0.4f, y + sz * 0.3f), active ? yellow : grey, active ? "1" : "0");
        };

        // TODO: Make this DND!
        // NAND Button
        draw_nand(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y);
        if (ImGui::Button("##DND_NAND", ImVec2(sz, sz)))
        {
            sim.NewNAND(200, 200);
        }

        // Drawing/Canvas area       
        ImVec2 canvas_pos = ImGui::GetCursorScreenPos();     // ImDrawList API uses screen coordinates!
        ImVec2 canvas_size = ImGui::GetContentRegionAvail(); // Resize canvas to what's available

        draw_list->AddRect(canvas_pos, ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y), IM_COL32(255, 255, 255, 255));

        // Draw Simulation items
        auto delta = io.MouseDelta;
        auto pos = ImGui::GetMousePos();
        for (auto& nand : sim.nands)
        {
            ImGui::PushID(nand->id);

            auto inputa = sim.GetNode(nand->inputa_id);
            auto inputb = sim.GetNode(nand->inputb_id);
            auto output = sim.GetNode(nand->output_id);
            draw_nand(nand->x, nand->y, inputa->active, inputb->active, output->active);

            ImGui::SetCursorScreenPos(ImVec2(nand->x, nand->y));
            ImGui::InvisibleButton("nand" + nand->id, ImVec2(64.0f, 64.0f));
            if (ImGui::IsItemHovered())
            {
                draw_list->AddRect(ImVec2(nand->x, nand->y), ImVec2(nand->x + 64.0f, nand->y + 64.0f), yellow);
                draw_list->AddText(nullptr, 0.0f, ImVec2(nand->x + 64.0f, nand->y + 64.0f), yellow, std::to_string(nand->id).c_str());

                if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
                {
                }
            }

            if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
            {
                auto delta = io.MouseDelta;
                nand->x += delta.x;
                nand->y += delta.y;

                inputa->x = nand->x;
                inputa->y = nand->y + (64 * 0.3f);

                inputb->x = nand->x;
                inputb->y = nand->y + (64 * 0.7f);

                output->x = nand->x + 64;
                output->y = nand->y + 32;
            }
            ImGui::PopID();
        }

        for (auto& node : sim.nodes)
        {
            ImGui::PushID(node->id);

            auto bounds_ul = ImVec2(node->x - 16.0f, node->y - 16.0f);
            auto bounds_lr = ImVec2(node->x + 16.0f, node->y + 16.0f);

            draw_list->AddText(nullptr, 0.0f, ImVec2(node->x, node->y), node->active ? yellow : grey, node->active ? "1" : "0");
            for (auto& driven_id : node->driving_ids)
            {
                auto driven = sim.GetNode(driven_id);
                draw_list->AddLine(ImVec2(node->x, node->y), ImVec2(driven->x, driven->y), node->active ? yellow : grey, thickness);
            }

            ImGui::SetCursorScreenPos(ImVec2(node->x - 16.0f, node->y - 16.0f));
            ImGui::InvisibleButton("node" + node->id, ImVec2(32.0f, 32.0f));

            if (ImGui::IsMouseHoveringRect(bounds_ul, bounds_lr))
            {
                draw_list->AddRect(ImVec2(node->x - 16.0f, node->y - 16.0f), ImVec2(node->x + 16.0f, node->y + 16.0f), yellow);
                draw_list->AddText(nullptr, 0.0f, ImVec2(node->x + 16.0f, node->y + 16.0f), yellow, std::to_string(node->id).c_str());

                if (ImGui::IsMouseClicked(ImGuiMouseButton_Middle) || ImGui::IsKeyPressed(ImGuiKey_Backspace))
                {
                    node->driving_ids.clear();
                }
            }

            if (ImGui::IsMouseHoveringRect(bounds_ul, bounds_lr) && ImGui::IsMouseDragging(ImGuiMouseButton_Left) && !node->attached_nand)
            {
                node->x += delta.x;
                node->y += delta.y;
            }
                                      
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceNoHoldToOpenOthers | ImGuiDragDropFlags_SourceAutoExpirePayload))
            {
                draw_list->AddLine(ImVec2(node->x, node->y), ImVec2(pos.x, pos.y), node->active ? yellow : grey, thickness);
                ImGui::SetDragDropPayload("DND_NODE_PAYLOAD", &node->id, sizeof(uint32_t));
                ImGui::EndDragDropSource();
            }

            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_NODE_PAYLOAD", ImGuiDragDropFlags_AcceptNoDrawDefaultRect))
                {
                    auto from_node_id = *(uint32_t*)payload->Data;
                    sim.ConnectNodes(from_node_id, node->id);
                }
                ImGui::EndDragDropTarget();
            }
            ImGui::PopID();
        }

        for (auto& entry : sim.node_lookup)
        {
            auto str = entry.first;
            auto node = entry.second;
            draw_list->AddText(nullptr, 0.0f, ImVec2(node->x, node->y - 16.0f), yellow, str.c_str());
        }

        for (auto& label : sim.labels)
        {
            ImGui::SetCursorScreenPos(ImVec2(label.x, label.y));
            ImGui::Text(label.text.c_str());
            auto rect = ImGui::GetItemRectSize();
            if (ImGui::IsMouseHoveringRect(ImVec2(label.x, label.y), ImVec2(label.x + rect.x, label.y + rect.y)))
            {
                draw_list->AddRect(ImVec2(label.x, label.y), ImVec2(label.x + rect.x, label.y + rect.y), yellow);
                if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
                {
                    label.x += delta.x;
                    label.y += delta.y;
                }
            }
        }

        if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
        {
            sim.DumpJSONObjectToFile(sim.DumpToJSONObject(), "dump.json");
        }
    }
    ImGui::End();

    Frame_Submit();
}

void Window::Sim()
{
    sim.Step();
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
            if (ImGui::MenuItem("Exit"))
            {
                done = true;
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Simulation"))
        {
            if (ImGui::MenuItem("Step"))
            {
                //sim.Step();
            }
            if (ImGui::MenuItem("Clear"))
            {
                sim.step_count = 0;
                sim.step_time = 0;
                sim.nands.clear();
                sim.nodes.clear();
                sim.nand_lookup.clear();
                sim.node_lookup.clear();
                sim.labels.clear();
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
