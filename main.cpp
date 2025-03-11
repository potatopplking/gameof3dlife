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
    window.sim = std::make_unique<Simulation::GameOfLife2D>(20,20);

    std::cout << "grid size: " << window.sim->GetGridSize() << std::endl;
    std::cout << "voxel color : " << window.sim->GetVoxels()[0].color[1] << std::endl;

    while (!window.ExitRequested()) {
        window.ProcessEvents();
        window.ClearWindow(utils::Color{100,0,0,255});
        window.Update();
    }

    return 0;
}
