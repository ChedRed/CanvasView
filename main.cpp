#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"
#include "Vector2.h"
#include <fstream>
#include <functional>
#include <iostream>
#ifdef _WIN32
#include <Windows.h>
#endif
#define _USE_MATH_DEFINES
#include <curl/curl.h>
#include <cstdio>
#include <format>
#include <thread>
#include <atomic>
#include <cstdlib>
#include <chrono>
#include <filesystem>
#include "TextHelp.h"
#include "ui.h"


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
std::string homedir;
std::string mainpath;
iVector2 windowsize = {960, 540};
Vector2 WindowSize = {960, 540};
SDL_Window * Window;
SDL_Renderer * Render;


CURL * handle;
static char errorBuffer[CURL_ERROR_SIZE];
std::atomic_bool loop = true;
std::atomic_int busy = -1;


SDL_Event e;
bool mainloop = true;
bool focus = true;
Vector2 mouse;
const bool * keystates = SDL_GetKeyboardState(NULL);
SDL_Color MainColor = {215, 55, 37, 255};
Uint32 mousebitmask;
bool darkmode = true;



std::string key = "4613~JW2h7AZXUK6LBkEnKxtPtmrJ22cUeUVGyfYCUhnuNNKat46nVAVN9ufUvaFUxvZA";
std::string CourseBase = "https://creanlutheran.instructure.com/api/v1/"; // conversations, courses


const int tcount = 7;


float deltime = 0;
float then = 0;
float now = 0;


struct item{
    std::string name;
    int id;
    std::string stamp;
};


struct course{
    std::string name;
    int id;
    std::vector<item> assignments;
};


struct user{
    std::string name;
    int id;
    std::vector<course> courses;
};


user muser;


size_t write(char * ptr, size_t size, size_t nmemb, std::string * data) {
    data->append(ptr, size * nmemb);
    return size * nmemb;
}


std::string command(const char * cmd){
    std::string returnv;
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &returnv);
    curl_easy_setopt(handle, CURLOPT_URL, cmd);
    curl_easy_perform(handle);
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
    return {name, id, std::format("{:%FT%TZ}", std::chrono::system_clock::now())};
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
    return {splice(split(returnv[1], ":", 1)[1], 1, 1), std::stoi(split(returnv[0], ":", 1)[1])};
}


size_t userwrite(char * ptr, size_t size, size_t nmemb, user * tuser, int coursenum){
    std::string returnv;
    returnv.append(ptr, size * nmemb);
    tuser->courses[coursenum].assignments.push_back(ToItem(returnv));
    return size * nmemb;
}


void FirStage(){
    std::string value = splice(command((CourseBase + "courses").c_str()), 1, 1);
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
        muser.courses.push_back(ToCourse(RawCourses[i]));
    }
}


void SecondStage(int coursenum, int thread){
    int page = 1;
    std::string value = "";
    while (true){
        std::string next = splice(command((CourseBase + "courses/" + std::to_string(muser.courses[coursenum].id) + "/assignments?per-page=100&page=" + std::to_string(page)).c_str()), 1, 1);

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
        if (busy.load() != -1){
            busy.store(thread);
            muser.courses[coursenum].assignments = courseitems;
            std::cout << muser.courses[coursenum].assignments.size() << " assignments in " << muser.courses[coursenum].name << std::endl;
            busy.store(-1);
            break;
        }
    }
}


void CanvasThread(){

    FirStage();
    std::cout << "Collected all courses" << std::endl;
    std::vector<std::thread> threads;
    int courses = 0;
    for (int i = 0; i < tcount; i++){
        if (courses < muser.courses.size()){
            threads.emplace_back(SecondStage, courses);
        }
    }
    while (courses < muser.courses.size()){
        for (int i = 0; i < tcount; i++){
            if (threads[i].joinable()){
                threads[i].join();
                if (courses < muser.courses.size()){
                    std::thread t(SecondStage, courses, i);
                    // threads[i] = t;
                    courses++;
                }
            }
        }
    }
    for (int i = 0; i < muser.courses.size(); i++){
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


    if (os == 1){
        rpath = "./";
        SDL_SetHint(SDL_HINT_RENDER_DRIVER, "metal");
    }
    elif (os == 2){
        rpath = "../Resources/";
    }


    // std::filesystem::path stored = mainpath+".stored";

    // if (std::filesystem::exists(stored)){
    //     std::ifstream file(stored);
    //     file.close();
    // }
    // elif(!std::filesystem::exists(mainpath)){
    //     std::filesystem::create_directory(mainpath);
    // }
    // std::fstream file(stored, std::ios::out);


    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, ("Authorization: Bearer "+key).c_str());
    curl_global_init(CURL_GLOBAL_ALL);
    handle = curl_easy_init();
    curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write);



    /* Initialize SDL, create window and renderer */
    std::cout << "Initializing SDL3" << std::endl;
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer("Canvas View", windowsize.x, windowsize.y, SDL_WINDOW_RESIZABLE, &Window, &Render);
    SDL_SetWindowMinimumSize(Window, 960, 540);
    SDL_SetRenderVSync(Render, 1);
    SDL_SetRenderDrawBlendMode(Render, SDL_BLENDMODE_BLEND);
    SDL_StartTextInput(Window);
    std::cout << "Successfully initialized SDL3" << std::endl;



    ui UI = ui(Render, 2, 3, 8, 8, 16);



    /* Initialize SDL_ttf, create font object */
    TTF_Init();
    TTF_Font * font = TTF_OpenFont((SDL_GetBasePath()+rpath+"Font.ttf").c_str(), 12);


    /* Init text assistant :) */
    TextCharacters CharactersB = TextCharacters(Render, font, "");
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
                    SDL_GetWindowSize(Window, &windowsize.x, &windowsize.y);
                    WindowSize.x = windowsize.x;
                    WindowSize.y = windowsize.y;
                    Title.Position = {(float)windowsize.x/2, (float)windowsize.y/2};
                    Intro.Position = {(float)windowsize.x/2, (float)windowsize.y/2};


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
            SDL_SetWindowTitle(Window, (Title.Text).c_str());
        }
        else{
            SDL_SetWindowTitle(Window, "Canvas View - Log In");
        }


        UI.Render(Render, WindowSize);
        Title.Render(Render, deltime);
        Intro.Render(Render);


        /* Push render content */
        SDL_RenderPresent(Render);
        SDL_SetRenderDrawColor(Render, (Uint8)255*(1-darkmode), (Uint8)255*(1-darkmode), (Uint8)255*(1-darkmode), 255);
        SDL_RenderClear(Render);


        /* Wait if unfocussed */
        if (!focus) SDL_Delay(250);
    }


    /* Exit properly */
    loop.store(false);
    SDL_DestroyRenderer(Render);
    SDL_StopTextInput(Window);
    SDL_DestroyWindow(Window);
    SDL_Quit();
    canvas.join();
    curl_easy_cleanup(handle);
    return 0;
}


#ifdef _WIN32
/* Windows window subsystem :( */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
    return main(__argc, __argv);
}
#endif
