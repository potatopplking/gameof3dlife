#include <iostream>

#include "utilities.hpp"
#include "voxel.hpp"
#include "simulations/game_of_life_2D.hpp"
#include "simulations/game_of_life_3D.hpp"
#include "ui.hpp"

// Voxel and GridSize are used both in Simulation and Window
// Simulation uses i,j,k coords - rows, cols, stacks
// window uses x,y,z,w (openGL coords)

int main(void)
{
    Log::info("Press 'q' to quit");

    UI::Window window{ 800, 600 };
    window.Init();

    window.SetSimulation(std::make_unique<Simulation::GameOfLife3D>(30,30,30));

    window.Run();

    return 0;
}
