#pragma once

#include <memory>

#include <SDL3/SDL.h>

#include "simulation.hpp"
#include "utilities.hpp"
#include "voxel.hpp"

namespace UI
{



void draw_cube(utils::SimCoords pos = {0,0,0});

void enable_light();


// mouse input in, camera position out
class Camera
{
    using MousePos = utils::Vec<int, 2>;
    using CS = utils::CoordinateSystem;

public:
    Camera() : 
        up{     0.0, 1.0, 0.0 },
        pos{    0.0, 0.0, 0.0 },
        lookAt{ 0.0, 0.0, 0.0 },
        aspect{ 0 }
    {}
    ~Camera() = default;

    static constexpr double CAMERA_ZOOM_FACTOR = 1.0;

    void SetPan(MousePos diff);
    void SetZoom(float scroll_diff);
    void SetRotation(MousePos diff);

    void SetPerspectiveProjection();
    void TranslateRotateScene();

    double aspect;
    utils::CSVec<CS::SPHERICAL, double, 3> pos; // TODO move to private

private:

    template <utils::CoordinateSystem CS>
    std::array<utils::CSVec<CS, double, 3>,2> GetImagePlaneBasis() {
        auto view_vector = this->lookAt - this->pos;
        auto imagePlaneX = utils::CrossProduct(view_vector, this->up);
        auto imagePlaneY = utils::CrossProduct(imagePlaneX, this->up);
        std::array<utils::CSVec<CS, double, 3>,2> lol{imagePlaneX, imagePlaneY};
        return lol;
    }

    utils::CSVec<CS::CARTESIAN, double, 3> lookAt;
    utils::CSVec<CS::CARTESIAN, double, 3> up;
};

class Window
{
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

        Window();
        Window(int width, int height);
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
