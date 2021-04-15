#pragma once

#include <SDL.h>

#include "simulation.h"

class Window
{
public:
    Window(int w, int h);
    ~Window();

    void HandleEvents();
    void Draw();
    void Sim();

    void Frame_Prepare();
    void Frame_Toolbar(Simulation& sim);
    void Frame_Submit();

    bool QuitRequested() const
    {
        return done;
    }

    SDL_Window* GetSDLWindow()
    {
        return window;
    };

private:
    int m_width;
    int m_height;

    bool done = false;
    SDL_Window* window;
    SDL_GLContext gl_context;

    Simulation sim;
    void DrawNAND(nand_t*& nand);
    void DrawNode(node_t*& node);
};