#pragma once

#include <chrono>
#include <memory>
#include <queue>
#include <unordered_map>
#include <vector>

#include "definitions.h"

class Simulation
{
public:
    Simulation()
    {
        // Define High Constants
        auto vcc = NewNode("vcc");
        AddConstant(vcc, true, true);

        // Define Low Constants
        auto gnd = NewNode("gnd");
        AddConstant(gnd, true, false);

        // Define Others
        auto clk = NewNode("clk");
        AddConstant(clk, true, false);

        auto input = NewNode("input");
        AddConstant(input, true, false);

        auto input0 = NewNode("input0");
        AddConstant(input0, true, false);

        auto input1 = NewNode("input1");
        AddConstant(input1, true, false);

        auto input2 = NewNode("input2");
        AddConstant(input2, true, false);

        auto output = NewNode("output");
        AddConstant(output, false, false);

        auto output0 = NewNode("output0");
        AddConstant(output0, false, false);

        auto output1 = NewNode("output1");
        AddConstant(output1, false, false);

        auto output2 = NewNode("output2");
        AddConstant(output2, false, false);
    }

    ~Simulation() = default;

    void Step()
    {
        auto start_time = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < 4; i++) // TODO: Reduce this to as small as possible
        {
            for (auto& constant : constants)
            {
                q.push(constant);
            }

            while (!q.empty())
            {
                auto& component = q.front();
                q.pop();
                
                component->Simulate(&q);
            }
        }
        auto end_time = std::chrono::high_resolution_clock::now();
        auto time_taken = end_time - start_time;
        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(time_taken);
        step_time = milliseconds.count();
        ++step_count;
    }

    std::shared_ptr<node_t> NewNode(std::string name = "")
    {
        auto node = std::make_shared<node_t>();
        nodes.emplace_back(node);
        node->id = nodes.size() - 1;
        node->self = node;

        if (!name.empty())
        {
            node->name = name;
            node_lookup[name] = node;
        }

        return node;
    }

    std::shared_ptr<node_t> GetNode(uint16_t id)
    {
        return nodes[id];
    }

    std::shared_ptr<node_t> GetNode(std::string name)
    {
        return node_lookup[name];
    }

    std::shared_ptr<nand_t> NewNAND(std::string name = "")
    {
        auto nand = std::make_shared<nand_t>();
        nands.emplace_back(nand);
        nand->id = nands.size() - 1;
        nand->self = nand;

        // Attach Nodes
        nand->inputA_node = NewNode();
        nand->inputB_node = NewNode();
        nand->output_node = NewNode();

        nand->inputA_node->connected_nand = nand;
        nand->inputB_node->connected_nand = nand;
        nand->output_node->connected_nand = nand;

        nand->output_node->active = true;

        if (!name.empty())
        {
            nand->name = name;
            nand_lookup[name] = nand;
        }

        return nand;
    }

    std::shared_ptr<nand_t> GetTransistor(uint16_t id)
    {
        return nands[id];
    }

    std::shared_ptr<nand_t> GetTransistor(std::string name)
    {
        return nand_lookup[name];
    }

    void ConnectNodes(std::shared_ptr<node_t> n0, std::shared_ptr<node_t> n1)
    {
        n0->connected_nodes.emplace_back(n1);
    };

    
    void SetActive(std::shared_ptr<node_t> n, bool new_active)
    {
        n->active = new_active;
        n->dirty = true;
        q.push(n);
    }

    void AddConstant(std::shared_ptr<component_t> component, bool locked, bool active)
    {
        component->locked = locked;
        component->active = active;
        constants.emplace_back(component);
    }

    // Step Information
    long step_count = 0;
    long long step_time = 0;

private:
    // Components
    std::vector<std::shared_ptr<nand_t>> nands;
    std::vector<std::shared_ptr<node_t>> nodes;
    std::unordered_map<std::string, std::shared_ptr<nand_t>> nand_lookup;
    std::unordered_map<std::string, std::shared_ptr<node_t>> node_lookup;

    // Simulation
    std::queue<std::shared_ptr<component_t>> q;
    std::vector<std::shared_ptr<component_t>> constants;
};
