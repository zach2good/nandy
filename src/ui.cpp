#include "ui.h"

#include <cstdio>

#include <glad/glad.h>

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"

UI::UI(Simulation& sim)
: done(false)
, sim(sim)
{
}

void UI::Draw(Simulation& _sim)
{
    Draw_Menubar();
    Draw_Toolbar();
    Draw_Canvas();
}

void UI::Draw_Menubar()
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
                sim.Step();
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

void UI::Draw_Node(node_t* node)
{
    /*
    auto& io = ImGui::GetIO();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    auto pos = ImGui::GetMousePos();
    auto delta = io.MouseDelta;
    auto bounds_ul = ImVec2(node->x - 16.0f, node->y - 16.0f);
    auto bounds_lr = ImVec2(node->x + 16.0f, node->y + 16.0f);

    ImGui::PushID(node->id);

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
    */
}

void UI::Draw_NAND(nand_t* nand)
{
    auto& io = ImGui::GetIO();
    auto pos = ImGui::GetMousePos();
    auto delta = io.MouseDelta;

    auto bounds_ul = ImVec2(nand->x - 16.0f, nand->y - 16.0f);
    auto bounds_lr = ImVec2(nand->x + 16.0f, nand->y + 16.0f);

    ImGui::PushID(nand->id);

    auto inputa = sim.GetNode(nand->inputa_id);
    auto inputb = sim.GetNode(nand->inputb_id);
    auto output = sim.GetNode(nand->output_id);
    Draw_Primitive_NAND(nand->x, nand->y, inputa->active, inputb->active, output->active);

    ImGui::SetCursorScreenPos(ImVec2(nand->x, nand->y));
    ImGui::InvisibleButton("nand" + nand->id, ImVec2(64.0f, 64.0f));
    if (ImGui::IsItemHovered())
    {
        //draw_list->AddRect(ImVec2(nand->x - offset_x, nand->y - offset_y), ImVec2(nand->x + 64.0f - offset_x, nand->y + 64.0f - offset_y), yellow);
        //draw_list->AddText(nullptr, 0.0f, ImVec2(nand->x + 64.0f - offset_x, nand->y + 64.0f - offset_y), yellow, std::to_string(nand->id).c_str());

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

void UI::Draw_Primitive_NAND(float x, float y, float w, float h, bool a, bool b, bool o)
{
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    // Legs
    draw_list->AddLine(ImVec2(x + w * 0.0f, y + h * 0.3f), ImVec2(x + w * 0.2f, y + h * 0.3f), a ? yellow : grey, thickness);
    draw_list->AddLine(ImVec2(x + w * 0.0f, y + h * 0.7f), ImVec2(x + w * 0.2f, y + h * 0.7f), b ? yellow : grey, thickness);
    draw_list->AddLine(ImVec2(x + w * 0.8f, y + h * 0.5f), ImVec2(x + w * 1.0f, y + h * 0.5f), o ? yellow : grey, thickness);

    // Body
    draw_list->AddLine(ImVec2(x + w * 0.2f, y + h * 0.2f), ImVec2(x + w * 0.2f, y + h * 0.8f), yellow, thickness);
    draw_list->AddLine(ImVec2(x + w * 0.2f, y + h * 0.2f), ImVec2(x + w * 0.5f, y + h * 0.2f), yellow, thickness);
    draw_list->AddLine(ImVec2(x + w * 0.5f, y + h * 0.2f), ImVec2(x + w * 0.7f, y + h * 0.3f), yellow, thickness);
    draw_list->AddLine(ImVec2(x + w * 0.7f, y + h * 0.3f), ImVec2(x + w * 0.8f, y + h * 0.5f), yellow, thickness);
    draw_list->AddLine(ImVec2(x + w * 0.7f, y + h * 0.7f), ImVec2(x + w * 0.8f, y + h * 0.5f), yellow, thickness);
    draw_list->AddLine(ImVec2(x + w * 0.5f, y + h * 0.8f), ImVec2(x + w * 0.7f, y + h * 0.7f), yellow, thickness);
    draw_list->AddLine(ImVec2(x + w * 0.2f, y + h * 0.8f), ImVec2(x + w * 0.5f, y + h * 0.8f), yellow, thickness);
    draw_list->AddCircle(ImVec2(x + w * 0.8f, y + h * 0.5f), w * 0.05f, yellow, 8, thickness * 2);
}

void UI::Draw_Primitive_Node(float x, float y, float w, float h, bool active)
{
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    draw_list->AddCircle(ImVec2(x, y), 2.0f * zoom, active ? yellow : grey);
}

void UI::Draw_Toolbar()
{
    ImGui::Text("Step Count: %ld", sim.step_count);

    ImGui::SameLine();
    ImGui::Text("| Step Time: %lldus", sim.step_time);

    ImGui::SameLine();
    ImGui::Text("| Queue Ops: %ld", sim.queue_ops);

    ImGui::SameLine();
    ImGui::Text("| Nodes: %zu", sim.nodes.size());

    ImGui::SameLine();
    ImGui::Text("| NANDs: %zu", sim.nands.size());

    ImGui::SameLine();
    ImGui::Text("| Zoom: %f", zoom);

    ImGui::SameLine();
    ImGui::Text("| Offset X: %f", offset_x);

    ImGui::SameLine();
    ImGui::Text("| Offset Y: %f", offset_y);

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
              sim.NewNode(300, 300, name);
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
    ImGui::Text("Add label (enter):");
    ImGui::SameLine();
    std::array<char, 20> arr{ { 0 } };
    ImGui::PushItemWidth(128);
    if (ImGui::InputText("##", arr.data(), arr.size(), ImGuiInputTextFlags_EnterReturnsTrue))
    {
        std::string str(arr.data(), arr.size());
        sim.AddLabel(200, 200, str);
    }

    // TODO: Make this DND!
    // NAND Button
    if (ImGui::Button("NAND", ImVec2(sz, sz)))
    {
        static uint32_t nand_label_id = 0;
        sim.NewNAND(300, 300, std::to_string(nand_label_id++));
    }

    ImGui::SameLine();
    if (ImGui::Button("Node", ImVec2(sz, sz)))
    {
        static uint32_t nand_label_id = 0;
        sim.NewNode(300, 300, std::to_string(nand_label_id++));
    }

    // False vertical seperator
    ImGui::SameLine();
    ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
    ImGui::Button("", ImVec2(2.0f, sz));
    ImGui::PopStyleVar();
    ImGui::PopItemFlag();

    ImGui::SameLine();
    if (ImGui::Button("Reset", ImVec2(sz, sz)))
    {
        offset_x = 0.0f;
        offset_y = 0.0f;
        zoom = 1.0f;
    }

    ImGui::SameLine();
    if (ImGui::Button("Zoom +", ImVec2(sz, sz)))
    {
        zoom *= 1.5f;
    }
    ImGui::SameLine();
    if (ImGui::Button("Zoom -", ImVec2(sz, sz)))
    {
        zoom *= 0.5f;
    }
    zoom = std::clamp(zoom, 0.02f, 10.0f);
    thickness = 2.0f * zoom;
}

void UI::Draw_Canvas()
{
    // Setup
    ImGuiIO& io = ImGui::GetIO();
    auto mouse_delta = io.MouseDelta;
    auto mouse_pos = ImGui::GetMousePos();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    canvas_pos = ImGui::GetCursorScreenPos();     // ImDrawList API uses screen coordinates!
    canvas_size = ImGui::GetContentRegionAvail(); // Resize canvas to what's available

    // Clear previous state
    mouse_over_item = nullptr;

    // Clipping
    ImGui::PushClipRect(canvas_pos, ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y), false);

    // Outline
    draw_list->AddRect(canvas_pos, ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y), IM_COL32(255, 255, 255, 255));

    // Canvas
    auto is_visible = [&](float x, float y)
    {
        return !(canvas_pos.x + x * zoom + offset_x + sz * zoom < canvas_pos.x ||
                 canvas_pos.y + y * zoom + offset_y + sz * zoom < canvas_pos.y ||
                 canvas_pos.x + x * zoom + offset_x > canvas_pos.x + canvas_size.x ||
                 canvas_pos.y + y * zoom + offset_y > canvas_pos.y + canvas_size.y);
    };

    // Cull by visibility
    std::vector<nand_t*> drawable_nands;
    std::vector<node_t*> drawable_nodes;
    for (auto& nand : sim.nands)
    {
        auto x = nand->x;
        auto y = nand->y;
        if (is_visible(x, y))
        {
            drawable_nands.emplace_back(nand);
        }
    }
    for (auto& node : sim.nodes)
    {
        auto x = node->x;
        auto y = node->y;
        if (is_visible(x, y))
        {
            drawable_nodes.emplace_back(node);
        }
    }

    // Draw primitives and collect mouse-over components
    // NOTE: Priority is given to Nodes, since they are smaller and harder to pick
    for (auto& node : drawable_nodes)
    {
        // Draw_Node(node);
        auto node_sz = sz / 4.0f;
        auto canvas_x = canvas_pos.x + node->x * zoom + offset_x;
        auto canvas_y = canvas_pos.y + node->y * zoom + offset_y;

        Draw_Primitive_Node(canvas_x, canvas_y, node_sz * zoom, node_sz * zoom, node->active);
        for(auto& driving_id : node->driving_ids)
        {
            auto* driving_node = sim.GetNode(driving_id);
            auto driving_canvas_x = canvas_pos.x + driving_node->x * zoom + offset_x;
            auto driving_canvas_y = canvas_pos.y + driving_node->y * zoom + offset_y;
            draw_list->AddLine(ImVec2(canvas_x, canvas_y), ImVec2(driving_canvas_x, driving_canvas_y), node->active ? yellow : grey, thickness);
        }

        ImGui::SetCursorScreenPos(ImVec2(canvas_x  - ((node_sz / 2.0f) * zoom), canvas_y - ((node_sz / 2.0f) * zoom)));
        auto name = std::string("node") + std::to_string(node->id);
        ImGui::InvisibleButton(name.c_str(), ImVec2(node_sz * zoom, node_sz * zoom));
        if (ImGui::IsItemHovered())
        {
            mouse_over_item = node;
        }
    }
    for (auto& nand : drawable_nands)
    {
        // Draw_NAND(nand);
        auto canvas_x = canvas_pos.x + nand->x * zoom + offset_x;
        auto canvas_y = canvas_pos.y + nand->y * zoom + offset_y;

        bool a = sim.GetNode(nand->inputa_id)->active;
        bool b = sim.GetNode(nand->inputb_id)->active;
        bool o = sim.GetNode(nand->output_id)->active;

        Draw_Primitive_NAND(canvas_x, canvas_y, sz * zoom, sz * zoom, a, b, o);

        ImGui::SetCursorScreenPos(ImVec2(canvas_x, canvas_y));
        auto name = std::string("nand") + std::to_string(nand->id);
        ImGui::InvisibleButton(name.c_str(), ImVec2(sz * zoom, sz * zoom));
        if (ImGui::IsItemHovered() && !mouse_over_item)
        {
            mouse_over_item = nand;
        }
    }
    if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && !mouse_over_item)
    {
        dragging_canvas = true;
    }

    // Draw "hover" box
    if (!dragging_item && !dragging_canvas)
    {
        if (mouse_over_item && mouse_over_item->type == COMPONENT::NODE)
        {
            auto* node = static_cast<node_t*>(mouse_over_item);
            auto node_sz = sz / 4.0f;
            auto canvas_x = canvas_pos.x + node->x * zoom + offset_x - ((node_sz / 2.0f) * zoom);
            auto canvas_y = canvas_pos.y + node->y * zoom + offset_y - ((node_sz / 2.0f) * zoom);

            draw_list->AddRect(ImVec2(canvas_x, canvas_y), ImVec2(canvas_x + node_sz * zoom, canvas_y + node_sz * zoom), yellow);

            if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && !node->attached_nand)
            {
                dragging_item = node;
            }

            if (ImGui::IsMouseDown(ImGuiMouseButton_Right) && !connecting_node)
            {
                connecting_node = node;
            }

            if (ImGui::IsMouseDown(ImGuiMouseButton_Middle))
            {
                node->driving_ids.clear();
            }
        }
        else if (mouse_over_item && mouse_over_item->type == COMPONENT::NAND)
        {
            auto* nand = static_cast<nand_t*>(mouse_over_item);
            auto canvas_x = canvas_pos.x + nand->x * zoom + offset_x;
            auto canvas_y = canvas_pos.y + nand->y * zoom + offset_y;

            draw_list->AddRect(ImVec2(canvas_x, canvas_y), ImVec2(canvas_x + sz * zoom, canvas_y + sz * zoom), yellow);

            if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
            {
                dragging_item = nand;
            }
        }
    }

    if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
    {
        if (dragging_item && dragging_item->type == COMPONENT::NAND)
        {
            auto* nand = static_cast<nand_t*>(dragging_item);
            auto canvas_x = canvas_pos.x + nand->x * zoom + offset_x;
            auto canvas_y = canvas_pos.y + nand->y * zoom + offset_y;
            auto zoomed_delta_x = mouse_delta.x * (1.0f / zoom);
            auto zoomed_delta_y = mouse_delta.y * (1.0f / zoom);

            draw_list->AddRect(ImVec2(canvas_x, canvas_y), ImVec2(canvas_x + sz * zoom, canvas_y + sz * zoom), yellow);

            nand->x += zoomed_delta_x;
            nand->y += zoomed_delta_y;

            sim.GetNode(nand->inputa_id)->x += zoomed_delta_x;
            sim.GetNode(nand->inputa_id)->y += zoomed_delta_y;

            sim.GetNode(nand->inputb_id)->x += zoomed_delta_x;
            sim.GetNode(nand->inputb_id)->y += zoomed_delta_y;

            sim.GetNode(nand->output_id)->x += zoomed_delta_x;
            sim.GetNode(nand->output_id)->y += zoomed_delta_y;
        }
        else if (dragging_item && dragging_item->type == COMPONENT::NODE)
        {
            auto* node = static_cast<node_t*>(dragging_item);
            auto node_sz = sz / 4.0f;
            auto canvas_x = canvas_pos.x + node->x * zoom + offset_x;
            auto canvas_y = canvas_pos.y + node->y * zoom + offset_y;

            draw_list->AddRect(ImVec2(canvas_x, canvas_y), ImVec2(canvas_x + node_sz * zoom, canvas_y + node_sz * zoom), yellow);

            node->x += mouse_delta.x * (1.0f / zoom);
            node->y += mouse_delta.y * (1.0f / zoom);
        }
        else if (dragging_canvas) // Dragging "camera" around
        {
            offset_x += mouse_delta.x;
            offset_y += mouse_delta.y;

            offset_x = std::min(0.0f, offset_x);
            offset_y = std::min(0.0f, offset_y);
        }
    }
    else if (ImGui::IsMouseDragging(ImGuiMouseButton_Right) && connecting_node)
    {
        auto canvas_x = canvas_pos.x + connecting_node->x * zoom + offset_x;
        auto canvas_y = canvas_pos.y + connecting_node->y * zoom + offset_y;
        draw_list->AddLine(ImVec2(canvas_x, canvas_y), ImVec2(mouse_pos.x, mouse_pos.y), connecting_node->active ? yellow : grey, thickness);
    }

    auto snap_value = 50.0f;
    if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
    {
        /*
        if (dragging_item && dragging_item->type == COMPONENT::NAND)
        {
            // "Snap" to nearest 25px
            auto* nand = static_cast<nand_t*>(dragging_item);

            nand->x = ((nand->x + snap_value / 2.0f) / snap_value) * snap_value;
            nand->y = ((nand->y + snap_value / 2.0f) / snap_value) * snap_value;
        }
        */

        dragging_item = nullptr;
        dragging_canvas = false;
    }

    if (ImGui::IsMouseReleased(ImGuiMouseButton_Right))
    {
        if (connecting_node && mouse_over_item && mouse_over_item->type == COMPONENT::NODE && connecting_node->id != mouse_over_item->id)
        {
            sim.ConnectNodes(connecting_node->id, mouse_over_item->id);
        }
        connecting_node = nullptr;
    }

    ImGui::PopClipRect();
}
