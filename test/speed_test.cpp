#include "simulation.h"
#include <chrono>
#include <iostream>

int main(int argc, char** argv)
{
    using namespace std::chrono_literals;
    using clock = std::chrono::high_resolution_clock;

    Simulation sim;

    auto start = clock::now();
    for (int i = 0; i < 1; ++i)
    {
        sim.Step();
    }
    auto end = clock::now();
    std::cout << "Frequency: " << 1'000.0f / std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "Hz";

    return 0;
}
