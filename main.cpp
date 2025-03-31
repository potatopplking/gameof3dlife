#include <iostream>

#include "utilities.hpp"
#include "voxel.hpp"
#include "simulation.hpp"
#include "ui.hpp"

// Voxel and GridSize are used both in Simulation and Window
// Simulation uses i,j,k coords - rows, cols, stacks
// window uses x,y,z,w (openGL coords)

int main(void)
{
    std::cout << "Press 'q' to quit\n";

    UI::Window window;
    window.Init();

    window.SetSimulation(std::make_unique<Simulation::GameOfLife2D>(20,20));

    window.Run();

    return 0;
}
