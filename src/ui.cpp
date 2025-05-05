#include <cmath>
#include <vector>
#include <numbers>
#include <iostream>

#include <SDL3/SDL.h>
#include <GL/glew.h>

#include "ui.hpp"
#include "utilities.hpp"

// A hack to get the thing to compile on Windows:
// this probably due to the fact that GLEW has to be compiled with C linkage,
// and the rest of the code is compiled with C++ linkage.
#ifdef _WIN32
GLenum GLEWAPIENTRY glewInit(void);
#endif

namespace UI {

void draw_cube(utils::SimCoords pos) {
    // TODO asi jsou spatne normaly - pruhledne to vykresluje spatne
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    glLoadIdentity();
//
//    glTranslatef(0.0f, 0.0f, -5.0f);
//    glRotatef(30.0f, 1.0f, 0.0f, 0.0f);
//    glRotatef(30.0f, 0.0f, 1.0f, 0.0f);
//
    // TODO probably should set matrix mode
    glPushMatrix();
    glTranslatef(
        float(pos[0]), float(pos[1]), float(pos[2])
    );
    GLfloat vertices[] = {
        // Front face
        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        // Back face
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f
    };

    GLuint indices[] = {
        2, 1, 0,    0, 3, 2,    // Front
        1, 5, 6,    6, 2, 1,    // Right
        7, 6, 5,    5, 4, 7,    // Back
        4, 0, 3,    3, 7, 4,    // Left
        4, 5, 1,    1, 0, 4,    // Bottom
        3, 2, 6,    6, 7, 3     // Top
    };

    GLfloat normals[] = {
        // Front
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        // Back
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f
    };

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glNormalPointer(GL_FLOAT, 0, normals);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, indices);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glPopMatrix();
}

void enable_light() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);    // Enable lighting
    glEnable(GL_LIGHT0);      // Enable light #0
    glEnable(GL_COLOR_MATERIAL);

    // Add ambient light
    GLfloat ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);

    // Configure light 0
    GLfloat light_position[] = { 10.0f, 10.0f, 10.0f, 0.0f };
    GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
}

Window::Window(int width, int height) :
    size{ width,height },
    mouse_prev_pos{ 0,0 },
    renderer{ nullptr },
    window{ nullptr },
    context{ nullptr }
{
    using namespace utils;
    Log::debug("Window constructor called");
    camera.pos[SphericalIndex::PHI] = -2.0;
    camera.pos[SphericalIndex::THETA] = 1.0;
    camera.pos[SphericalIndex::R] = 30.0;

    camera.up[CartesianIndex::X] = 0.0;
    camera.up[CartesianIndex::Y] = 0.0;
    camera.up[CartesianIndex::Z] = 1.0;

    camera.lookAt[CartesianIndex::X] = 0.0;
    camera.lookAt[CartesianIndex::Y] = 0.0;
    camera.lookAt[CartesianIndex::Z] = 0.0;
}

Window::~Window()
{
    SDL_DestroyRenderer(this->renderer);
    SDL_GL_DestroyContext(this->context);
    SDL_DestroyWindow(this->window);
    SDL_Quit();
    Log::debug("Window destructor called");
}

