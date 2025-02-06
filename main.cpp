#include <iostream>
#include <array>

#include <SDL2/SDL.h>
#include <GL/glew.h>


namespace UI
{

using Color = std::array<uint8_t, 4>;

class Window
{
    public:
    SDL_Renderer* renderer;
    SDL_Window* window;
    SDL_GLContext context;

    Window()
    {
        std::cout << "Window constructor called\n";
    }

    ~Window()
    {
        SDL_DestroyRenderer(this->renderer);
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

        this->window = SDL_CreateWindow("SDL2 Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
        if (this->window == nullptr) {
            std::cerr << "Window could not be created! Error: " << SDL_GetError() << std::endl;
            SDL_Quit();
            return 1;
        }

//        this->context = SDL_GL_CreateContext(this->window);
//        if (this->context == nullptr) {
//            std::cerr << "GL context could not be created! Error: " << SDL_GetError() << std::endl;
//            SDL_DestroyWindow(window);
//            SDL_Quit();
//        }

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
            if (event.type == SDL_KEYDOWN) {
                std::cout << "Got key down\n";
                SDL_KeyboardEvent* kbd_event = reinterpret_cast<SDL_KeyboardEvent*>(&event);
                if (kbd_event->keysym.sym == 'q') {
                    this->exit_requested = true;
                }
            }
            if (event.type == SDL_QUIT) {
                this->exit_requested = false;
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
        SDL_SetRenderDrawColor(this->renderer, R, G, B, A);
        SDL_RenderClear(this->renderer);
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
