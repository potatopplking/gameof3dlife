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
    mouse_position{ 0,0 },
    mouse_init_position{ 0,0 },
    renderer{ nullptr },
    window{ nullptr },
    context{ nullptr }
{
    std::cout << "Window constructor called\n";
    camera.pos[0] = -45.0;
    camera.pos[1] = 63.0;
    camera.pos[2] = 30.0;
}

Window::~Window()
{
    SDL_DestroyRenderer(this->renderer);
    SDL_GL_DestroyContext(this->context);
    SDL_DestroyWindow(this->window);
    SDL_Quit();
    std::cout << "Window destructor called\n";
}

int Window::Init()
{
    if (SDL_Init(SDL_INIT_VIDEO) == false) {
        std::cerr << "SDL could not initialize! Error: " << SDL_GetError() << std::endl;
        return 1;
    }
    this->window = SDL_CreateWindow(
	"SDL2 Window",
	this->size[0],
	this->size[1],
	SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );
    if (this->window == nullptr) {
        std::cerr << "Window could not be created! Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    this->context = SDL_GL_CreateContext(this->window);
    if (this->context == nullptr) {
        std::cerr << "GL context could not be created! Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    if (glewInit() != GLEW_OK) {
        std::cerr << "GLEW init failed!" << std::endl;
        SDL_GL_DestroyContext(this->context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    Resize();

    this->renderer = SDL_CreateRenderer(window, NULL);
    if (this->renderer == nullptr) {
        std::cerr << "Renderer could not be created! Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(this->window);
        SDL_Quit();
        return 1;
    }


    return 0;
}

// if called without arguments, just refresh the matrices and viewport
void Window::Resize()
{
    glViewport(0, 0, this->size[0], this->size[1]);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    auto aspect = static_cast<float>(this->size[0])/this->size[1];
    gluPerspective(45.0f, aspect, 0.1f, 100.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    auto cam_pos = this->camera.pos.Convert<utils::CoordinateSystem::CARTESIAN>();
    gluLookAt(cam_pos[0], cam_pos[1], cam_pos[2], 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

    camera.aspect = static_cast<float>(this->size[0]) / this->size[1];
}

void Window::Resize(int width, int height)
{
    Resize(utils::Vec<int,2>{width, height});
}

void Window::Resize(utils::Vec<int, 2> new_size)
{
    this->size = new_size;
    Resize();
}

void Window::ProcessEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_EVENT_WINDOW_RESIZED: {
                std::cout << "Window resized event" << std::endl;
                SDL_WindowEvent win = event.window;
                auto new_size = utils::Vec<int,2>{win.data1, win.data2};
                Resize(new_size);
                break;
            }
	        case SDL_EVENT_KEY_DOWN: {
                std::cout << "Got key down\n";
                SDL_KeyboardEvent kbd_event = event.key;
                if (kbd_event.key == 'q') {
                    this->exit_requested = true;
                } else if (kbd_event.key == 'r') {
                    std::cout << "Reinitializing simulation..." << std::endl;
                    this->sim->InitRandomState();
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
                    std::cout << "Mouse pressed, setting init mouse position (" << mouse_event.x << ", " << mouse_event.y << ")" << std::endl;
                    this->mouse_init_position[0] = static_cast<int>(mouse_event.x);
                    this->mouse_init_position[1] = static_cast<int>(mouse_event.y);
                }
                break;
            }
            case SDL_EVENT_MOUSE_WHEEL: {
                SDL_MouseWheelEvent mouse_event = event.wheel;
                this->camera.pos[2] += this->CAMERA_ZOOM_FACTOR * mouse_event.y;
                std::cout << "Mouse wheel event, new coord[3] = " << this->camera.pos[2] << std::endl;
                break;
            }
            case SDL_EVENT_MOUSE_MOTION: {
                SDL_MouseMotionEvent mouse_event = event.motion;
                if (mouse_event.state == SDL_BUTTON_LMASK) {
                    int diff_x = this->mouse_init_position[0] - static_cast<int>(mouse_event.x);
                    int diff_y = this->mouse_init_position[1] - static_cast<int>(mouse_event.y);
                    auto pos = VEC_FROM_XY(mouse_event);
                    auto diff = this->mouse_init_position - pos;
                    std::cout << "Mouse motion diff: " << diff;
                    this->camera.pos[0] += -1.0 * diff[0] * 0.1;
                    this->camera.pos[1] += -1.0 * diff[1] * 0.1;
                    std::cout << " yaw: " << this->camera.pos[0] << " deg; pitch: " << this->camera.pos[1] << "deg\n";
                    auto coords = this->camera.pos.Convert<utils::CoordinateSystem::CARTESIAN>();
                    std::cout << "spherical: " << this->camera.pos.pos 
                                << " to cartesian coords: " << coords.pos << std::endl;
                    mouse_init_position[0] = static_cast<int>(mouse_event.x);
                    mouse_init_position[1] = static_cast<int>(mouse_event.y);
                }
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
    const double dt = 0.1;
    // TODO timing
    static uint32_t timer = 0;
    if (++timer%50 == 0) {
        double time_passed = this->sim->Step(dt);
    }
}

void Window::Render(const std::vector<Voxel>& voxels) {
    glViewport(0, 0, this->size[0], this->size[1]);

    camera.SetPerspectiveProjection();
    camera.TranslateRotateScene();

    for (auto& voxel : voxels) {
        auto [R,G,B,A] = voxel.color.elements;
        glColor3ub(R,G,B);
        draw_cube(voxel.position);
    }
    enable_light();
    this->Flush();
}

void Window::Flush()
{
    // Swap the buffers
    SDL_GL_SwapWindow(window);
    // for some reason calling SDL_RenderPresent causes the window to be blank on Windows
    //SDL_RenderPresent(this->renderer);
}

void Window::Run()
{
    while (!ExitRequested()) {
        ProcessEvents();
        UpdateSimulation();
        ClearWindow(utils::Color{100,0,0,255});
        auto voxels = this->sim->GetVoxels();
        Render(voxels);
    }
}

void Window::SetSimulation(std::unique_ptr<Simulation::BaseSimulation> new_sim)
{
  this->sim = std::move(new_sim);
}

void Camera::SetPerspectiveProjection()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, this->aspect, 0.1f, 100.0f);
}

void Camera::TranslateRotateScene()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    auto cam_pos = pos.Convert<utils::CoordinateSystem::CARTESIAN>();
    gluLookAt(cam_pos[0], cam_pos[1], cam_pos[2], 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
}

}