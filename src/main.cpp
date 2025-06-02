#include <iostream>
#include <memory>

#include "utilities.hpp"
#include "voxel.hpp"
#include "ui.hpp"
#include "simulations/game_of_life_3D.hpp"
#include "simulations/recorder.hpp"
#include "simulations/playback.hpp"

int main(void)
{
    Log::info("Press 'q' to quit");

    UI::Window window{800, 600};
    window.Init();

#if 1
    window.SetSimulation(
        std::make_unique<Simulation::Playback>("gol3d.sim")
    );
#else
    window.SetSimulation( 
        std::make_unique<Simulation::Recorder>(
            std::make_unique<Simulation::GameOfLife3D>(50, 50, 50),
            std::string("gol3d.sim")
        )
    );
#endif

    window.Run();

    return 0;
}
