#pragma once

#include <chrono>
#include <cstdio>
#include <vector>

#include <SDL.h>

#include <glad/glad.h>

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"

class Simulation;

class Window
{
public:
    Window();
    ~Window();

    void HandleEvents();
    void Draw(Simulation& sim);

    void Frame_Prepare();
    void Frame_Toolbar(Simulation& sim);
    void Frame_Submit();

    bool QuitRequested()
    {
        return done;
    }
    SDL_Window* GetSDLWindow()
    {
        return window;
    };

private:
    void CherryTheme();

    bool done = false;
    SDL_Window* window;
    SDL_GLContext gl_context;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
};