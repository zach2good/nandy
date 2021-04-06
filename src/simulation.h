#pragma once

#include <chrono>
#include <fstream>
#include <memory>
#include <queue>
#include <unordered_map>
#include <vector>

// TODO: Fix includes
#include "../ext/json/nlohmann/json.hpp"

#include "definitions.h"

class Simulation
{
public:
    Simulation();
    ~Simulation() = default;

    void Step();

    std::shared_ptr<nand_t> NewNAND(int x = 0, int y = 0, const std::string& name = "");
    std::shared_ptr<node_t> NewNode(int x = 0, int y = 0, const std::string& name = "");

    std::shared_ptr<nand_t> GetNAND(uint32_t id)
    {
        return nands[id];
    }

    std::shared_ptr<node_t> GetNode(uint32_t id)
    {
        return nodes[id];
    }

    std::shared_ptr<nand_t> LookupNAND(const std::string& name)
    {
        return nand_lookup.find(name) != nand_lookup.end() ? nand_lookup[name] : nullptr;
    }

    std::shared_ptr<node_t> LookupNode(const std::string& name)
    {
        return node_lookup.find(name) != node_lookup.end() ? node_lookup[name] : nullptr;
    }

    void ConnectNodes(const uint32_t id0, const uint32_t id1)
    {
        printf("Connecting: %d -> %d\n", id0, id1);
        auto& n0 = nodes[id0];
        n0->driving_ids.emplace_back(id1);
    };

    nlohmann::json DumpToJSONObject()
    {
        nlohmann::json j;
        for (auto& nand : nands)
        {
            j["nands"][nand->id]["id"] = nand->id;
            j["nands"][nand->id]["x"] = nand->x;
            j["nands"][nand->id]["y"] = nand->y;
            j["nands"][nand->id]["is_nand"] = nand->is_nand;

            j["nands"][nand->id]["inputa_id"] = nand->inputa_id;
            j["nands"][nand->id]["inputb_id"] = nand->inputb_id;
            j["nands"][nand->id]["output_id"] = nand->output_id;
        }

        for (auto& node : nodes)
        {
            j["nodes"][node->id]["id"] = node->id;
            j["nodes"][node->id]["x"] = node->x;
            j["nodes"][node->id]["y"] = node->y;
            j["nodes"][node->id]["is_nand"] = node->is_nand;

            j["nodes"][node->id]["active"] = node->active;
            j["nodes"][node->id]["driving_ids"] = node->driving_ids;

            j["nodes"][node->id]["attached_nand"] = node->attached_nand;
            j["nodes"][node->id]["nand_id"] = node->nand_id;
        }

        for (auto& entry : nand_lookup)
        {
            auto str = entry.first;
            auto ptr = entry.second;
            j["nand_lookup"][str] = ptr->id;
        }

        for (auto& entry : node_lookup)
        {
            auto str = entry.first;
            auto ptr = entry.second;
            j["node_lookup"][str] = ptr->id;
        }

        return j;
    }

    void DumpJSONObjectToFile(const nlohmann::json& j, const std::string& filename)
    {
        std::ofstream o(filename);
        o << j.dump();
    }

    void LoadFromJSONObject(const nlohmann::json& j)
    {
        using nlohmann::json;

        nands.clear();
        nodes.clear();
        nand_lookup.clear();
        node_lookup.clear();

        if (j.find("nands") != j.end())
        {
            for (auto& entry : j["nands"])
            {
                auto nand = std::make_shared<nand_t>();
                nand->id = entry["id"];
                nand->x = entry["x"];
                nand->y = entry["y"];
                nand->is_nand = entry["is_nand"];

                nand->inputa_id = entry["inputa_id"];
                nand->inputb_id = entry["inputb_id"];
                nand->output_id = entry["output_id"];

                nands.emplace_back(nand);
            }
        }

        if (j.find("nodes") != j.end())
        {
            for (auto& entry : j["nodes"])
            {
                auto node = std::make_shared<node_t>();
                node->id = entry["id"];
                node->x = entry["x"];
                node->y = entry["y"];
                node->is_nand = entry["is_nand"];

                node->active = entry["active"];
                node->driving_ids = entry["driving_ids"].get<std::vector<uint32_t>>();

                node->attached_nand = entry["attached_nand"];
                node->nand_id = entry["nand_id"];

                nodes.emplace_back(node);
            }
        }

        if (j.find("nand_lookup") != j.end())
        {
            for (auto& entry : j["nand_lookup"].items())
            {
                auto str = entry.key();
                auto id = entry.value();
                nand_lookup[str] = GetNAND(id);
            }
        }

        if (j.find("node_lookup") != j.end())
        {
            for (auto& entry : j["node_lookup"].items())
            {
                auto str = entry.key();
                auto id = entry.value();
                node_lookup[str] = GetNode(id);
            }
        }
    }

    void LoadFromJSONFile(const std::string& filename)
    {
        std::ifstream i(filename);
        nlohmann::json j;
        i >> j;
        LoadFromJSONObject(j);
    }

    void LoadFromJSONString(const std::string& str)
    {
        nlohmann::json j;
        j.parse(str);
        LoadFromJSONObject(j);
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
