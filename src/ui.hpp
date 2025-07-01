#pragma once

#include <memory>

#include <SDL3/SDL.h>

#include "simulations/base.hpp"
#include "utilities.hpp"
#include "voxel.hpp"

namespace UI
{

// Helper functions for rendering, these should ideally be replaced with some shader
void draw_cube(utils::SimCoords pos = {0,0,0});
void enable_light();


// mouse input in, camera position out
class Camera
{
    using MousePos = utils::Vec<int, 2>;
    using CS = utils::CoordinateSystem;

public:
    Camera() : 
        aspect{ 0 }
    {}
    ~Camera() = default;

    static constexpr double CAMERA_ZOOM_FACTOR = 1.0;

    void SetPan(MousePos diff);
    void SetZoom(float scroll_diff);
    void SetRotation(MousePos diff);
    void LookAt(utils::CSVec<CS::CARTESIAN, double, 3> pos, double from_distance);

    void SetPerspectiveProjection();
    void TranslateRotateScene();

    double aspect;
    utils::CSVec<CS::CARTESIAN, double, 3> lookAt; // TODO a bit confusing, lookAt always has to be zero, we use offset instead
    utils::CSVec<CS::CARTESIAN, double, 3> up;
    utils::CSVec<CS::CARTESIAN, double, 3> offset;
    utils::CSVec<CS::SPHERICAL, double, 3> pos; // TODO move to private

private:

    // TODO normalize
    template <utils::CoordinateSystem CS>
    std::array<utils::CSVec<CS, double, 3>,2> GetProjectionPlaneBasis() {
        auto view_vector = this->lookAt - this->pos;
        auto imagePlaneX = utils::CrossProduct(view_vector, this->up);
        auto imagePlaneY = utils::CrossProduct(imagePlaneX, this->up);
        std::array<utils::CSVec<CS, double, 3>,2> lol{imagePlaneX, imagePlaneY};
        return lol;
    }

};

class Window {
    public:
        // SDL and OpenGL attributes
        SDL_Renderer* renderer;
        SDL_Window* window;
        SDL_GLContext context;
        // Simulation
        std::unique_ptr<Simulation::BaseSimulation> sim;
        utils::Vec<int, 2> size;
        utils::Vec<int, 2> mouse_prev_pos;
        Camera camera;
        bool simulation_paused = false;
        double real_time_elapsed = 0.0; // Tracks time passed in the real world

        Window();
        Window(int width, int height);
        ~Window();

        int Init();
        void Run();
        void SetSimulation(std::unique_ptr<Simulation::BaseSimulation> new_sim);
        void Resize(int width, int height);
        void Resize(utils::Vec<int, 2> new_size);
        void ResetSimulation(); // Added declaration for SimulationReset
        void PrintTimeStats();
        double GetUptime();

    private:
        bool exit_requested = false;
        bool wireframe_enabled = false;
        bool alpha_enabled = false;
        // time measurement
        utils::TimeStats render_time;
        utils::TimeStats sim_time;
        utils::TimeStats total_time;

        void ProcessEvents();
        bool ExitRequested();
        void ClearWindow(utils::Color c);
        void UpdateSimulation(bool force);
        void Render(const std::vector<Voxel, utils::TrackingAllocator<Voxel>>& voxels);
        void DrawAxis();
        void Flush();
        void Resize();
        void UpdateTime();
};

}
