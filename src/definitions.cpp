#include "definitions.h"

void node_t::Simulate(std::queue<std::shared_ptr<component_t>>* q)
{
    for (auto& connected_node : connected_nodes)
    {
        if (!connected_node->locked)
        {
            connected_node->active = this->active;
            connected_node->dirty = true;

            q->push(connected_node);
        }
    }

    if (connected_nand != nullptr)
    {
        q->push(std::static_pointer_cast<component_t>(connected_nand));
    }
}

void nand_t::Simulate(std::queue<std::shared_ptr<component_t>>* q)
{
    bool starting_output_state = this->output_node->active;
    bool next_output_state = !(this->inputA_node->active && this->inputB_node->active);

    this->output_node->active = next_output_state;
    this->output_node->dirty = true;

    q->push(this->output_node);
}
