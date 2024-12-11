#include <iostream>
#ifdef _WIN32
#include <Windows.h>
#endif
#define _USE_MATH_DEFINES
#include <cstdio>
#include "TextHelp.h"


#undef min
#undef max
#define elif else if
#ifdef _WIN32
#define SDL_MODKEY SDL_SCANCODE_LCTRL
#elifdef __APPLE__
#define SDL_MODKEY SDL_SCANCODE_LGUI
#endif


int os = 0;
std::string rpath;
iVector2 windowsize = {960, 540};


SDL_Event e;
bool loop = true;
bool focus = true;
Vector2 mouse;
const bool * keystates = SDL_GetKeyboardState(NULL);
Uint32 mousebitmask;
bool darkmode = true;


float deltime = 0;
float then = 0;
float now = 0;


std::string command(const char * cmd){
    std::cout << cmd << std::endl;
    FILE * pipe = _popen(cmd, "r");
    if (!pipe) return "";

    char buffer[128];
    std::string returnv = "";
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr){
        returnv += std::string() + buffer;
    }
    _pclose(pipe);
    return returnv;
}


/* Main! */
int main(int argc, char* argv[]) {


    /* Set os variable */

    #ifdef _WIN32
    os = 1;
    #elifdef __APPLE__
    os = 2;
    #elifdef __linux__
    os = 3;
    #endif


    std::string key = "4613~JW2h7AZXUK6LBkEnKxtPtmrJ22cUeUVGyfYCUhnuNNKat46nVAVN9ufUvaFUxvZA";
    std::string CourseBase = "https://creanlutheran.instructure.com/api/v1/"; // conversations, courses


    if (os == 1) rpath = "./";
    elif (os == 2) rpath = "../Resources/";


    /* Initialize SDL, create window and renderer */
    std::cout << "Initializing SDL3" << std::endl;
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window * window = SDL_CreateWindow("RePiskel", windowsize.x, windowsize.y, SDL_WINDOW_RESIZABLE | SDL_WINDOW_MOUSE_CAPTURE);
    SDL_Renderer * renderer = SDL_CreateRenderer(window, NULL);
    SDL_SetWindowMinimumSize(window, 960, 540);
    SDL_SetRenderVSync(renderer, 1);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_StartTextInput(window);
    std::cout << "Success! Initializing loop" << std::endl;
    std::cout << "curl -H \\\"Authorization: Bearer 4613~JW2h7AZXUK6LBkEnKxtPtmrJ22cUeUVGyfYCUhnuNNKat46nVAVN9ufUvaFUxvZA\\\" \\\"https://creanlutheran.instructure.com/api/v1/conversations/94004\\\"" << std::endl;
    std::cout << command("curl -H 'Authorization: Bearer 4613~JW2h7AZXUK6LBkEnKxtPtmrJ22cUeUVGyfYCUhnuNNKat46nVAVN9ufUvaFUxvZA' https://creanlutheran.instructure.com/api/v1/conversations/94004") << std::endl;
    /* Initialize SDL_ttf, create font object */
    TTF_Init();
    TTF_Font * font = TTF_OpenFont((SDL_GetBasePath()+rpath+"Font.ttf").c_str(), 12);


    /* Init text assistant :) */
    TextCharacters Characters = {renderer, font, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890,.~!@#$%^&*()_+-=:;\"'? "};
    TextObject Title = {"", Center, Center, Vector2(windowsize.x/2, windowsize.y/2), {255*darkmode, 255*darkmode, 255*darkmode, 255}, true};


    /* Main loop */
    while (loop){


        /* Get mouse pos and get FPS */
        deltime = (SDL_GetPerformanceCounter() - then) / (double)SDL_GetPerformanceFrequency();
        then = SDL_GetPerformanceCounter();


        /* Update mouse and input text */
        mousebitmask = SDL_GetMouseState(&mouse.x, &mouse.y);
        std::string Input = "";


        /* Poll inputs */
        while (SDL_PollEvent(&e)){
            switch (e.type) {
                case SDL_EVENT_QUIT:
                    loop = false;
                    break;


                /* Resize window */
                case SDL_EVENT_WINDOW_RESIZED:
                    SDL_GetWindowSize(window, &windowsize.x, &windowsize.y);


                /* Reduce FPS if unfocussed */
                case SDL_EVENT_WINDOW_FOCUS_GAINED:
                    focus = true;
                    break;
                case SDL_EVENT_WINDOW_FOCUS_LOST:
                    focus = false;
                    break;


                /* Interact with UI */
                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                    Title.TrySelect(mouse, keystates[SDL_SCANCODE_LSHIFT], Characters);
                    break;


                case SDL_EVENT_KEY_DOWN:


                    /* Undo/Redo */
                    if (keystates[SDL_MODKEY]) {
                        if (e.key.key == SDLK_A){
                            Title.Edit("a", keystates[SDL_MODKEY], Characters);
                        }
                        elif (e.key.key == SDLK_C){
                            Title.Edit("c", keystates[SDL_MODKEY], Characters);
                        }
                        elif (e.key.key == SDLK_X){
                            Title.Edit("x", keystates[SDL_MODKEY], Characters);
                        }
                        elif (e.key.key == SDLK_V){
                            Title.Edit("v", keystates[SDL_MODKEY], Characters);
                        }
                    }


                    /* Change title text */
                    if (e.key.key == SDLK_DELETE || e.key.key == SDLK_BACKSPACE){
                        Title.Delete(e.key.key == SDLK_DELETE);
                    }
                    if (e.key.key == SDLK_LEFT){
                        Title.MoveCursor(keystates[SDL_SCANCODE_LSHIFT] || keystates[SDL_SCANCODE_RSHIFT], keystates[SDL_SCANCODE_LCTRL] || keystates[SDL_SCANCODE_RCTRL], true);
                    }
                    elif (e.key.key == SDLK_RIGHT){
                        Title.MoveCursor(keystates[SDL_SCANCODE_LSHIFT] || keystates[SDL_SCANCODE_RSHIFT], keystates[SDL_SCANCODE_LCTRL] || keystates[SDL_SCANCODE_RCTRL], false);
                    }
                    break;


                case SDL_EVENT_TEXT_INPUT:
                    Title.Edit(e.text.text, false, Characters);
                    break;
            }
        }

        if (mousebitmask & SDL_BUTTON_LMASK){
            Title.ConTrySelect(mouse, Characters);
        }


        Title.Render(renderer, Characters);
        if (Title.Text.length()){
            SDL_SetWindowTitle(window, (Title.Text).c_str());
        }
        else{
            SDL_SetWindowTitle(window, "Canvas View - Log In");
        }


        /* Push render content */
        SDL_RenderPresent(renderer);
        SDL_SetRenderDrawColor(renderer, 255-(Uint8)(255*darkmode), 255-(Uint8)(255*darkmode), 255-(Uint8)(255*darkmode), 255);
        SDL_RenderClear(renderer);


        /* Wait if unfocussed */
        if (!focus) SDL_Delay(250);
    }


    /* Exit properly */
    SDL_DestroyRenderer(renderer);
    SDL_StopTextInput(window);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}


#ifdef _WIN32
/* Windows window subsystem :( */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
    return main(__argc, __argv);
}
#endif
