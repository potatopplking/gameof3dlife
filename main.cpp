// TODO use uniform initialization
// TODO use std::initializer_list ?

#include <cassert>
#include <iostream>
#include <array>
#include <vector>
#include <memory>
#include <cmath>
#include <thread>
#include <chrono>
#include <random>

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
	    Vec(std::initializer_list<T> list) {
		assert(size >= list.size());
		//std:: cout << "Using std::initializer_list constructor" << std::endl;
		size_t i = 0;
		for (auto& l : list) {
			this->elements[i++] = l;
		}
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

	    bool operator==(const Vec& other) {
		return this->elements == other.elements;
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
		    // C++17 constexpr if
		    if constexpr (std::is_integral_v<T>) {
                	std::cout << int(element) << " ";
		    } else if constexpr (std::is_floating_point_v<T>) {
                    	std::cout << float(element) << " ";
                    }
		}
                std::cout << "}";
                return os;
            }

    };

  using Color = Vec<uint8_t, 4>;

}

// Voxel and GridSize are used both in Simulation and Window
// Simulation uses i,j,k coords - rows, cols, stacks
// window uses x,y,z,w (openGL coords)

using SimCoords = utils::Vec<int32_t, 3>;

struct Voxel {
    // coordinates of the voxel are given by its position in Voxel array
    utils::Color color;
    SimCoords position;
    // TODO 
    // create VBO (or something else) when creating Simulation,
    // then during Updates only pass color array to the Window to draw them
    Voxel() : color({0,0,0,0}), position({0,0,0}) {} 

};


namespace Simulation {

  class BaseSimulation {
    public:
      // tries to conform to dt, but may differ - returns time that actually passed in the simulation
      virtual double Step(double dt) = 0;
      virtual void InitRandomState() = 0;
      virtual const utils::Vec<int32_t, 3>& GetGridSize() = 0;
      virtual const std::vector<Voxel>& GetVoxels() = 0; // TODO vector not efficient?
  };

  class GameOfLife2D : public BaseSimulation {
    public:
      GameOfLife2D(int32_t rows, int32_t cols): gridSize{rows,cols,1} {
          this->voxels.resize(rows * cols);
	  auto black = utils::Color{0,0,0,255};
          size_t i = 0;
          const float voxel_size = 1.0f;

          for (int row = 0; row < rows; row++) {
              for (int col = 0; col < cols; col++) {
                uint32_t index = row*cols + col; 
                this->voxels[index].color = black;
		this->voxels[index].position = {row,col,0};
              }
          }
          this->InitRandomState();
      }

      ~GameOfLife2D() = default;

      // (Re)initialize to random state
      void InitRandomState() override {
          std::random_device rd;
          std::mt19937 gen(rd());
          std::bernoulli_distribution dis(0.5);

          auto [rows, cols, _] = this->GetGridSize().elements;
          this->cells.resize(rows*cols);
          for (uint32_t index = 0; index < rows*cols; index++) {
                this->cells[index] = dis(gen);
          }
      }

      double Step(double dt) override {
        utils::Color white({255,255,255,255});
        utils::Color black({0,0,0,255});
        auto [rows, cols, _] = this->gridSize.elements; 

        for (uint32_t i = 0; i < rows*cols; i++) {
            auto neighbours_alive = this->SumNeighbouringCells(i);
            if (neighbours_alive < 2) {
                this->cells[i] = 0; // dies from underpopulation
            } else if (neighbours_alive == 3) {
                this->cells[i] = 1; // either stays alive or gets created
            } else if (neighbours_alive > 3) {
                this->cells[i] = 0; // dies from overpopulation
            }
        }


        for (uint32_t index = 0; index < rows*cols; index++) {
            this->voxels[index].color = this->cells[index] ? white : black;
        }
        return dt;
      }

      const utils::Vec<int32_t, 3>& GetGridSize() override {
        return this->gridSize;
      }

      const std::vector<Voxel>& GetVoxels() override {
        return this->voxels;
      }

    private:
      SimCoords gridSize;
      std::vector<Voxel> voxels;
      std::vector<bool> cells;

      uint32_t SumNeighbouringCells(uint32_t index) {
        auto [rows, cols, _] = this->GetGridSize().elements;
        int32_t row = index / cols;
        int32_t col = index % cols;
        uint32_t sum_alive = 0;
        int32_t up_row = row-1;
        bool up_exists = up_row >= 0;
        int32_t down_row = row+1;
        bool down_exist = down_row < rows;
        int32_t left_col = col-1;
        bool left_exists = left_col >= 0;
        int32_t right_col = col+1;
        bool right_exists = right_col < cols;
        
        if (up_exists) {
            if (left_exists) {
                sum_alive += cells[up_row*cols + left_col];
            }
            sum_alive += cells[up_row*cols + col];
            if (right_exists) {
                sum_alive += cells[up_row*cols + right_col];
            }
        }
        if (left_exists) {
            sum_alive += cells[row*cols + left_col];
        }
        if (right_exists) {
            sum_alive += cells[row*cols + right_col];
        }
        if (down_exist) {
            if (left_exists) {
                sum_alive += cells[down_row*cols + left_col];
            }
            sum_alive += cells[down_row*cols];
            if (right_exists) {
                sum_alive += cells[down_row*cols + right_col];
            }
        }
        // TODO tohle je fakt bida, urcite to jde lip

        return sum_alive;
      }
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

void draw_cube() {
    draw_cube(SimCoords({0,0,0}));
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
        size({2000,1000}),
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
        // TODO timing
        static uint32_t timer = 0;
        if (++timer%500 == 0) {
            double time_passed = this->sim->Step(dt);
        }
        auto voxels = this->sim->GetVoxels();
        this->Render(voxels);
    }

    
    private:
        bool exit_requested = false;
        void Render(const std::vector<Voxel>& voxels) {
            glViewport(0, 0, this->size[0], this->size[1]);

            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluPerspective(45.0f, 640.0f / 480.0f, 0.1f, 100.0f);

            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            auto coords = this->camera_pos.toCartesian();
            gluLookAt(coords[0], coords[1], coords[2], 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

            for (auto& voxel : voxels) {
                auto R = voxel.color[0], G = voxel.color[1], B = voxel.color[2], A = voxel.color[3];
                glColor3ub(R,G,B);
                draw_cube(voxel.position);
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
    window.sim = std::make_unique<Simulation::GameOfLife2D>(20,20);

    std::cout << "grid size: " << window.sim->GetGridSize() << std::endl;
    std::cout << "voxel color : " << window.sim->GetVoxels()[0].color[1] << std::endl;

    while (!window.ExitRequested()) {
        window.ProcessEvents();
        window.ClearWindow(utils::Color({100,0,0,255}));
        window.Update();
    }

    return 0;
}
