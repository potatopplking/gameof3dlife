#pragma once

#include <memory>

#include <SDL2/SDL.h>

#include "simulation.hpp"
#include "utilities.hpp"
#include "voxel.hpp"

namespace UI
{



void draw_cube(utils::SimCoords pos = {0,0,0});

void enable_light();

class Window
{
    public:
        static constexpr double CAMERA_ZOOM_FACTOR = 1.0;
        // SDL and OpenGL attributes
        SDL_Renderer* renderer;
        SDL_Window* window;
        SDL_GLContext context;
        // Simulation
        std::unique_ptr<Simulation::BaseSimulation> sim;
        utils::Vec<int, 2> size;
        utils::Vec<int, 2> mouse_position, mouse_init_position;
        utils::SphericCoords camera_pos{.0, .0, 5.0};

        Window();
        ~Window();

        int Init();
        void Run();
        void SetSimulation(std::unique_ptr<Simulation::BaseSimulation> new_sim);
        void Resize(int width, int height);
        void Resize(utils::Vec<int, 2> new_size);

    private:
        bool exit_requested = false;

        void ProcessEvents();
        bool ExitRequested();
        void ClearWindow(utils::Color c);
        void UpdateSimulation();
        void Render(const std::vector<Voxel>& voxels);
        void Flush();
        void Resize();
};

}
