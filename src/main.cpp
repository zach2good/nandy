#include "window.h"
#include <chrono>

int main(int argc, char* argv[])
{
    using namespace std::chrono_literals;
    constexpr std::chrono::nanoseconds time_step(30ms);
    using clock = std::chrono::high_resolution_clock;

    Window window(1280, 720);

    while (!window.QuitRequested())
    {
        auto time_now = clock::now();
        auto next_step = time_now + time_step;

        while (time_now < next_step)
        {
            window.HandleEvents();
            window.Sim();

            time_now = clock::now();
        }
        
        window.Draw();
    }

    return 0;
}
