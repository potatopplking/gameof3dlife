#include <iostream>
#include <array>
#include <vector>
#include <memory>
#include <cmath>

#include <SDL2/SDL.h>
#include <GL/glew.h>

GLfloat vertices[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
};

namespace utils
{
    #define VEC_FROM_XY(obj) utils::Vec<int,2>({obj.x, obj.y})

    template<typename T, int size>
    class Vec
    {

        // TODO SIMD intrinsics
        // TODO tests

        public:
            std::array<T, size> elements;

            Vec(std::array<T, size> init_elements) {
                // by copy
                this->elements = init_elements;
            }

            Vec operator+(const Vec& other) {
                auto result = other;
                for (int i = 0; i < size; i++) {
                    result.elements[i] += this->elements[i];
                }
                return result;
            }

            Vec operator-(const Vec& other) {
                auto result = other;
                for (int i = 0; i < size; i++) {
                    result.elements[i] -= this->elements[i];
                }
                return result;
            }

            T& operator[](int index) {
                return this->elements[index];
            }


            const T& operator[](int index) const {
                return this->elements[index];
            }


            friend std::ostream& operator<<(std::ostream& os, const Vec& obj) {
                std::cout << "{ ";
                for (const auto& element : obj.elements) {
                    std::cout << element << " ";
                }
                std::cout << "}";
                return os;
            }

    };

  using Color = Vec<uint8_t, 4>;

}

utils::Vec<float, 2> A({1.0, 2.0});
utils::Vec<float, 2> B({3.0, 4.0});
auto C = A+B;
auto D = A-B;
float lol = C[0];

// Voxel and GridSize are used both in Simulation and Window
// Simulation uses i,j,k coords - rows, cols, stacks
// window uses x,y,z,w (openGL coords)

struct Voxel {
    // coordinates of the voxel are given by its position in Voxel array
    utils::Color color;

    Voxel() : color({0,0,0,0}) {} 
};

using SimCoords = utils::Vec<uint32_t, 3>;

namespace Simulation {

  class BaseSimulation {
    public:
      // tries to conform to dt, but may differ - returns time that actually passed in the simulation
      virtual double Step(double dt) = 0;
      virtual void InitRandomState() = 0;
      virtual const utils::Vec<uint32_t, 3>& GetGridSize() = 0;
      virtual const std::vector<Voxel>& GetVoxels() = 0; // TODO vector not efficient?
  };

  class GameOfLife2D : public BaseSimulation {
    public:
      GameOfLife2D(uint32_t rows, uint32_t cols): gridSize({rows,cols,1}) {
          this->voxels.resize(rows * cols);
          std::array<utils::Color, 2> colors{
              utils::Color({0,0,0,255}),
              utils::Color({255,255,255,255})
          };
          size_t i = 0;
          for (auto& voxel : this->voxels) {
              voxel.color = colors[++i % 2];
          }
      }

      ~GameOfLife2D() {

      }

      // (Re)initialize to random state
      void InitRandomState() override {
        
      }

      double Step(double dt) override {
        return dt;
      }

      const utils::Vec<uint32_t, 3>& GetGridSize() override {
        return this->gridSize;
      }

      const std::vector<Voxel>& GetVoxels() override {
        return this->voxels;
      }

    private:
      SimCoords gridSize;
      std::vector<Voxel> voxels;

  };
}


namespace UI
{

using Vec2D = std::array<int, 2>;

// tracks position on a sphere. Used for camera
class SphericCoords
{
    public:
        // pitch, yaw, radius
        utils::Vec<double, 3> coords;

        SphericCoords(): coords({.0,.0,.0}) {}
        SphericCoords(double phi, double theta, double r): coords({phi, theta, r}) {
            std::cout << "SphericCoords initalized: " << coords << std::endl;
        }

        utils::Vec<double, 3> toCartesian() {
            auto phi = coords[0] * M_PI / 180;
            auto theta = coords[1] * M_PI / 180;
            auto r = coords[2];

            auto x = r * sin(phi) * cos(theta);
            auto y = r * sin(phi) * sin(theta);
            auto z = r * cos(phi);
            
            return utils::Vec<double, 3>({x,y,z});
        }
};

void draw_cube() {
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    glLoadIdentity();
//
//    glTranslatef(0.0f, 0.0f, -5.0f);
//    glRotatef(30.0f, 1.0f, 0.0f, 0.0f);
//    glRotatef(30.0f, 0.0f, 1.0f, 0.0f);
//
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
    SphericCoords camera_pos{.0, .0, 5.0};

    Window() : 
        size({800, 600}),
        mouse_position({0,0}),
        mouse_init_position({0,0})
    {
        std::cout << "Window constructor called\n";
    }

    ~Window()
    {
        SDL_DestroyRenderer(this->renderer);
        SDL_GL_DeleteContext(this->context);
        SDL_DestroyWindow(this->window);
        SDL_Quit();
        std::cout << "Window destructor called\n";
    }

    int Init()
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
        gluPerspective(45.0f, 640.0f / 480.0f, 0.1f, 100.0f);

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

    void ProcessEvents()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_KEYDOWN: {
                    std::cout << "Got key down\n";
                    SDL_KeyboardEvent kbd_event = event.key;
                    if (kbd_event.keysym.sym == 'q') {
                        this->exit_requested = true;
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

    bool ExitRequested()
    {
        return this->exit_requested;
    }


    

    void ClearWindow(utils::Color c)
    {
        uint8_t R = c[0], G = c[1], B = c[2], A = c[3];
//        SDL_SetRenderDrawColor(this->renderer, R, G, B, A);
//        SDL_RenderClear(this->renderer);

        // Clear the screen
        glClearColor(
                (GLfloat)R/255.0,
                (GLfloat)G/255.0,
                (GLfloat)B/255.0,
                (GLfloat)A/255.0
                );
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void Update() {
        const double dt = 0.1;
        double time_passed = this->sim->Step(dt);
        auto voxels = this->sim->GetVoxels();
        this->Render(voxels); 
    }

    
    private:
        bool exit_requested = false;
        void Render(const std::vector<Voxel>& voxels) {
            // TODO render voxels
            glViewport(0, 0, this->size[0], this->size[1]);

            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluPerspective(45.0f, 640.0f / 480.0f, 0.1f, 100.0f);

            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            auto coords = this->camera_pos.toCartesian();
            gluLookAt(coords[0], coords[1], coords[2], 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

            for (int i = 0; i < 10; i++) {
                draw_cube();
                glTranslatef(1.0f, 0.0f, 0.0f);
                uint8_t R = 17*i + 15;
                uint8_t G = 11*i + 50;
                uint8_t B = 47*i + 100;
                glColor3ub(R,G,B);
            }
            enable_light();
            this->Flush();
        }
        void Flush()
        {
            // Swap the buffers
            SDL_GL_SwapWindow(window);
            SDL_RenderPresent(this->renderer);
        }
};

}

int main(void)
{
    std::cout << "Press 'q' to quit\n";

    UI::Window window;
    window.Init();
    window.sim = std::make_unique<Simulation::GameOfLife2D>(10,10);

    std::cout << "grid size: " << window.sim->GetGridSize() << std::endl;
    std::cout << "voxel color : " << window.sim->GetVoxels()[0].color[1] << std::endl;

    while (!window.ExitRequested()) {
        window.ProcessEvents();
        window.ClearWindow(utils::Color({100,0,0,255}));
        window.Update();
    }

    return 0;
}
