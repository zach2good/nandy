#pragma once

#include "simulation.h"
#include "gtest/gtest.h"


TEST(CompoundComponentTests, NOTTest)
{
    Simulation sim;

    auto input = sim.GetNode("input");
    auto nand = sim.NewNAND();
    auto output = sim.GetNode("output");

    sim.ConnectNodes(input, nand->inputA_node);
    sim.ConnectNodes(input, nand->inputB_node);
    sim.ConnectNodes(nand->output_node, output);

    sim.SetActive(input, true);
    sim.Step();
    EXPECT_NE(input->active, output->active);

    sim.SetActive(input, true);
    sim.Step();
    EXPECT_NE(input->active, output->active);

    sim.SetActive(input, true);
    sim.Step();
    EXPECT_NE(input->active, output->active);
}

TEST(CompoundComponentTests, ANDTest)
{
    Simulation sim;

    auto input0 = sim.GetNode("input0");
    auto input1 = sim.GetNode("input1");
    auto nand0 = sim.NewNAND();
    auto nand1 = sim.NewNAND();
    auto output = sim.GetNode("output");

    sim.ConnectNodes(input0, nand0->inputA_node);
    sim.ConnectNodes(input1, nand0->inputB_node);

    sim.ConnectNodes(nand0->output_node, nand1->inputA_node);
    sim.ConnectNodes(nand0->output_node, nand1->inputB_node);

    sim.ConnectNodes(nand1->output_node, output);

    sim.SetActive(input0, false);
    sim.SetActive(input1, false);
    sim.Step();
    EXPECT_FALSE(output->active);

    sim.SetActive(input0, true);
    sim.SetActive(input1, false);
    sim.Step();
    EXPECT_FALSE(output->active);

    sim.SetActive(input0, false);
    sim.SetActive(input1, true);
    sim.Step();
    EXPECT_FALSE(output->active);

    sim.SetActive(input0, true);
    sim.SetActive(input1, true);
    sim.Step();
    EXPECT_TRUE(output->active);
}

TEST(CompoundComponentTests, ORTest)
{
    Simulation sim;

    auto input0 = sim.GetNode("input0");
    auto input1 = sim.GetNode("input1");
    auto nand0 = sim.NewNAND();
    auto nand1 = sim.NewNAND();
    auto nand2 = sim.NewNAND();
    auto output = sim.GetNode("output");

    sim.ConnectNodes(input0, nand0->inputA_node);
    sim.ConnectNodes(input0, nand0->inputB_node);

    sim.ConnectNodes(input1, nand1->inputA_node);
    sim.ConnectNodes(input1, nand1->inputB_node);

    sim.ConnectNodes(nand0->output_node, nand2->inputA_node);
    sim.ConnectNodes(nand1->output_node, nand2->inputB_node);

    sim.ConnectNodes(nand2->output_node, output);

    sim.SetActive(input0, false);
    sim.SetActive(input1, false);
    sim.Step();
    EXPECT_FALSE(output->active);

    sim.SetActive(input0, true);
    sim.SetActive(input1, false);
    sim.Step();
    EXPECT_TRUE(output->active);

    sim.SetActive(input0, false);
    sim.SetActive(input1, true);
    sim.Step();
    EXPECT_TRUE(output->active);

    sim.SetActive(input0, true);
    sim.SetActive(input1, true);
    sim.Step();
    EXPECT_TRUE(output->active);
}
