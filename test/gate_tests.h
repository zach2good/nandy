#pragma once

#include "definitions.h"
#include "simulation.h"

TEST(gate_tests, NAND_table)
{
    Simulation sim;

    auto nand = sim.NewNAND(0, 0);

    nand->inputA_node->active = false;
    nand->inputB_node->active = false;
    nand->output_node->active = false;

    sim.Step();

    EXPECT_TRUE(nand->output_node->active);

    nand->inputA_node->active = true;
    nand->inputB_node->active = false;
    nand->output_node->active = false;

    sim.Step();

    EXPECT_TRUE(nand->output_node->active);

    nand->inputA_node->active = false;
    nand->inputB_node->active = true;
    nand->output_node->active = false;

    sim.Step();

    EXPECT_TRUE(nand->output_node->active);

    nand->inputA_node->active = true;
    nand->inputB_node->active = true;
    nand->output_node->active = true;

    sim.Step();

    EXPECT_FALSE(nand->output_node->active);
}

TEST(gate_tests, NOT_table)
{
    Simulation sim;

    auto input = sim.NewNode(0, 0);
    auto nand = sim.NewNAND(0, 0);

    sim.ConnectNodes(input, nand->inputA_node);
    sim.ConnectNodes(input, nand->inputB_node);

    input->active = false;
    nand->output_node->active = false;

    sim.Step();

    EXPECT_TRUE(nand->output_node->active);

    input->active = true;
    nand->output_node->active = true;

    sim.Step();

    EXPECT_FALSE(nand->output_node->active);
}

TEST(gate_tests, AND_table)
{
    Simulation sim;

    auto nand0 = sim.NewNAND(0, 0);
    auto nand1 = sim.NewNAND(0, 0);

    sim.ConnectNodes(nand0->output_node, nand1->inputA_node);
    sim.ConnectNodes(nand0->output_node, nand1->inputB_node);

    nand0->inputA_node->active = false;
    nand0->inputB_node->active = false;
    nand0->output_node->active = true;

    sim.Step();

    EXPECT_FALSE(nand1->output_node->active);

    nand0->inputA_node->active = true;
    nand0->inputB_node->active = false;
    nand0->output_node->active = true;

    sim.Step();

    EXPECT_FALSE(nand1->output_node->active);

    nand0->inputA_node->active = false;
    nand0->inputB_node->active = true;
    nand0->output_node->active = true;

    sim.Step();

    EXPECT_FALSE(nand1->output_node->active);

    nand0->inputA_node->active = true;
    nand0->inputB_node->active = true;
    nand0->output_node->active = false;

    sim.Step();

    EXPECT_TRUE(nand1->output_node->active);
}