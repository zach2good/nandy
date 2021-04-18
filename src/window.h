#pragma once

#include <SDL.h>

#include "simulation.h"
#include "ui.h"

class Window
{
public:
    Window();
    ~Window();

    void HandleEvents();
    void Step();
    void Draw();

    [[nodiscard]] bool QuitRequested() const
    {
        return done;
    }

    SDL_Window* GetSDLWindow()
    {
        return window;
    };

private:
    void Frame_Prepare();
    void Frame_Submit();

    bool done = false;
    SDL_Window* window;
    SDL_GLContext gl_context;

    Simulation sim;
    UI ui;
};