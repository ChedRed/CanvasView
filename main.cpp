#include <iostream>
#ifdef _WIN32
#include <Windows.h>
#endif
#define _USE_MATH_DEFINES
#include <cstdio>
#include <chrono>
#include <format>
#include <thread>
#include <atomic>
#include <cstdlib>
#include <filesystem>
#include "TextHelp.h"


std::string cmdprefix = "";


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
std::string mainpath;
iVector2 windowsize = {960, 540};


SDL_Event e;
std::atomic_bool loop = true;
bool mainloop = true;
bool focus = true;
Vector2 mouse;
const bool * keystates = SDL_GetKeyboardState(NULL);
SDL_Color MainColor = {215, 55, 37, 255};
Uint32 mousebitmask;
bool darkmode = true;



std::string key = "4613~JW2h7AZXUK6LBkEnKxtPtmrJ22cUeUVGyfYCUhnuNNKat46nVAVN9ufUvaFUxvZA";
std::string CourseBase = "https://creanlutheran.instructure.com/api/v1/"; // conversations, courses


std::atomic_int writing = -1;
std::atomic_bool live;
const int tcount = 7;


float deltime = 0;
float then = 0;
float now = 0;


struct course{
    std::string name;
    int id;
};


struct item{
    std::string name;
    std::string stamp;
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


std::vector<std::string> split(std::string value, std::string key, int limit = 0){
    std::string check;
    std::string add;
    std::vector<std::string> returnv;
    int left = limit;
    bool uselimit = left < 1;
    for (int i = 0; i < value.length(); i++){
        if (key.contains(value[i]) && ((uselimit)?(limit > 0):true)){
            check += value[i];
        }
        else{
            add += check + value[i];
            check = "";
            limit--;
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


item ToItem(std::string value){
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
    int id = std::stoi(split(returnv[0], ":", 1)[1]);
    std::string name;
    for (int i = 0; i < returnv.size(); i++){
        if (split(returnv[i], ":", 1)[0] == "\"name\""){
            name = split(returnv[i], ":", 1)[1];
        }
    }
    return {name, std::format("{:%FT%TZ}", std::chrono::system_clock::now()), id};
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
    return {split(returnv[1], ":", 1)[1], std::stoi(split(returnv[0], ":", 1)[1])};
}


void FirStage(){
    std::string value = splice(command((cmdprefix + "curl --no-progress-meter -H 'Authorization: Bearer " + key + "' " + CourseBase + "courses").c_str()), 1, 1);
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
        courses.push_back(ToCourse(RawCourses[i]));
    }
}


void SecondStage(int course, int thread){
    int page = 1;
    std::string value = "";
    while (true){
        std::string next = splice(command((cmdprefix + "curl --no-progress-meter -H 'Authorization: Bearer " + key + "' " + CourseBase + "courses/" + std::to_string(course) + "/assignments?page=" + std::to_string(page) + "").c_str()), 1, 1);

        if (next == ""){
            break;
        }
        if (page == 1){
            value = next;
        }
        else{
            value += ","+next;
        }
        page++;
    }
    std::vector<std::string> RawAssignments;
    std::string add;
    int depth = 0;
    for (int i = 0; i < value.length(); i++){
        if (std::string("([{").contains(value[i])) depth++;
        elif (std::string(")]}").contains(value[i])) depth--;
        elif (depth == 0 && std::string(",").contains(value[i])){
            RawAssignments.push_back(add);
            add = "";
        }
        else add += value[i];
    }


    std::vector<item> courseitems;
    for (int i = 0; i < RawAssignments.size(); i++){
        courseitems.push_back(ToItem(RawAssignments[i]));
    }
    while (true){
        if (writing.load() == -1){
            writing.store(thread);
            break;
        }
    }
    items.push_back(courseitems);
    std::cout << courseitems.size() << " items in course " << course << std::endl;
    writing.store(-1);
}


void CanvasThread(){
    FirStage();
    std::cout << "Collected all courses" << std::endl;
    std::thread threads[tcount];
    std::atomic_bool tactive[tcount] = {false};
    std::vector<int> ids;
    for (int i = 0; i < courses.size(); i++){
        ids.push_back(courses[i].id);
    }
    std::cout << "Number of IDs: " + std::to_string(ids.size()) << std::endl;
    while (ids.size() > 0){
        for (int i = 0; i < tcount; i++){
            if (threads[i].joinable()){
                threads[i].join();
                std::cout << "Joined thread " + std::to_string(i) << std::endl;
            }
            if (ids.size() > 0){
                threads[i] = std::thread(SecondStage, ids[0], i);
                std::cout << "Started thread " + std::to_string(i) + " on assignments for " + std::to_string(ids[0]) << std::endl;
                ids.erase(ids.begin());
            }
        }
    }
    for (int i = 0; i < tcount; i++){
        if (threads[i].joinable()){
            threads[i].join();
            std::cout << "Joined thread " + std::to_string(i) << std::endl;
        }
    }
    std::cout << "Collected all assignments" << std::endl;

    while (loop.load()){
        SDL_Delay(0);
    }
}



/* Main! */
int main(int argc, char* argv[]) {


    /* Set os variable */
    #ifdef _WIN32
    os = 1;
    cmdprefix = "cmd /C ";
    mainpath = "C:\\Program Files\\Canvas View\\";
    #elifdef __APPLE__
    homedir = std::getenv("HOME");
    mainpath = homedir+"/Library/Application Support/com.chedred.canvasview/";
    os = 2;
    #elifdef __linux__
    os = 3;
    #endif


    if (os == 1) rpath = "./";
    elif (os == 2) rpath = "../Resources/";


    /* Initialize SDL, create window and renderer */
    std::cout << "Initializing SDL3" << std::endl;
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window * window = SDL_CreateWindow("Canvas View", windowsize.x, windowsize.y, SDL_WINDOW_RESIZABLE | SDL_WINDOW_MOUSE_CAPTURE);
    SDL_Renderer * renderer = SDL_CreateRenderer(window, NULL);
    SDL_SetWindowMinimumSize(window, 960, 540);
    SDL_SetRenderVSync(renderer, 1);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_StartTextInput(window);
    std::cout << "Successfully initialized SDL3" << std::endl;


    std::cout << homedir << std::endl;


    /* Create textures */
    int borderscale = 6;
    SDL_Texture * Corner = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, borderscale, borderscale);
    SDL_SetRenderTarget(renderer, Corner);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    Vector2 CenterOf = {borderscale, borderscale};
    Vector2 Pos;
    for (int y = 0; y < borderscale; y++){
        for (int x = 0; x < borderscale; x++){
            Pos = {x, y};
            if ((CenterOf-Pos).Magnitude() <= borderscale+1){
                SDL_SetRenderDrawColor(renderer, MainColor.r, MainColor.g, MainColor.b, (Uint8)limit((borderscale-(CenterOf-Pos).Magnitude()+1)*255, 0, 255));
                SDL_RenderPoint(renderer, x, y);
            }
        }
    }
    SDL_Texture * InvertedCorner = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, borderscale, borderscale);
    SDL_SetRenderTarget(renderer, InvertedCorner);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    for (int y = 0; y < borderscale; y++){
        for (int x = 0; x < borderscale; x++){
            Pos = {x, y};
            if ((CenterOf-Pos).Magnitude() > borderscale){
                SDL_SetRenderDrawColor(renderer, MainColor.r, MainColor.g, MainColor.b, (Uint8)limit(((CenterOf-Pos).Magnitude()-borderscale)*255, 0, 255));
                SDL_RenderPoint(renderer, x, y);
            }
        }
    }


    SDL_SetRenderTarget(renderer, NULL);


    /* Initialize SDL_ttf, create font object */
    TTF_Init();
    TTF_Font * font = TTF_OpenFont((SDL_GetBasePath()+rpath+"Font.ttf").c_str(), 12);


    /* Init text assistant :) */
    TextCharacters CharactersB = TextCharacters(renderer, font, "");
    TextCharacters * Characters = &CharactersB;
    TextObject Title = TextObject("", Characters, Center, Top, Vector2((float)windowsize.x/2, (float)windowsize.y/2), {(Uint8)(255*darkmode), (Uint8)(255*darkmode), (Uint8)(255*darkmode), 255}, true);
    TextObject Intro = TextObject("Insert your Canvas API key:", Characters, Center, Bottom, Vector2((float)windowsize.x/2, (float)windowsize.y/2), {(Uint8)(255*darkmode), (Uint8)(255*darkmode), (Uint8)(255*darkmode), 255}, false);


    std::thread canvas = std::thread(CanvasThread);


    /* Main loop */
    while (mainloop){


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
                    mainloop = false;
                    break;


                /* Resize window */
                case SDL_EVENT_WINDOW_RESIZED:
                    SDL_GetWindowSize(window, &windowsize.x, &windowsize.y);
                    Title.Position = {windowsize.x/2, windowsize.y/2};
                    Intro.Position = {windowsize.x/2, windowsize.y/2};


                /* Reduce FPS if unfocussed */
                case SDL_EVENT_WINDOW_FOCUS_GAINED:
                    focus = true;
                    break;
                case SDL_EVENT_WINDOW_FOCUS_LOST:
                    focus = false;
                    break;


                /* Interact with UI */
                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                    Title.TrySelect(mouse, keystates[SDL_SCANCODE_LSHIFT]);
                    break;


                case SDL_EVENT_KEY_DOWN:


                    /* Undo/Redo */
                    if (keystates[SDL_MODKEY]) {
                        if (e.key.key == SDLK_A){
                            Title.Edit("a", keystates[SDL_MODKEY]);
                        }
                        elif (e.key.key == SDLK_C){
                            Title.Edit("c", keystates[SDL_MODKEY]);
                        }
                        elif (e.key.key == SDLK_X){
                            Title.Edit("x", keystates[SDL_MODKEY]);
                        }
                        elif (e.key.key == SDLK_V){
                            Title.Edit("v", keystates[SDL_MODKEY]);
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
                    Title.Edit(e.text.text, false);
                    break;
            }
        }

        if (mousebitmask & SDL_BUTTON_LMASK){
            Title.ConTrySelect(mouse);
        }


        if (Title.Text.length()){
            SDL_SetWindowTitle(window, (Title.Text).c_str());
        }
        else{
            SDL_SetWindowTitle(window, "Canvas View - Log In");
        }


        Title.Render(renderer, deltime);
        Intro.Render(renderer);
        SDL_FRect temprect = {borderscale, borderscale, borderscale, borderscale};
        SDL_RenderTextureRotated(renderer, Corner, NULL, &temprect, 0, NULL, SDL_FLIP_NONE);
        temprect = {borderscale * 2, borderscale * 2, borderscale, borderscale};
        SDL_RenderTextureRotated(renderer, InvertedCorner, NULL, &temprect, 0, NULL, SDL_FLIP_NONE);
        temprect = (SDL_FRect){windowsize.x - (borderscale * 2), borderscale, borderscale, borderscale};
        SDL_RenderTextureRotated(renderer, Corner, NULL, &temprect, 90, NULL, SDL_FLIP_NONE);
        temprect = (SDL_FRect){windowsize.x - (borderscale * 3), borderscale * 2, borderscale, borderscale};
        SDL_RenderTextureRotated(renderer, InvertedCorner, NULL, &temprect, 90, NULL, SDL_FLIP_NONE);
        temprect = (SDL_FRect){windowsize.x - (borderscale * 2), windowsize.y - (borderscale * 2), borderscale, borderscale};
        SDL_RenderTextureRotated(renderer, Corner, NULL, &temprect, 180, NULL, SDL_FLIP_NONE);
        temprect = (SDL_FRect){windowsize.x - (borderscale * 3), windowsize.y - (borderscale * 3), borderscale, borderscale};
        SDL_RenderTextureRotated(renderer, InvertedCorner, NULL, &temprect, 180, NULL, SDL_FLIP_NONE);
        temprect = (SDL_FRect){borderscale, windowsize.y - (borderscale * 2), borderscale, borderscale};
        SDL_RenderTextureRotated(renderer, Corner, NULL, &temprect, 270, NULL, SDL_FLIP_NONE);
        temprect = (SDL_FRect){borderscale * 2, windowsize.y - (borderscale * 3), borderscale, borderscale};
        SDL_RenderTextureRotated(renderer, InvertedCorner, NULL, &temprect, 270, NULL, SDL_FLIP_NONE);


        SDL_SetRenderDrawColor(renderer, MainColor.r, MainColor.g, MainColor.b, MainColor.a);
        temprect = (SDL_FRect){borderscale * 2, borderscale, windowsize.x - (borderscale * 4), borderscale};
        SDL_RenderFillRect(renderer, &temprect);
        temprect = (SDL_FRect){borderscale, borderscale * 2, borderscale, windowsize.y - (borderscale * 4)};
        SDL_RenderFillRect(renderer, &temprect);
        temprect = (SDL_FRect){windowsize.x - (borderscale * 2), borderscale * 2, borderscale,  windowsize.y - (borderscale * 4)};
        SDL_RenderFillRect(renderer, &temprect);
        temprect = (SDL_FRect){borderscale * 2, windowsize.y - (borderscale * 2), windowsize.x - (borderscale * 4), borderscale};
        SDL_RenderFillRect(renderer, &temprect);


        /* Push render content */
        SDL_RenderPresent(renderer);
        SDL_SetRenderDrawColor(renderer, 255-(Uint8)(255*darkmode), 255-(Uint8)(255*darkmode), 255-(Uint8)(255*darkmode), 255);
        SDL_RenderClear(renderer);


        /* Wait if unfocussed */
        if (!focus) SDL_Delay(250);
    }


    /* Exit properly */
    loop.store(false);
    SDL_DestroyRenderer(renderer);
    SDL_StopTextInput(window);
    SDL_DestroyWindow(window);
    SDL_Quit();
    canvas.join();
    return 0;
}


#ifdef _WIN32
/* Windows window subsystem :( */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
    return main(__argc, __argv);
}
#endif
