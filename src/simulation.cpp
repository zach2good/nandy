#include "simulation.h"

Simulation::Simulation()
{
    LoadFromJSONFile("dump.json");
    Step();
}

void Simulation::Step()
{
    // TODO: This is accurate, but inefficient
    for (auto& entry : node_lookup)
    {
        q.push(entry.second);
    }

    auto start_time = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < 1; i++) // TODO: Reduce this to as small as possible
    {
        while (!q.empty())
        {
            auto& component = q.front();
            q.pop();

            if (component->is_nand)
            {
                auto nand = GetNAND(component->id);

                auto inputa = nodes[nand->inputa_id];
                auto inputb = nodes[nand->inputb_id];
                auto output = nodes[nand->output_id];

                bool starting_output_state = output->active;
                output->active = !(inputa->active && inputb->active);

                if (output->active != starting_output_state)
                {
                    q.push(output);
                }
            }
            else // is_node
            {
                auto node = GetNode(component->id);
                for (auto& connected_node_id : node->driving_ids)
                {
                    auto connected_node = nodes[connected_node_id];
                    if (node->active != connected_node->active)
                    {
                        connected_node->active = node->active;
                        q.push(connected_node);
                    }
                }

                if (node->attached_nand && node->id != nands[node->nand_id]->output_id)
                {
                    auto connected_nand = nands[node->nand_id];
                    q.push(connected_nand);
                }
            }
        }
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    auto time_taken = end_time - start_time;
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(time_taken);
    step_time = microseconds.count();
    ++step_count;
}

std::shared_ptr<nand_t> Simulation::NewNAND(int x, int y, const std::string& name)
{
    auto nand = std::make_shared<nand_t>();
    nands.emplace_back(nand);
    nand->id = nands.size() - 1;
    nand->x = x;
    nand->y = y;
    nand->is_nand = true;

    if (!name.empty())
    {
        nand_lookup[name] = nand;
    }

    // Attach Nodes
    auto inputa = NewNode(x, y + (64 * 0.3f));
    auto inputb = NewNode(x, y + (64 * 0.7f));
    auto output = NewNode(x + 64, y + 32);

    inputa->attached_nand = true;
    inputa->nand_id = nand->id;

    inputb->attached_nand = true;
    inputb->nand_id = nand->id;

    output->attached_nand = true;
    output->nand_id = nand->id;

    nand->inputa_id = inputa->id;
    nand->inputb_id = inputb->id;
    nand->output_id = output->id;

    return nand;
}

std::shared_ptr<node_t> Simulation::NewNode(int x, int y, const std::string& name)
{
    auto node = std::make_shared<node_t>();
    nodes.emplace_back(node);
    node->id = nodes.size() - 1;
    node->is_nand = false;

    node->x = x;
    node->y = y;

    if (!name.empty())
    {
        node_lookup[name] = node;
    }

    return node;
}