int Window::Init()
{
    if (SDL_Init(SDL_INIT_VIDEO) == false) {
        Log::error("SDL could not initialize! Error: ", SDL_GetError());
        return 1;
    }
    this->window = SDL_CreateWindow(
	"SDL2 Window",
	this->size[0],
	this->size[1],
	SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );
    if (this->window == nullptr) {
        Log::error("Window could not be created! Error: ", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    this->context = SDL_GL_CreateContext(this->window);
    if (this->context == nullptr) {
        Log::error("GL context could not be created! Error: ", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    if (glewInit() != GLEW_OK) {
        Log::error("GLEW init failed!");
        SDL_GL_DestroyContext(this->context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    Resize(this->size);

    this->renderer = SDL_CreateRenderer(window, NULL);
    if (this->renderer == nullptr) {
        Log::error("Renderer could not be created! Error: ", SDL_GetError());
        SDL_DestroyWindow(this->window);
        SDL_Quit();
        return 1;
    }


    return 0;
}

void Window::Resize(int width, int height)
{
    Resize(utils::Vec<int,2>{width, height});
}

void Window::Resize(utils::Vec<int, 2> new_size)
{
    this->size = new_size;
    camera.aspect = static_cast<float>(this->size[0]) / this->size[1];
}

void Window::ProcessEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_EVENT_WINDOW_RESIZED: {
                Log::debug("Window resized event");
                SDL_WindowEvent win = event.window;
                auto new_size = utils::Vec<int,2>{win.data1, win.data2};
                Resize(new_size);
                break;
            }
	        case SDL_EVENT_KEY_DOWN: {
                Log::debug("Got key down");
                SDL_KeyboardEvent kbd_event = event.key;
                if (kbd_event.key == 'q') {
                    this->exit_requested = true;
                } else if (kbd_event.key == 'r') {
                    ResetSimulation();
                } else if (kbd_event.key == 's') {
                    this->camera.SetZoom(1);
                } else if (kbd_event.key == 'w') {
                    this->camera.SetZoom(-1);
                } else if (kbd_event.key == ' ') { // Spacebar to toggle pause
                    this->simulation_paused = !this->simulation_paused;
                    Log::info(this->simulation_paused ? "Simulation paused" : "Simulation resumed");
                }
                break;
            }
            case SDL_EVENT_QUIT: {
                this->exit_requested = true;
                break;
            }
            case SDL_EVENT_MOUSE_BUTTON_DOWN: {
                SDL_MouseButtonEvent mouse_event = event.button;
                if (mouse_event.down == true) {
                    this->mouse_prev_pos = utils::Vec<int,2>{mouse_event.x, mouse_event.y};
                }
                break;
            }
            case SDL_EVENT_MOUSE_WHEEL: {
                SDL_MouseWheelEvent mouse_event = event.wheel;
                this->camera.SetZoom(mouse_event.y);
                break;
            }
            case SDL_EVENT_MOUSE_MOTION: {
                SDL_MouseMotionEvent mouse_event = event.motion;
                auto mouse_current_pos = utils::Vec<int, 2>{mouse_event.x, mouse_event.y};
                auto diff = this->mouse_prev_pos - mouse_current_pos;
                if (mouse_event.state == SDL_BUTTON_LMASK) {
                    camera.SetRotation(diff);
                } else if (mouse_event.state == SDL_BUTTON_MMASK) {
                    camera.SetPan(diff);   
                }
                //Log::debug("Mouse motion event: ", diff[0], ", ", diff[1]);
                mouse_prev_pos = mouse_current_pos;
                break;
            }
        }
    }
}

bool Window::ExitRequested()
{
    return this->exit_requested;
}

void Window::ClearWindow(utils::Color c)
{
auto [R,G,B,A] = c.elements;
//        SDL_SetRenderDrawColor(this->renderer, R, G, B, A);
//        SDL_RenderClear(this->renderer);

    // Clear the screen
    glClearColor(
            static_cast<GLfloat>(R)/255.0f,
            static_cast<GLfloat>(G)/255.0f,
            static_cast<GLfloat>(B)/255.0f,
            static_cast<GLfloat>(A)/255.0f
            );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::UpdateSimulation() {
    // conversion factor between real and simulation time:
    // t_conversion_factor = real_time / simulation_time;
    constexpr double t_conversion_factor = 0.5;

    double current_time = GetUptime();
    static double last_time = current_time;
    static double next_update_time = current_time;
    
    double delta_time = current_time - last_time;
    last_time = current_time;

    if (current_time >= next_update_time) {
        this->sim->Step(0.1);
        next_update_time = current_time + this->sim->GetStepSize()*t_conversion_factor;
    }
}

void Window::Render(const std::vector<Voxel>& voxels) {
    ClearWindow(utils::Color{100,0,0,255});
    glViewport(0, 0, this->size[0], this->size[1]);

    camera.SetPerspectiveProjection();
    camera.TranslateRotateScene();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);

    for (auto& voxel : voxels) {
        auto [R,G,B,A] = voxel.color.elements;
        glColor4ub(R,G,B,A);
        draw_cube(voxel.position);
    }

    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);

    DrawAxis();
    enable_light();
    this->Flush();
}

void Window::DrawAxis() {
    constexpr double length = 100.0;
    glBegin(GL_LINES);
        glColor3f(1.0, 0.0, 0.0);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(length, 0.0, 0.0);
        glColor3f(0.0, 1.0, 0.0);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(0.0, length, 0.0);
        glColor3f(0.0, 0.0, 1.0);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(0.0, 0.0, length);
    glEnd();
}

void Window::Flush()
{
    // Swap the buffers
    SDL_GL_SwapWindow(window);
    // for some reason calling SDL_RenderPresent causes the window to be blank on Windows
    //SDL_RenderPresent(this->renderer);
}

void Window::UpdateTime()
{
    this->real_time_elapsed = SDL_GetTicks() / 1000.0;
}

double Window::GetUptime()
{
    return this->real_time_elapsed;
}

void Window::Run()
{
    while (!ExitRequested()) {
        ProcessEvents();
        UpdateTime();
        if (!this->simulation_paused) {
            UpdateSimulation();
        }

        auto voxels = this->sim->GetVoxels();
        Render(voxels);
    }
}

void Window::SetSimulation(std::unique_ptr<Simulation::BaseSimulation> new_sim)
{
  this->sim = std::move(new_sim);
}

void Window::ResetSimulation() {
    Log::info("Resetting simulation and real time...");
    this->sim->InitRandomState();
    this->real_time_elapsed = 0.0; // Reset real time
}

void Camera::SetPerspectiveProjection()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, this->aspect, 0.1f, 1000.0f); // Increased far clipping plane from 100.0f to 1000.0f
}   

