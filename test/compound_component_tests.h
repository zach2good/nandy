#pragma once

#include "simulation.h"
#include "gtest/gtest.h"

// https://en.wikipedia.org/wiki/NAND_logic

TEST(CompoundComponentTests, NOT)
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

TEST(CompoundComponentTests, AND)
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

TEST(CompoundComponentTests, OR)
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

TEST(CompoundComponentTests, NOR)
{
    Simulation sim;

    auto input0 = sim.GetNode("input0");
    auto input1 = sim.GetNode("input1");
    auto nand0 = sim.NewNAND();
    auto nand1 = sim.NewNAND();
    auto nand2 = sim.NewNAND();
    auto nand3 = sim.NewNAND();
    auto output = sim.GetNode("output");

    sim.ConnectNodes(input0, nand0->inputA_node);
    sim.ConnectNodes(input0, nand0->inputB_node);

    sim.ConnectNodes(input1, nand1->inputA_node);
    sim.ConnectNodes(input1, nand1->inputB_node);

    sim.ConnectNodes(nand0->output_node, nand2->inputA_node);
    sim.ConnectNodes(nand1->output_node, nand2->inputB_node);

    sim.ConnectNodes(nand2->output_node, nand3->inputA_node);
    sim.ConnectNodes(nand2->output_node, nand3->inputB_node);

    sim.ConnectNodes(nand3->output_node, output);

    sim.SetActive(input0, false);
    sim.SetActive(input1, false);
    sim.Step();
    EXPECT_TRUE(output->active);

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
    EXPECT_FALSE(output->active);
}

TEST(CompoundComponentTests, XOR)
{
    Simulation sim;

    auto input0 = sim.GetNode("input0");
    auto input1 = sim.GetNode("input1");
    auto nand0 = sim.NewNAND();
    auto nand1 = sim.NewNAND();
    auto nand2 = sim.NewNAND();
    auto nand3 = sim.NewNAND();
    auto output = sim.GetNode("output");

    sim.ConnectNodes(input0, nand0->inputA_node);
    sim.ConnectNodes(input0, nand1->inputA_node);

    sim.ConnectNodes(input1, nand0->inputB_node);
    sim.ConnectNodes(input1, nand2->inputB_node);

    sim.ConnectNodes(nand0->output_node, nand1->inputB_node);
    sim.ConnectNodes(nand1->output_node, nand2->inputA_node);

    sim.ConnectNodes(nand1->output_node, nand3->inputA_node);
    sim.ConnectNodes(nand2->output_node, nand3->inputB_node);

    sim.ConnectNodes(nand3->output_node, output);

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
    EXPECT_FALSE(output->active);
}

