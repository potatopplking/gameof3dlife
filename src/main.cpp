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

    utils::NewCSVec<utils::CoordinateSystem::CARTESIAN, double, 3> lol{1,2,3};

    return 0;

    UI::Window window{ 800, 600 };
    window.Init();

    window.SetSimulation(std::make_unique<Simulation::GameOfLife2D>(20,20));

    window.Run();

    return 0;
}
