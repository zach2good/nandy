#pragma once

#include "definitions.h"
#include "simulation.h"

TEST(gate_tests, NAND_table)
{ 
    Simulation sim;
    sim.LoadFromJSONString(R"json(
        {
            "nands": [
                {
                    "id": 0,
                    "inputa_id": 0,
                    "inputb_id": 1,
                    "is_nand": true,
                    "output_id": 2,
                    "x": 200,
                    "y": 200
                }
            ],
            "node_lookup": {
                "c0": 3,
                "c1": 4,
                "c2": 5
            },
            "nodes": [
                {
                    "active": true,
                    "attached_nand": true,
                    "driving_ids": [],
                    "id": 0,
                    "is_nand": false,
                    "nand_id": 0,
                    "x": 200,
                    "y": 219
                },
                {
                    "active": false,
                    "attached_nand": true,
                    "driving_ids": [],
                    "id": 1,
                    "is_nand": false,
                    "nand_id": 0,
                    "x": 200,
                    "y": 244
                },
                {
                    "active": true,
                    "attached_nand": true,
                    "driving_ids": [
                        5
                    ],
                    "id": 2,
                    "is_nand": false,
                    "nand_id": 0,
                    "x": 264,
                    "y": 232
                },
                {
                    "active": true,
                    "attached_nand": false,
                    "driving_ids": [
                        0
                    ],
                    "id": 3,
                    "is_nand": false,
                    "nand_id": 0,
                    "x": 136,
                    "y": 217
                },
                {
                    "active": false,
                    "attached_nand": false,
                    "driving_ids": [
                        1
                    ],
                    "id": 4,
                    "is_nand": false,
                    "nand_id": 0,
                    "x": 136,
                    "y": 244
                },
                {
                    "active": true,
                    "attached_nand": false,
                    "driving_ids": [],
                    "id": 5,
                    "is_nand": false,
                    "nand_id": 0,
                    "x": 325,
                    "y": 231
                }
            ]
        }
    )json");
    
    auto c0 = sim.LookupNode("c0");
    auto c1 = sim.LookupNode("c1");
    auto c2 = sim.LookupNode("c2");
    
    sim.Step();

    c0->active = false;
    c1->active = false;
    
    sim.Step();

    EXPECT_TRUE(c2->active);
}