void Camera::TranslateRotateScene()
{
    using namespace utils;
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    auto eye_pos_cartesian = CSVec<CoordinateSystem::CARTESIAN, double, 3>(pos);
    //auto up = CSVec<CoordinateSystem::CARTESIAN, double, 3>{ 0.0, 1.0, 0.0 };
    auto eye_pos = CSVec<CoordinateSystem::CARTESIAN, double, 3>(pos) + offset;
    auto look_at = lookAt + offset;
    gluLookAt(eye_pos[0],
              eye_pos[1],
              eye_pos[2],
              look_at[0],
              look_at[1],
              look_at[2],
              up[0], up[1], up[2]);
}

void Camera::SetZoom(float scroll_diff)
{
    this->pos[utils::SphericalIndex::R] += this->CAMERA_ZOOM_FACTOR * scroll_diff;
}

void Camera::SetPan(MousePos diff)
{
    using namespace utils;
    auto pos_cartesian = CSVec<CoordinateSystem::CARTESIAN, double, 3>(this->pos);
    auto view_vector = this->lookAt - pos_cartesian;
    auto imagePlaneX = CrossProduct(view_vector, this->up);
    auto imagePlaneY = CrossProduct(-imagePlaneX, view_vector);
    imagePlaneX.Normalize();
    imagePlaneY.Normalize();
    Log::debug("----------------------------");
    Log::debug("pos_cartesian: ", pos_cartesian);
    Log::debug("this->up: ", this->up);
    Log::debug("view_vector: ", view_vector);
    Log::debug("imagePlaneX: ", imagePlaneX);
    Log::debug("imagePlaneY: ", imagePlaneY);
    Log::debug("this->offset: ", this->offset);
    this->offset += imagePlaneX * diff[0] * 0.05;
    this->offset += imagePlaneY * diff[1] * 0.05;

    Log::debug("SetPan called: (", diff[0], ", ", diff[1], ")");
}

void Camera::SetRotation(MousePos diff)
{
    using namespace utils;

    auto& theta = this->pos[SphericalIndex::THETA];
    theta += diff[1] * 0.005; 
    theta = theta > M_PI ? M_PI : theta;
    theta = theta <= 0.1 ? 0.1 : theta;

    auto &phi = this->pos[SphericalIndex::PHI];
    phi += diff[0] * 0.005;

    Log::debug("SetRotation called: (", diff[0], ", ", diff[1], ")");
    auto eye_pos_cartesian = CSVec<CoordinateSystem::CARTESIAN, double, 3>(pos);
    Log::debug("	Camera up: ", up);
    Log::debug("	Camera lookAt: ", lookAt);
    Log::debug("	Camera pos: ", pos);
    Log::debug("	Camera pos cartesian: ", eye_pos_cartesian);
}


}
