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

    uint16_t id;
    std::string name;
    std::shared_ptr<component_t> self;

    bool dirty = true;
    bool active = false;
    bool locked = false;
    bool drawable = false;
    bool probe = false;

    int x = 0;
    int y = 0;
};

struct node_t : public component_t
{
    std::vector<std::shared_ptr<node_t>> driving;
    std::shared_ptr<nand_t> connected_nand;

    void Simulate(std::queue<std::shared_ptr<component_t>>* q) override;
};

struct nand_t : public component_t
{
    std::shared_ptr<node_t> inputA_node;
    std::shared_ptr<node_t> inputB_node;
    std::shared_ptr<node_t> output_node;

    void Simulate(std::queue<std::shared_ptr<component_t>>* q) override;
};
