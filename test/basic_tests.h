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

TEST(BasicTests, SingleTransistor)
{
    Simulation sim;

    auto vcc = sim.GetNode("vcc");
    auto gnd = sim.GetNode("gnd");
    auto input = sim.GetNode("input");
    auto output = sim.GetNode("output");

    auto t = sim.NewTransistor();

    EXPECT_TRUE(t != nullptr);

    sim.ConnectNodes(vcc, t->input_node);
    sim.ConnectNodes(input, t->enable_node);
    sim.ConnectNodes(t->output_node, output);

    input->active = false;
    sim.Step();

    EXPECT_FALSE(output->active);

    input->active = true;
    sim.Step();

    EXPECT_TRUE(output->active);
}

TEST(BasicTests, ANDGate)
{
    Simulation sim;

    auto vcc = sim.GetNode("vcc");
    auto gnd = sim.GetNode("gnd");
    auto input0 = sim.GetNode("input0");
    auto input1 = sim.GetNode("input1");
    auto output = sim.GetNode("output");

    auto t0 = sim.NewTransistor();
    auto t1 = sim.NewTransistor();

    EXPECT_TRUE(t0 != nullptr);
    EXPECT_TRUE(t1 != nullptr);

    sim.ConnectNodes(vcc, t0->input_node);

    sim.ConnectNodes(input0, t0->enable_node);
    sim.ConnectNodes(input1, t1->enable_node);

    sim.ConnectNodes(t0->output_node, t1->input_node);

    sim.ConnectNodes(t1->output_node, output);

    sim.Step();

    EXPECT_FALSE(output->active);

    input0->active = true;
    sim.Step();

    EXPECT_FALSE(output->active);

    input1->active = true;
    sim.Step();

    EXPECT_TRUE(output->active);

    input0->active = false;
    sim.Step();

    EXPECT_FALSE(output->active);
}
