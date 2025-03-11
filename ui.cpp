#include <iostream>
#include <vector>
#include <cmath>

#include <SDL2/SDL.h>
#include <GL/glew.h>

#include "ui.hpp"
#include "utilities.hpp"

namespace UI {

utils::Vec<double, 3> SphericCoords::toCartesian() {
    auto phi = coords[0] * M_PI / 180;
    auto theta = coords[1] * M_PI / 180;
    auto r = coords[2];

    auto x = r * sin(phi) * cos(theta);
    auto y = r * sin(phi) * sin(theta);
    auto z = r * cos(phi);

    return utils::Vec<double, 3>({x,y,z});
}

void draw_cube(SimCoords pos) {
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

Window::Window() : 
    size{1200,1200},
    mouse_position{0,0},
    mouse_init_position{0,0}
{
    std::cout << "Window constructor called\n";
}

Window::~Window()
{
    SDL_DestroyRenderer(this->renderer);
    SDL_GL_DeleteContext(this->context);
    SDL_DestroyWindow(this->window);
    SDL_Quit();
    std::cout << "Window destructor called\n";
}

int Window::Init()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! Error: " << SDL_GetError() << std::endl;
        return 1;
    }
    this->window = SDL_CreateWindow("SDL2 Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, this->size[0], this->size[1], SDL_WINDOW_OPENGL);
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
        SDL_GL_DeleteContext(this->context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    glViewport(0, 0, this->size[0], this->size[1]);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
auto aspect = static_cast<float>(this->size[0])/this->size[1];
    gluPerspective(45.0f, aspect, 0.1f, 100.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0f, 0.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

    this->renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (this->renderer == nullptr) {
        std::cerr << "Renderer could not be created! Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }


    return 0;
}

void Window::ProcessEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_KEYDOWN: {
                std::cout << "Got key down\n";
                SDL_KeyboardEvent kbd_event = event.key;
                if (kbd_event.keysym.sym == 'q') {
                    this->exit_requested = true;
                } else if (kbd_event.keysym.sym == 'r') {
                    std::cout << "Reinitializing simulation..." << std::endl;
                    this->sim->InitRandomState();
                }
                break;
            }
            case SDL_QUIT: {
                this->exit_requested = false;
                break;
            }
            case SDL_MOUSEBUTTONDOWN: {
                SDL_MouseButtonEvent mouse_event = event.button;
                if (mouse_event.state == SDL_PRESSED) {
                    std::cout << "Mouse pressed, setting init mouse position (" << mouse_event.x << ", " << mouse_event.y << ")" << std::endl;
                    this->mouse_init_position[0] = mouse_event.x;
                    this->mouse_init_position[1] = mouse_event.y;
                }
                break;
            }
            case SDL_MOUSEWHEEL: {
                SDL_MouseWheelEvent mouse_event = event.wheel;
                this->camera_pos.coords[2] += this->CAMERA_ZOOM_FACTOR * mouse_event.preciseY;
                std::cout << "Mouse wheel event, new coord[3] = " << this->camera_pos.coords[2] << std::endl;
                break;
            }
            case SDL_MOUSEMOTION: {
                SDL_MouseMotionEvent mouse_event = event.motion;
                if (mouse_event.state == SDL_PRESSED) {
                    int diff_x = this->mouse_init_position[0] - mouse_event.x; 
                    int diff_y = this->mouse_init_position[1] - mouse_event.y;
                    auto pos = VEC_FROM_XY(mouse_event);
                    auto diff = this->mouse_init_position - pos;
                    std::cout << "Mouse motion diff: " << diff;
                    this->camera_pos.coords[0] += -1.0 * diff[0] * 0.1;
                    this->camera_pos.coords[1] += -1.0 * diff[1] * 0.1;
                    std::cout << " yaw: " << this->camera_pos.coords[0] << " deg; pitch: " << this->camera_pos.coords[1] << "deg\n";
                    auto coords = this->camera_pos.toCartesian();
                    std::cout << "spherical: " << this->camera_pos.coords 
                                << " to cartesian coords: " << coords << std::endl;
                    mouse_init_position[0] = mouse_event.x;
                    mouse_init_position[1] = mouse_event.y;
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
            static_cast<GLfloat>(R)/255.0,
            static_cast<GLfloat>(G)/255.0,
            static_cast<GLfloat>(B)/255.0,
            static_cast<GLfloat>(A)/255.0
            );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::Update() {
    const double dt = 0.1;
    // TODO timing
    static uint32_t timer = 0;
    if (++timer%500 == 0) {
        double time_passed = this->sim->Step(dt);
    }
    auto voxels = this->sim->GetVoxels();
    this->Render(voxels);
}

void Window::Render(const std::vector<Voxel>& voxels) {
    glViewport(0, 0, this->size[0], this->size[1]);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    auto aspect = static_cast<float>(this->size[0]) / this->size[1];
    gluPerspective(45.0f, aspect, 0.1f, 100.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    auto cam_pos = this->camera_pos.toCartesian();
    gluLookAt(cam_pos[0], cam_pos[1], cam_pos[2], 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

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
    SDL_RenderPresent(this->renderer);
}

}
