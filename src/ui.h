#pragma once

#include "definitions.h"
#include "simulation.h"
#include "imgui.h"

class UI
{
public:
    UI(Simulation& sim);
    ~UI() = default;

    void Draw(Simulation& sim);
    [[nodiscard]] bool QuitRequested() const
    {
        return done;
    }

private:
    Simulation& sim;

    // Drawing Tasks
    void Draw_Menubar();
    void Draw_Toolbar();
    void Draw_Canvas();
    void Draw_NAND(nand_t* nand);
    void Draw_Node(node_t* node);

    // Primitives
    void Draw_Primitive_NAND(float x, float y, float w, float h, bool a = false, bool b = false, bool o = false);
    void Draw_Primitive_Node(float x, float y, float w, float h, bool active = false);

    // State
    bool done;
    enum UISTATE
    {
        DEFAULT,
        PLACING,
    } state;

    // Canvas
    ImVec2 canvas_pos;
    ImVec2 canvas_size;
    float offset_x = 0.0f;
    float offset_y = 0.0f;
    float zoom = 1.0f;
    const float sz = 64.0f;
    float thickness = 2.0f;

    // TODO: Replace all of this with a state machine
    component_t* mouse_over_item = nullptr;
    component_t* dragging_item = nullptr;
    node_t* connecting_node = nullptr;
    bool dragging_canvas = false;

    // Colours
    const ImU32 red = ImColor(ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
    const ImU32 yellow = ImColor(ImVec4(1.0f, 1.0f, 0.4f, 1.0f));
    const ImU32 grey = ImColor(ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
    const ImU32 green = ImColor(ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
};
