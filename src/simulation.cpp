#include "simulation.h"

Simulation::Simulation()
{
    // Clock
    clk = NewNode(50, 200, "clk");

    // Long NOT chain
    node_t* last_node = clk;
    int size_step = 64;
    for (int i = 0; i < std::pow(2, 16) + 1; ++i)
    {
        auto* nand = NewNAND(100 + (i * size_step), 200);

        ConnectNodes(last_node->id, nand->inputa_id);
        ConnectNodes(last_node->id, nand->inputb_id);

        last_node = nodes[nand->output_id];
    }

    Step();
    step_count = 0;
    queue_ops = 0;
}

void Simulation::Step()
{
    queue_ops = 0;

    clk->active = !clk->active;

    for(auto& entry : node_lookup)
    {
        q.push(entry.second);
    }

    auto start_time = std::chrono::high_resolution_clock::now();
    while (!q.empty())
    {
        auto* component = q.front();
        q.pop();
        ++queue_ops;

        if (component->type == COMPONENT::NAND)
        {
            auto* nand = GetNAND(component->id);

            auto* inputa = nodes[nand->inputa_id];
            auto* inputb = nodes[nand->inputb_id];
            auto* output = nodes[nand->output_id];

            bool starting_output_state = output->active;
            output->active = !(inputa->active && inputb->active);

            if (!nand->dirty || starting_output_state != output->active)
            {
                nand->dirty = true;
                q.push(output);
            }
        }
        else if (component->type == COMPONENT::NODE)
        {
            auto* node = GetNode(component->id);

            for (auto& connected_node_id : node->driving_ids)
            {
                auto* connected_node = nodes[connected_node_id];
                if (!connected_node->dirty || node->active != connected_node->active)
                {
                    connected_node->dirty = true;
                    connected_node->active = node->active;
                    q.push(connected_node);
                }
            }

            if (node->attached_nand)
            {
                auto* connected_nand = nands[node->nand_id];
                q.push(connected_nand);
            }
        }
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    auto time_taken = end_time - start_time;
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(time_taken);
    step_time = microseconds.count();
    ++step_count;
}

nand_t* Simulation::NewNAND(int x, int y, const std::string& name)
{
    auto* nand = new nand_t();
    nand->id = nands.size();

    nand->x = x;
    nand->y = y;
    nand->type = COMPONENT::NAND;
    nand->dirty = false;

    if (!name.empty())
    {
        nand_lookup[name] = nand;
    }

    // Attach Nodes
    auto* inputa = NewNode(x, y + (64 * 0.3f));
    auto* inputb = NewNode(x, y + (64 * 0.7f));
    auto* output = NewNode(x + 64, y + 32);

    inputa->attached_nand = true;
    inputa->nand_id = nand->id;

    inputb->attached_nand = true;
    inputb->nand_id = nand->id;

    output->attached_nand = true;
    output->nand_id = nand->id;

    nand->inputa_id = inputa->id;
    nand->inputb_id = inputb->id;
    nand->output_id = output->id;

    nands.emplace_back(nand);

    return nand;
}

node_t* Simulation::NewNode(int x, int y, const std::string& name)
{
    auto* node = new node_t();
    node->id = nodes.size();
    node->type = COMPONENT::NODE;
    node->x = x;
    node->y = y;

    if (!name.empty())
    {
        node_lookup[name] = node;
    }

    nodes.emplace_back(node);

    return node;
}
