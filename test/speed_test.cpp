#include "simulation.h"
#include <chrono>
#include <iostream>

int main(int argc, char** argv)
{
    using namespace std::chrono_literals;
    using clock = std::chrono::high_resolution_clock;

    std::cout << "Speed test; launching Sim...\n";
    Simulation sim;
    auto* clk = sim.nodes[0];
    auto* last_node = sim.nodes[sim.nodes.size() - 1];
    auto* n0 = sim.nodes[sim.nodes.size() - 4];
    auto* n1 = sim.nodes[sim.nodes.size() - 7];
    auto* n2 = sim.nodes[sim.nodes.size() - 10];

    std::size_t steps = 4;
    auto start = clock::now();
    for (int i = 0; i < steps; ++i)
    {
        sim.Step();
        std::cout << clk->active << " ..., " << n2->active << ", " << n1->active << ", " << n0->active << ", " << last_node->active << "\n";
    }
    auto end = clock::now();
    std::cout << "Step Count: " << sim.step_count << "\n";
    auto period = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::cout << "NANDs: " << sim.nands.size() << "\n";
    std::cout << "Nodes: " << sim.nodes.size() << "\n";
    std::cout << "Time Taken (us): " << period << "\n";
    std::cout << "Time Taken (per step) (us): " << period / (long long)steps << "\n";
    std::cout << "Frequency (Hz): " << (1.0f / ((float)period / (float)steps)) * 1'000'000.0f << "\n";
    std::cout << "Queue Ops: " << sim.queue_ops << "\n";

    return 0;
}
