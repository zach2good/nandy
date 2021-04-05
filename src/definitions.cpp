#include "definitions.h"

void node_t::Simulate(std::queue<std::shared_ptr<component_t>>* q)
{
    for (auto& connected_node : driving)
    {
        connected_node->active = this->active;
        connected_node->dirty = true;

        q->push(connected_node);
    }

    if (connected_nand != nullptr)
    {
        connected_nand->Simulate(q);
    }
}

void nand_t::Simulate(std::queue<std::shared_ptr<component_t>>* q)
{
    bool starting_output_state = this->output_node->active;
    this->output_node->active = !(this->inputA_node->active && this->inputB_node->active);

    if (this->output_node->active != starting_output_state)
    {
        q->push(this->output_node);
    }
}
