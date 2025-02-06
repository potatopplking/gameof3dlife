#include <iostream>
#include <array>

#include <SDL2/SDL.h>
#include <GL/glew.h>

GLfloat vertices[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
};

namespace utils
{

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

            T operator[](int index) const {
                return this->elements[index];
            }

    };

}

utils::Vec<float, 2> A({1.0, 2.0});
utils::Vec<float, 2> B({3.0, 4.0});
auto C = A+B;
auto D = A-B;
float lol = C[0];


namespace UI
{

using Color = std::array<uint8_t, 4>;
using Vec2D = std::array<int, 2>;

class Window
{
    public:
    // SDL and OpenGL attributes
    SDL_Renderer* renderer;
    SDL_Window* window;
    SDL_GLContext context;

    Vec2D size;
    Vec2D mouse_position;

    Window() : 
        size({800, 600}),
        mouse_position({0,0})
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
                    SDL_KeyboardEvent* kbd_event = reinterpret_cast<SDL_KeyboardEvent*>(&event);
                    if (kbd_event->keysym.sym == 'q') {
                        this->exit_requested = true;
                    }
                    break;
                }
                case SDL_QUIT: {
                    this->exit_requested = false;
                    break;
                }
                case SDL_MOUSEMOTION: {
                    SDL_MouseMotionEvent* mouse_event = reinterpret_cast<SDL_MouseMotionEvent*>(&event);
                    if (mouse_event->state == SDL_PRESSED) {
                        std::cout << "Mouse motion:\tx: " << mouse_event->x << "\t\ty: " << mouse_event->y << std::endl; 
                        
                    }
                }
            }
        }
    }

    bool ExitRequested()
    {
        return this->exit_requested;
    }


    

    void ClearWindow(UI::Color c)
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

        // Draw the triangle
        glBegin(GL_TRIANGLES);
        for (int i = 0; i < 3; i++) {
            glVertex3f(vertices[i * 3], vertices[i * 3 + 1], vertices[i * 3 + 2]);
        }
        glEnd();

        // Swap the buffers
        SDL_GL_SwapWindow(window);

    }

    void Flush()
    {
        SDL_RenderPresent(this->renderer);
    }

    private:
        bool exit_requested = false;
};

}


int main(void)
{
    std::cout << "Press 'q' to quit\n";

    UI::Window window;
    window.Init();

    while (!window.ExitRequested()) {
        window.ProcessEvents();
        window.ClearWindow({100,0,0,255});
        window.Flush();
    }

    return 0;
}
