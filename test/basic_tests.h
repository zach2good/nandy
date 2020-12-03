#pragma once

#include "simulation.h"
#include "gtest/gtest.h"


TEST(BasicTests, Construction)
{
    EXPECT_NO_THROW(Simulation sim);
}

TEST(BasicTests, CheckConstants)
{
    Simulation sim;

    auto vcc = sim.GetNode("vcc");
    auto gnd = sim.GetNode("gnd");
    auto input = sim.GetNode("input");
    auto output = sim.GetNode("output");

    EXPECT_TRUE(vcc != nullptr);
    EXPECT_TRUE(vcc->active);
    EXPECT_TRUE(vcc->locked);

    EXPECT_TRUE(gnd != nullptr);
    EXPECT_FALSE(gnd->active);
    EXPECT_TRUE(gnd->locked);

    EXPECT_TRUE(input != nullptr);
    EXPECT_TRUE(input->locked);

    EXPECT_TRUE(output != nullptr);
}

TEST(BasicTests, BasicNode)
{
    Simulation sim;

    auto vcc = sim.GetNode("vcc");
    auto output = sim.GetNode("output");

    sim.ConnectNodes(vcc, output);
    
    sim.Step();

    EXPECT_TRUE(output->active);
}

TEST(BasicTests, SequentialNodes)
{
    Simulation sim;

    auto vcc = sim.GetNode("vcc");
    auto n0 = sim.NewNode("n0");
    auto n1 = sim.NewNode("n1");
    auto output = sim.GetNode("output");

    sim.ConnectNodes(vcc, n0);
    sim.ConnectNodes(n0, n1);
    sim.ConnectNodes(n1, output);

    sim.Step();

    EXPECT_TRUE(output->active);
}

TEST(BasicTests, ParallelNodes)
{
    Simulation sim;

    auto vcc = sim.GetNode("vcc");
    
    auto n0 = sim.NewNode("n0");
    auto n1 = sim.NewNode("n1");
    auto n2 = sim.NewNode("n2");

    auto n3 = sim.NewNode("n3");
    auto n4 = sim.NewNode("n4");
    auto n5 = sim.NewNode("n5");

    sim.ConnectNodes(vcc, n0);
    sim.ConnectNodes(vcc, n1);
    sim.ConnectNodes(vcc, n2);

    sim.ConnectNodes(n0, n3);
    sim.ConnectNodes(n1, n4);
    sim.ConnectNodes(n2, n5);

    sim.Step();

    EXPECT_TRUE(n3->active);
    EXPECT_TRUE(n4->active);
    EXPECT_TRUE(n5->active);
}

TEST(BasicTests, BasicNAND_High_High)
{
    Simulation sim;

    auto input0 = sim.GetNode("input0");
    auto input1 = sim.GetNode("input1");
    auto nand = sim.NewNAND("nand");
    auto output = sim.GetNode("output");

    sim.ConnectNodes(input0, nand->inputA_node);
    sim.ConnectNodes(input1, nand->inputB_node);
    sim.ConnectNodes(nand->output_node, output);

    // High + High = Low
    sim.SetActive(input0, true);
    sim.SetActive(input1, true);

    sim.Step();

    EXPECT_FALSE(output->active);
}

TEST(BasicTests, BasicNAND_Low_Low)
{
    Simulation sim;

    auto input0 = sim.GetNode("input0");
    auto input1 = sim.GetNode("input1");
    auto nand = sim.NewNAND("nand");
    auto output = sim.GetNode("output");

    sim.ConnectNodes(input0, nand->inputA_node);
    sim.ConnectNodes(input1, nand->inputB_node);
    sim.ConnectNodes(nand->output_node, output);

    // Low + Low = High
    sim.SetActive(input0, false);
    sim.SetActive(input1, false);

    sim.Step();

    EXPECT_TRUE(output->active);
}

TEST(BasicTests, BasicNAND_Low_High)
{
    Simulation sim;

    auto input0 = sim.GetNode("input0");
    auto input1 = sim.GetNode("input1");
    auto nand = sim.NewNAND("nand");
    auto output = sim.GetNode("output");

    sim.ConnectNodes(input0, nand->inputA_node);
    sim.ConnectNodes(input1, nand->inputB_node);
    sim.ConnectNodes(nand->output_node, output);

    // Low + High = High
    sim.SetActive(input0, false);
    sim.SetActive(input1, true);

    sim.Step();

    EXPECT_TRUE(output->active);
}

TEST(BasicTests, BasicNAND_High_Low)
{
    Simulation sim;

    auto input0 = sim.GetNode("input0");
    auto input1 = sim.GetNode("input1");
    auto nand = sim.NewNAND("nand");
    auto output = sim.GetNode("output");

    sim.ConnectNodes(input0, nand->inputA_node);
    sim.ConnectNodes(input1, nand->inputB_node);
    sim.ConnectNodes(nand->output_node, output);

    // High + Low = High
    sim.SetActive(input0, true);
    sim.SetActive(input1, false);

    sim.Step();

    EXPECT_TRUE(output->active);
}

TEST(BasicTests, ChainedNANDs)
{
    Simulation sim;

    auto vcc = sim.GetNode("vcc");
    auto gnd = sim.GetNode("gnd");
    auto nand0 = sim.NewNAND();
    auto nand1 = sim.NewNAND();
    auto nand2 = sim.NewNAND();
    auto output = sim.GetNode("output");

    // High + High = Low
    sim.ConnectNodes(vcc, nand0->inputA_node);
    sim.ConnectNodes(vcc, nand0->inputB_node);

    // Low + Low = High
    sim.ConnectNodes(gnd, nand1->inputA_node);
    sim.ConnectNodes(gnd, nand1->inputB_node);

    // Low + High = High
    sim.ConnectNodes(nand0->output_node, nand2->inputA_node);
    sim.ConnectNodes(nand1->output_node, nand2->inputB_node);

    sim.ConnectNodes(nand2->output_node, output);

    sim.Step();

    EXPECT_FALSE(nand0->output_node->active);
    EXPECT_TRUE(nand1->output_node->active);
    EXPECT_TRUE(output->active);
}
