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
        // Hard-coded sim for testing
        // NAND
        auto nand = NewNAND(300, 300, "nand");

        auto c0 = NewNode(200, 250, "c0");
        c0->active = true;
        c0->locked = true;

        auto c1 = NewNode(200, 400, "c1");
        c1->active = false;
        c1->locked = true;

        auto p0 = NewNode(400, 332, "p0");

        ConnectNodes(c0, nand->inputA_node);
        ConnectNodes(c1, nand->inputB_node);
        ConnectNodes(nand->output_node, p0);

        // NOT
        auto n0 = NewNode(500, 250, "n0");
        auto not = NewNAND(600, 300, "not");
        auto p1 = NewNode(700, 332, "p1");

        ConnectNodes(c0, n0);
        ConnectNodes(n0, not ->inputA_node);
        ConnectNodes(n0, not ->inputB_node);
        ConnectNodes(not ->output_node, p1);

        Step();
    }

    ~Simulation() = default;

    void Step()
    {
        // TODO: This is accurate, but inefficient
        for (auto& node : nodes)
        {
            q.push(node);
        }

        auto start_time = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < 4; i++) // TODO: Reduce this to as small as possible
        {
            while (!q.empty())
            {
                auto& component = q.front();
                q.pop();
                
                component->Simulate(&q);
            }
        }
        auto end_time = std::chrono::high_resolution_clock::now();
        auto time_taken = end_time - start_time;
        auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(time_taken);
        step_time = microseconds.count();
        ++step_count;
    }

    std::shared_ptr<node_t> NewNode(int x, int y, std::string name = "")
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

        node->x = x;
        node->y = y;

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

    std::shared_ptr<nand_t> NewNAND(int x, int y, std::string name = "")
    {
        auto nand = std::make_shared<nand_t>();
        nands.emplace_back(nand);
        nand->id = nands.size() - 1;
        nand->self = nand;

        // Attach Nodes
        nand->inputA_node = NewNode(x, y + (64 * 0.3f));
        nand->inputB_node = NewNode(x, y + (64 * 0.7f));
        nand->output_node = NewNode(x + 64, y + 32);

        nand->inputA_node->connected_nand = nand;
        nand->inputB_node->connected_nand = nand;
        nand->output_node->connected_nand = nand;

        nand->output_node->active = true;

        if (!name.empty())
        {
            nand->name = name;
            nand_lookup[name] = nand;
        }

        nand->x = x;
        nand->y = y;

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
        n0->driving.emplace_back(n1);
    };

    void SetActive(std::shared_ptr<node_t> n, bool new_active)
    {
        n->active = new_active;
        n->dirty = true;
        q.push(n);
    }

    std::shared_ptr<node_t> NewConstant(int x, int y, bool active)
    {
        auto constant = std::make_shared<node_t>();
        nodes.emplace_back(constant);
        constant->id = nodes.size() - 1;
        constant->self = constant;

        constant->locked = true;
        constant->active = active;
        constant->drawable = true;
        nodes.emplace_back(constant);

        constant->x = x;
        constant->y = y;

        return constant;
    }

    std::shared_ptr<node_t> NewProbe(int x, int y)
    {
        auto probe = std::make_shared<node_t>();
        nodes.emplace_back(probe);
        probe->id = nodes.size() - 1;
        probe->self = probe;

        nodes.emplace_back(probe);

        probe->x = x;
        probe->y = y;

        return probe;
    }

    // Step Information
    long step_count = 0;
    long long step_time = 0;

    // Components
    std::vector<std::shared_ptr<nand_t>> nands;
    std::vector<std::shared_ptr<node_t>> nodes;
    std::unordered_map<std::string, std::shared_ptr<nand_t>> nand_lookup;
    std::unordered_map<std::string, std::shared_ptr<node_t>> node_lookup;

    // Simulation
    std::queue<std::shared_ptr<component_t>> q;
};
