#pragma once

#include <chrono>
#include <memory>
#include <queue>
#include <unordered_map>
#include <vector>
#include <cstdint>

struct component_t
{
    uint32_t id = 0;
    uint32_t x = 0;
    uint32_t y = 0;
    bool is_nand = false;
};

struct nand_t : public component_t
{
    uint32_t inputa_id;
    uint32_t inputb_id;
    uint32_t output_id;
};

struct node_t : public component_t
{
    bool active = false;
    std::vector<uint32_t> driving_ids;
    bool attached_nand = false;
    uint32_t nand_id;
};

struct label_t : public component_t
{
    std::string text;
};