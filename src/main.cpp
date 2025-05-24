#include <iostream>

#include "utilities.hpp"
#include "voxel.hpp"
#include "simulations/game_of_life_3D.hpp"
#include "ui.hpp"

int main(void)
{
    Log::info("Press 'q' to quit");

    UI::Window window{800, 600};
    window.Init();

    window.SetSimulation(std::make_unique<Simulation::GameOfLife3D>(50, 50, 50));

    window.Run();

    return 0;
}
