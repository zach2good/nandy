#pragma once

#include <chrono>
#include <memory>
#include <unordered_map>
#include <vector>

struct node_t;
struct wire_t;
struct transistor_t;

struct node_t
{
    uint16_t id;
    std::string name;

    std::vector<std::shared_ptr<wire_t>> connected_wires;

    bool active = false;
    bool locked = false;
};

struct wire_t
{
    uint16_t id;
    std::string name;

    std::vector<std::shared_ptr<node_t>> nodes;
};

struct transistor_t
{
    uint16_t id;
    std::string name;

    std::shared_ptr<node_t> input_node;
    std::shared_ptr<node_t> output_node;
    std::shared_ptr<node_t> enable_node;
};

class Simulation
{
public:
    Simulation()
    {
        // Define High Constants
        auto vcc = NewNode("vcc");

        vcc->active = true;
        vcc->locked = true;

        // Define Low Constants
        auto gnd = NewNode("gnd");

        gnd->active = false;
        gnd->locked = true;

        // Define Others
        auto clk = NewNode("clk");
        clk->locked = true;

        auto input = NewNode("input");
        input->locked = true;

        auto input0 = NewNode("input0");
        input0->locked = true;

        auto input1 = NewNode("input1");
        input1->locked = true;

        auto output = NewNode("output");

        auto output0 = NewNode("output0");

        auto output1 = NewNode("output1");

        // Looks good? *Clicks tongs a few times*
        Step();
    }

    ~Simulation() = default;

    void Step()
    {
        auto start_time = std::chrono::high_resolution_clock::now();

        for (size_t i = 0; i < 2; i++)
        {
            for (auto& node : nodes)
            {
                for (auto& wire : node->connected_wires)
                {
                }
            }

            for (auto& wire : wires)
            {
                bool activeState = false;
                for (auto& node : wire->nodes)
                {
                    if (node->active)
                    {
                        activeState = true;
                    }
                }

                for (auto& node : wire->nodes)
                {
                    if (!node->locked && activeState)
                    {
                        node->active = activeState;
                    }
                }
            }

            for (auto& transistor : transistors)
            {
                if (transistor->enable_node->active)
                {
                    transistor->output_node->active = transistor->input_node->active;
                }
                else
                {
                    transistor->output_node->active = false;
                }
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

    std::shared_ptr<wire_t> NewWire(std::string name = "")
    {
        auto wire = std::make_shared<wire_t>();
        wires.emplace_back(wire);
        wire->id = wires.size() - 1;

        if (!name.empty())
        {
            wire->name = name;
            wire_lookup[name] = wire;
        }

        return wire;
    }

    std::shared_ptr<wire_t> GetWire(uint16_t id)
    {
        return wires[id];
    }

    std::shared_ptr<wire_t> GetWire(std::string name)
    {
        return wire_lookup[name];
    }

    std::shared_ptr<transistor_t> NewTransistor(std::string name = "")
    {
        auto transistor = std::make_shared<transistor_t>();
        transistors.emplace_back(transistor);
        transistor->id = transistors.size() - 1;

        // Intel 8086: 29,000 transistors
        if (transistors.size() > 29'000)
        {
            throw "Intel 8086 Transistor Limit (29,000) exceeded!";
        }

        // Attach Nodes
        transistor->input_node = NewNode();
        transistor->output_node = NewNode();
        transistor->enable_node = NewNode();

        if (!name.empty())
        {
            transistor->name = name;
            transistor_lookup[name] = transistor;
        }

        return transistor;
    }

    std::shared_ptr<transistor_t> GetTransistor(uint16_t id)
    {
        return transistors[id];
    }

    std::shared_ptr<transistor_t> GetTransistor(std::string name)
    {
        return transistor_lookup[name];
    }

    void ConnectNodes(std::shared_ptr<node_t> n0, std::shared_ptr<node_t> n1)
    {
        std::shared_ptr<wire_t> wire = NewWire();

        wire->nodes.emplace_back(n0);
        wire->nodes.emplace_back(n1);

        n0->connected_wires.emplace_back(wire);
        n1->connected_wires.emplace_back(wire);
    };

    // Step Information
    long step_count = 0;
    long long step_time = 0;

private:
    // Components
    std::vector<std::shared_ptr<transistor_t>> transistors;
    std::vector<std::shared_ptr<wire_t>> wires;
    std::vector<std::shared_ptr<node_t>> nodes;

    std::unordered_map<std::string, std::shared_ptr<transistor_t>> transistor_lookup;
    std::unordered_map<std::string, std::shared_ptr<wire_t>> wire_lookup;
    std::unordered_map<std::string, std::shared_ptr<node_t>> node_lookup;
};
