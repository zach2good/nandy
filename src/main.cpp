#include "simulation.h"
#include "window.h"

int main(int argc, char* argv[])
{
    Window window;
    Simulation sim;

    while (!window.QuitRequested())
    {
        window.HandleEvents();
        window.Draw(sim);
    }

    return 0;
}
