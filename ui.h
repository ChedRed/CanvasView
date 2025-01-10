#pragma once
#include "TextHelp.h"
#include "Vector2.h"

class ui{
public:
ui(int rows, int collumns, int width,  int margin, int radius);
bool Render(SDL_Renderer * Renderer, Vector2 WindowSize);
int Rows;
int Collumns;
int Margin;
bool SetWidth(int width);
bool SetRadius(int radius);
SDL_Color Color = {215, 55, 37, 255};
private:
int Width;
int Radius;
SDL_Texture * Corner;
};


inline ui::ui(int rows, int collumns, int width, int margin, int radius){
    Rows = rows;
    Collumns = collumns;
    Width = width;
    Margin = margin;
    Radius = radius;
}


inline bool ui::SetWidth(int width){

}


inline bool ui::SetRadius(int width){

}


inline bool ui::Render(SDL_Renderer * Renderer, Vector2 WindowSize){
    SDL_FRect temprect = {(float)Width, (float)Width, (float)Width, (float)Width};
    SDL_RenderTextureRotated(Renderer, Corner, NULL, &temprect, 0, NULL, SDL_FLIP_NONE);
    temprect = {(float)Width * 2, (float)Width * 2, (float)Width, (float)Width};
    SDL_RenderTextureRotated(Renderer, Corner, NULL, &temprect, 0, NULL, SDL_FLIP_NONE);
    temprect = (SDL_FRect){WindowSize.x - ((float)Width * 2), (float)Width, (float)Width, (float)Width};
    SDL_RenderTextureRotated(Renderer, Corner, NULL, &temprect, 90, NULL, SDL_FLIP_NONE);
    temprect = (SDL_FRect){WindowSize.x - ((float)Width * 3), (float)Width * 2, (float)Width, (float)Width};
    SDL_RenderTextureRotated(Renderer, Corner, NULL, &temprect, 90, NULL, SDL_FLIP_NONE);
    temprect = (SDL_FRect){WindowSize.x - ((float)Width * 2), WindowSize.y - ((float)Width * 2), (float)Width, (float)Width};
    SDL_RenderTextureRotated(Renderer, Corner, NULL, &temprect, 180, NULL, SDL_FLIP_NONE);
    temprect = (SDL_FRect){WindowSize.x - ((float)Width * 3), WindowSize.y - ((float)Width * 3), (float)Width, (float)Width};
    SDL_RenderTextureRotated(Renderer, Corner, NULL, &temprect, 180, NULL, SDL_FLIP_NONE);
    temprect = (SDL_FRect){(float)Width, WindowSize.y - ((float)Width * 2), (float)Width, (float)Width};
    SDL_RenderTextureRotated(Renderer, Corner, NULL, &temprect, 270, NULL, SDL_FLIP_NONE);
    temprect = (SDL_FRect){(float)Width * 2, WindowSize.y - ((float)Width * 3), (float)Width, (float)Width};
    SDL_RenderTextureRotated(Renderer, Corner, NULL, &temprect, 270, NULL, SDL_FLIP_NONE);


    SDL_SetRenderDrawColor(Renderer, Color.r, Color.g, Color.b, Color.a);
    temprect = (SDL_FRect){(float)Width * 2, (float)Width, WindowSize.x - ((float)Width * 4), (float)Width};
    SDL_RenderFillRect(Renderer, &temprect);
    temprect = (SDL_FRect){(float)Width, (float)Width * 2, (float)Width, WindowSize.y - ((float)Width * 4)};
    SDL_RenderFillRect(Renderer, &temprect);
    temprect = (SDL_FRect){WindowSize.x - ((float)Width * 2), (float)Width * 2, (float)Width,  WindowSize.y - ((float)Width * 4)};
    SDL_RenderFillRect(Renderer, &temprect);
    temprect = (SDL_FRect){(float)Width * 2, WindowSize.y - ((float)Width * 2), WindowSize.x - ((float)Width * 4), (float)Width};
    SDL_RenderFillRect(Renderer, &temprect);
    return true;
}
