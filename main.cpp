#include <iostream>
#ifdef _WIN32
#include <Windows.h>
#endif
#define _USE_MATH_DEFINES
#include <cstdio>
#include <thread>
#include <atomic>
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



std::string key = "4613~JW2h7AZXUK6LBkEnKxtPtmrJ22cUeUVGyfYCUhnuNNKat46nVAVN9ufUvaFUxvZA";
std::string CourseBase = "https://creanlutheran.instructure.com/api/v1/"; // conversations, courses


std::atomic_int stage;
int tcount = 4;


float deltime = 0;
float then = 0;
float now = 0;


struct course{
    std::string name;
    int id;
};


struct item{
    std::string name;
    int id;
};


std::vector<course> courses;
std::vector<std::vector<item> > items;


std::string command(const char * cmd){
    FILE * pipe = popen(cmd, "r");
    if (!pipe) return "";

    char buffer[128];
    std::string returnv = "";
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr){
        returnv += std::string() + buffer;
    }
    pclose(pipe);
    return returnv;
}


std::vector<std::string> split(std::string value, std::string key){
    std::string check;
    std::string add;
    std::vector<std::string> returnv;
    for (int i = 0; i < value.length(); i++){
        if (key.contains(value[i])){
            check += value[i];
        }
        else{
            add += check + value[i];
            check = "";
        }
        if (check == key){
            returnv.push_back(add);
            check = "";
            add = "";
        }
    }
    returnv.push_back(add);
    return returnv;
}


std::string splice(std::string value, int start, int end){
    std::string returnv;
    for (int i = start; i < value.length()-end; i++){
        returnv += value[i];
    }
    return returnv;
}


course ToCourse(std::string value){
    std::vector<std::string> returnv;
    std::string add;
    int depth = 0;
    for (int i = 0; i < value.length(); i++){
        if (std::string("([{").contains(value[i])) depth++;
        elif (std::string(")]}").contains(value[i])) depth--;
        elif (depth == 0 && std::string(",").contains(value[i])){
            returnv.push_back(add);
            add = "";
        }
        else add += value[i];
    }
    return {split(returnv[1], ":")[1], std::stoi(split(returnv[0], ":")[1])};
}


void FirStage(){
    std::string value = splice(command(("curl -H 'Authorization: Bearer " + key + "' " + CourseBase + "courses").c_str()), 2, 2);
    std::vector<std::string> RawCourses;
    std::string add;
    int depth = 0;
    for (int i = 0; i < value.length(); i++){
        if (std::string("([{").contains(value[i])) depth++;
        elif (std::string(")]}").contains(value[i])) depth--;
        elif (depth == 0 && std::string(",").contains(value[i])){
            RawCourses.push_back(add);
            add = "";
        }
        else add += value[i];
    }

    for (int i = 0; i < RawCourses.size(); i++){
        std::cout << RawCourses[i] << std::endl;
        courses.push_back(ToCourse(RawCourses[i]));
    }

    stage.store(1);
}


void SecondStage(int i){
    std::cout << command(("curl -H 'Authorization: Bearer " + key + "' " + CourseBase + "courses/" + std::to_string(i) + "/modules").c_str()) << std::endl;
    stage.fetch_add(1);
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
    std::cout << "Successfully initialized SDL3" << std::endl;


    /* Initialize SDL_ttf, create font object */
    TTF_Init();
    TTF_Font * font = TTF_OpenFont((SDL_GetBasePath()+rpath+"Font.ttf").c_str(), 12);


    /* Init text assistant :) */
    TextCharacters Characters = {renderer, font, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890,.~!@#$%^&*()_+-=:;\"'? "};
    TextObject Title = {"", Center, Center, Vector2(windowsize.x/2, windowsize.y/2), {(Uint8)(255*darkmode), (Uint8)(255*darkmode), (Uint8)(255*darkmode), 255}, true};


    std::thread threads[tcount];
    threads[0] = std::thread(FirStage);


    /* Main loop */
    while (loop){
        /* Get mouse pos and get FPS */
        deltime = (SDL_GetPerformanceCounter() - then) / (double)SDL_GetPerformanceFrequency();
        then = SDL_GetPerformanceCounter();


        if (stage.load() == 1){
            threads[0].join();
            std::cout << "Got all courses" << std::endl;
            stage.store(2);
        }
        elif (stage.load() == 2){
            stage.fetch_add(1);
            // for (int i = 0; i < tcount; i++){
            //     threads[i] = std::thread(SecondStage, i);
            // }
        }


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