TEST(CompoundComponentTests, XNOR)
{
    Simulation sim;

    auto input0 = sim.GetNode("input0");
    auto input1 = sim.GetNode("input1");
    auto nand0 = sim.NewNAND();
    auto nand1 = sim.NewNAND();
    auto nand2 = sim.NewNAND();
    auto nand3 = sim.NewNAND();
    auto nand4 = sim.NewNAND();
    auto output = sim.GetNode("output");

    sim.ConnectNodes(input0, nand0->inputA_node);
    sim.ConnectNodes(input0, nand1->inputA_node);

    sim.ConnectNodes(input1, nand0->inputB_node);
    sim.ConnectNodes(input1, nand2->inputB_node);

    sim.ConnectNodes(nand0->output_node, nand1->inputB_node);
    sim.ConnectNodes(nand1->output_node, nand2->inputA_node);

    sim.ConnectNodes(nand1->output_node, nand3->inputA_node);
    sim.ConnectNodes(nand2->output_node, nand3->inputB_node);

    sim.ConnectNodes(nand3->output_node, nand4->inputA_node);
    sim.ConnectNodes(nand3->output_node, nand4->inputB_node);

    sim.ConnectNodes(nand4->output_node, output);

    sim.SetActive(input0, false);
    sim.SetActive(input1, false);
    sim.Step();
    EXPECT_TRUE(output->active);

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

TEST(CompoundComponentTests, 3_INPUT_NAND)
{
    Simulation sim;

    auto input0 = sim.GetNode("input0");
    auto input1 = sim.GetNode("input1");
    auto input2 = sim.GetNode("input2");
    auto nand0 = sim.NewNAND();
    auto nand1 = sim.NewNAND();
    auto nand2 = sim.NewNAND();
    auto output = sim.GetNode("output");

    sim.ConnectNodes(input0, nand0->inputA_node);
    sim.ConnectNodes(input1, nand0->inputB_node);

    sim.ConnectNodes(nand0->output_node, nand1->inputA_node);
    sim.ConnectNodes(nand0->output_node, nand1->inputB_node);

    sim.ConnectNodes(nand1->output_node, nand2->inputA_node);
    sim.ConnectNodes(input2, nand2->inputB_node);

    sim.ConnectNodes(nand2->output_node, output);

    sim.SetActive(input0, false);
    sim.SetActive(input1, false);
    sim.SetActive(input2, false);
    sim.Step();
    EXPECT_TRUE(output->active);

    sim.SetActive(input0, true);
    sim.SetActive(input1, false);
    sim.SetActive(input2, false);
    sim.Step();
    EXPECT_TRUE(output->active);

    sim.SetActive(input0, false);
    sim.SetActive(input1, true);
    sim.SetActive(input2, false);
    sim.Step();
    EXPECT_TRUE(output->active);

    sim.SetActive(input0, false);
    sim.SetActive(input1, false);
    sim.SetActive(input2, true);
    sim.Step();
    EXPECT_TRUE(output->active);

    sim.SetActive(input0, true);
    sim.SetActive(input1, true);
    sim.SetActive(input2, false);
    sim.Step();
    EXPECT_TRUE(output->active);

    sim.SetActive(input0, false);
    sim.SetActive(input1, true);
    sim.SetActive(input2, true);
    sim.Step();
    EXPECT_TRUE(output->active);

    sim.SetActive(input0, true);
    sim.SetActive(input1, false);
    sim.SetActive(input2, true);
    sim.Step();
    EXPECT_TRUE(output->active);

    sim.SetActive(input0, true);
    sim.SetActive(input1, true);
    sim.SetActive(input2, true);
    sim.Step();
    EXPECT_FALSE(output->active);
}

TEST(CompoundComponentTests, 3_INPUT_AND)
{
    Simulation sim;

    auto input0 = sim.GetNode("input0");
    auto input1 = sim.GetNode("input1");
    auto input2 = sim.GetNode("input2");
    auto nand0 = sim.NewNAND();
    auto nand1 = sim.NewNAND();
    auto nand2 = sim.NewNAND();
    auto nand3 = sim.NewNAND();
    auto output = sim.GetNode("output");

    sim.ConnectNodes(input0, nand0->inputA_node);
    sim.ConnectNodes(input1, nand0->inputB_node);

    sim.ConnectNodes(nand0->output_node, nand1->inputA_node);
    sim.ConnectNodes(nand0->output_node, nand1->inputB_node);

    sim.ConnectNodes(nand1->output_node, nand2->inputA_node);
    sim.ConnectNodes(input2, nand2->inputB_node);

    sim.ConnectNodes(nand2->output_node, nand3->inputA_node);
    sim.ConnectNodes(nand2->output_node, nand3->inputB_node);

    sim.ConnectNodes(nand3->output_node, output);

    sim.SetActive(input0, false);
    sim.SetActive(input1, false);
    sim.SetActive(input2, false);
    sim.Step();
    EXPECT_FALSE(output->active);

    sim.SetActive(input0, true);
    sim.SetActive(input1, false);
    sim.SetActive(input2, false);
    sim.Step();
    EXPECT_FALSE(output->active);

    sim.SetActive(input0, false);
    sim.SetActive(input1, true);
    sim.SetActive(input2, false);
    sim.Step();
    EXPECT_FALSE(output->active);

    sim.SetActive(input0, false);
    sim.SetActive(input1, false);
    sim.SetActive(input2, true);
    sim.Step();
    EXPECT_FALSE(output->active);

    sim.SetActive(input0, true);
    sim.SetActive(input1, true);
    sim.SetActive(input2, false);
    sim.Step();
    EXPECT_FALSE(output->active);

    sim.SetActive(input0, false);
    sim.SetActive(input1, true);
    sim.SetActive(input2, true);
    sim.Step();
    EXPECT_FALSE(output->active);

    sim.SetActive(input0, true);
    sim.SetActive(input1, false);
    sim.SetActive(input2, true);
    sim.Step();
    EXPECT_FALSE(output->active);

    sim.SetActive(input0, true);
    sim.SetActive(input1, true);
    sim.SetActive(input2, true);
    sim.Step();
    EXPECT_TRUE(output->active);
}

TEST(CompoundComponentTests, NAND_Latch)
{
    Simulation sim;

    auto s = sim.GetNode("input0");
    auto r = sim.GetNode("input1");
    auto nand0 = sim.NewNAND();
    auto nand1 = sim.NewNAND();
    auto q = sim.GetNode("output0");
    auto not_q = sim.GetNode("output1");

    sim.ConnectNodes(s, nand0->inputA_node);
    sim.ConnectNodes(r, nand1->inputB_node);

    sim.ConnectNodes(nand0->output_node, nand1->inputA_node);
    sim.ConnectNodes(nand1->output_node, nand0->inputB_node);

    sim.ConnectNodes(nand0->output_node, q);
    sim.ConnectNodes(nand1->output_node, not_q);

    // Invalid
    sim.SetActive(s, false);
    sim.SetActive(r, false);
    sim.Step();
    EXPECT_TRUE(q->active);
    EXPECT_TRUE(not_q->active);

    // Reset
    sim.SetActive(s, false);
    sim.SetActive(r, true);
    sim.Step();
    EXPECT_TRUE(q->active);
    EXPECT_FALSE(not_q->active);

    // Set
    sim.SetActive(s, true);
    sim.SetActive(r, false);
    sim.Step();
    EXPECT_FALSE(q->active);
    EXPECT_TRUE(not_q->active);

    // Memory (No change from last state: Set)
    sim.SetActive(s, true);
    sim.SetActive(r, true);
    sim.Step();
    EXPECT_FALSE(q->active);
    EXPECT_TRUE(not_q->active);

    // Reset
    sim.SetActive(s, false);
    sim.SetActive(r, true);
    sim.Step();
    EXPECT_TRUE(q->active);
    EXPECT_FALSE(not_q->active);

    // Memory (No change from last state: Reset)
    sim.SetActive(s, true);
    sim.SetActive(r, true);
    sim.Step();
    EXPECT_TRUE(q->active);
    EXPECT_FALSE(not_q->active);
}

TEST(CompoundComponentTests, MUX)
{
    Simulation sim;

    auto input0 = sim.GetNode("input0");
    auto input1 = sim.GetNode("input1");
    auto sel = sim.GetNode("input2");
    auto nand0 = sim.NewNAND();
    auto nand1 = sim.NewNAND();
    auto nand2 = sim.NewNAND();
    auto nand3 = sim.NewNAND();
    auto output = sim.GetNode("output");

    sim.ConnectNodes(sel, nand0->inputA_node);
    sim.ConnectNodes(sel, nand0->inputB_node);
    sim.ConnectNodes(sel, nand1->inputB_node);

    sim.ConnectNodes(input0, nand1->inputA_node);
    sim.ConnectNodes(input1, nand2->inputB_node);

    sim.ConnectNodes(nand0->output_node, nand2->inputA_node);

    sim.ConnectNodes(nand1->output_node, nand3->inputA_node);
    sim.ConnectNodes(nand2->output_node, nand3->inputB_node);

    sim.ConnectNodes(nand3->output_node, output);

    // Sel -> 0
    sim.SetActive(input0, false);
    sim.SetActive(input1, false);
    sim.SetActive(sel, false);
    sim.Step();
    EXPECT_FALSE(output->active);

    sim.SetActive(input0, true);
    sim.SetActive(input1, false);
    sim.SetActive(sel, false);
    sim.Step();
    EXPECT_FALSE(output->active);

    sim.SetActive(input0, false);
    sim.SetActive(input1, true);
    sim.SetActive(sel, false);
    sim.Step();
    EXPECT_TRUE(output->active);

    sim.SetActive(input0, true);
    sim.SetActive(input1, true);
    sim.SetActive(sel, false);
    sim.Step();
    EXPECT_TRUE(output->active);

    // Sel -> 1
    sim.SetActive(input0, false);
    sim.SetActive(input1, false);
    sim.SetActive(sel, true);
    sim.Step();
    EXPECT_FALSE(output->active);

    sim.SetActive(input0, true);
    sim.SetActive(input1, false);
    sim.SetActive(sel, true);
    sim.Step();
    EXPECT_TRUE(output->active);

    sim.SetActive(input0, false);
    sim.SetActive(input1, true);
    sim.SetActive(sel, true);
    sim.Step();
    EXPECT_FALSE(output->active);

    sim.SetActive(input0, true);
    sim.SetActive(input1, true);
    sim.SetActive(sel, true);
    sim.Step();
    EXPECT_TRUE(output->active);
}

TEST(CompoundComponentTests, DEMUX)
{
    Simulation sim;

    auto input = sim.GetNode("input0");
    auto sel = sim.GetNode("input1");
    auto nand0 = sim.NewNAND();
    auto nand1 = sim.NewNAND();
    auto nand2 = sim.NewNAND();
    auto nand3 = sim.NewNAND();
    auto nand4 = sim.NewNAND();
    auto a = sim.GetNode("output0");
    auto b = sim.GetNode("output1");

    sim.ConnectNodes(input, nand1->inputA_node);
    sim.ConnectNodes(input, nand2->inputB_node);

    sim.ConnectNodes(sel, nand0->inputA_node);
    sim.ConnectNodes(sel, nand0->inputB_node);
    sim.ConnectNodes(sel, nand2->inputB_node);

    sim.ConnectNodes(nand0->output_node, nand1->inputB_node);

    sim.ConnectNodes(nand1->output_node, nand3->inputA_node);
    sim.ConnectNodes(nand2->output_node, nand3->inputB_node);

    sim.ConnectNodes(nand2->output_node, nand4->inputA_node);
    sim.ConnectNodes(nand2->output_node, nand4->inputB_node);

    sim.ConnectNodes(nand3->output_node, a);
    sim.ConnectNodes(nand4->output_node, b);

    sim.SetActive(input, false);
    sim.SetActive(sel, false);
    sim.Step();
    EXPECT_FALSE(a->active);
    EXPECT_FALSE(b->active);

    sim.SetActive(input, true);
    sim.SetActive(sel, false);
    sim.Step();
    EXPECT_TRUE(a->active);
    EXPECT_FALSE(b->active);

    sim.SetActive(input, false);
    sim.SetActive(sel, true);
    sim.Step();
    EXPECT_FALSE(a->active);
    EXPECT_FALSE(b->active);

    sim.SetActive(input, true);
    sim.SetActive(sel, true);
    sim.Step();
    EXPECT_FALSE(a->active);
    EXPECT_TRUE(b->active);
}
