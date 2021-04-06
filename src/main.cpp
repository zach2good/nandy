#include "simulation.h"
#include "window.h"

int main(int argc, char* argv[])
{
    Window window = Window(1280, 720);
    Simulation sim;

    while (!window.QuitRequested())
    {
        window.HandleEvents();
        window.Frame_Draw(sim);
    }

    return 0;
}
