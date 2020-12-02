#pragma once

#include <chrono>
#include <memory>
#include <queue>
#include <unordered_map>
#include <vector>

struct node_t;
struct nand_t;

struct component_t
{
    virtual ~component_t() = default;
    virtual void Simulate(std::queue<std::shared_ptr<component_t>>* q) = 0;
};

struct node_t : public component_t
{
    uint16_t id;
    std::string name;

    std::vector<std::shared_ptr<node_t>> connected_nodes;
    std::shared_ptr<nand_t> connected_nand;

    bool dirty = true;
    bool active = false;
    bool locked = false;

    void Simulate(std::queue<std::shared_ptr<component_t>>* q) override
    {
        if (this->dirty)
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

            this->dirty = false;
        }
    }
};

struct nand_t : public component_t
{
    uint16_t id;
    std::string name;

    std::shared_ptr<node_t> inputA_node;
    std::shared_ptr<node_t> inputB_node;
    std::shared_ptr<node_t> output_node;

    bool dirty = true;

    void Simulate(std::queue<std::shared_ptr<component_t>>* q) override
    {
        if (this->dirty)
        {
            bool starting_output_state = this->output_node->active;
            bool next_output_state = !(this->inputA_node->active && this->inputB_node->active);
            
            this->output_node->active = next_output_state;
            this->output_node->dirty = true;

            q->push(this->output_node);
            
            this->dirty = false;
        }
    }
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
    }

    ~Simulation() = default;

    void Step()
    {
        auto start_time = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < 4; i++)
        {
            for (auto& node : nodes)
            {
                node->dirty = true;
            }
            
            for (auto& nand : nands)
            {
                nand->dirty = true;
            }
            
            q.push(GetNode("vcc"));
            q.push(GetNode("clk"));
            q.push(GetNode("input"));
            q.push(GetNode("input0"));
            q.push(GetNode("input1"));

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
};
