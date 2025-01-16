#pragma once
#include "TextHelp.h"

class ui{
public:
ui(SDL_Renderer * Renderer, int rows, int collumns, int width, int margin, int radius);
bool Render(SDL_Renderer * Renderer, Vector2 WindowSize);
int Rows;
int Collumns;
int Margin;
bool SetWidth(SDL_Renderer * Renderer, int width);
bool SetRadius(SDL_Renderer * Renderer, int radius);
SDL_Color Color = {215, 55, 37, 255};
private:
int Width;
int Radius;
SDL_Texture * Corner;
bool RenderSpace(SDL_Renderer * Renderer, SDL_FRect rect);
};


inline ui::ui(SDL_Renderer * Renderer, int rows, int collumns, int width, int margin, int radius){
    Rows = rows;
    Collumns = collumns;
    Width = width;
    Margin = margin;
    Radius = radius;
    Corner = SDL_CreateTexture(Renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, radius, radius);
    SDL_SetRenderTarget(Renderer, Corner);
    SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 0);
    SDL_RenderClear(Renderer);
    Vector2 Pos;
    float alpha;
    for (int y = 0; y < Radius; y++){
        for (int x = 0; x < Radius; x++){
            Pos = {(float)x, (float)y};
            float z = (Pos+.5).Magnitude();
            alpha = 0;
            if (z < Width){
                alpha = z - Width + 1;
            }
            elif (Width < z && z < Radius){
                alpha = 1;
            }
            elif (Radius < z < Radius + 1){
                alpha = (Radius + 1) - z;
            }
            SDL_SetRenderDrawColor(Renderer, Color.r, Color.g, Color.b, alpha * 255);
            SDL_RenderPoint(Renderer, x, y);
        }
    }
    SDL_SetRenderTarget(Renderer, NULL);
}


inline bool ui::SetWidth(SDL_Renderer * Renderer, int width){
    Width = width;
    SDL_DestroyTexture(Corner);
    Corner = SDL_CreateTexture(Renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, Radius, Radius);
    SDL_SetRenderTarget(Renderer, Corner);
    SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 0);
    SDL_RenderClear(Renderer);
    Vector2 Pos;
    float alpha;
    for (int y = 0; y < Radius; y++){
        for (int x = 0; x < Radius; x++){
            Pos = {(float)x, (float)y};
            float z = (Pos+.5).Magnitude();
            alpha = 0;
            if (z < Width){
                alpha = z - Width + 1;
            }
            elif (Width < z && z < Radius){
                alpha = 1;
            }
            elif (Radius < z < Radius + 1){
                alpha = (Radius + 1) - z;
            }
            SDL_SetRenderDrawColor(Renderer, Color.r, Color.g, Color.b, alpha * 255);
            SDL_RenderPoint(Renderer, x, y);
        }
    }
    SDL_SetRenderTarget(Renderer, NULL);
    return true;
}


inline bool ui::SetRadius(SDL_Renderer * Renderer, int radius){
    Radius = radius;
    SDL_DestroyTexture(Corner);
    Corner = SDL_CreateTexture(Renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, Radius, Radius);
    SDL_SetRenderTarget(Renderer, Corner);
    SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 0);
    SDL_RenderClear(Renderer);
    Vector2 Pos;
    float alpha;
    for (int y = 0; y < Radius; y++){
        for (int x = 0; x < Radius; x++){
            Pos = {(float)x, (float)y};
            float z = (Pos+.5).Magnitude();
            alpha = 0;
            if (z < Width){
                alpha = z - Width + 1;
            }
            elif (Width < z && z < Radius){
                alpha = 1;
            }
            elif (Radius < z < Radius + 1){
                alpha = (Radius + 1) - z;
            }
            SDL_SetRenderDrawColor(Renderer, Color.r, Color.g, Color.b, alpha * 255);
            SDL_RenderPoint(Renderer, x, y);
        }
    }
    SDL_SetRenderTarget(Renderer, NULL);
    return true;
}


inline bool ui::RenderSpace(SDL_Renderer * Renderer, SDL_FRect rect){
    rect = {(float)(int)rect.x, (float)(int)rect.y, (float)(int)rect.w, (float)(int)rect.h};
    SDL_FRect temprect = {rect.x, rect.y, (float)Radius, (float)Radius};
    SDL_RenderTextureRotated(Renderer, Corner, NULL, &temprect, 180, NULL, SDL_FLIP_NONE);
    temprect = {rect.x + rect.w - Radius, rect.y, (float)Radius, (float)Radius};
    SDL_RenderTextureRotated(Renderer, Corner, NULL, &temprect,-90, NULL, SDL_FLIP_NONE);
    temprect = {rect.x + rect.w - Radius, rect.y + rect.h - Radius, (float)Radius, (float)Radius};
    SDL_RenderTextureRotated(Renderer, Corner, NULL, &temprect, 0, NULL, SDL_FLIP_NONE);
    temprect = {rect.x, rect.y + rect.h - Radius, (float)Radius, (float)Radius};
    SDL_RenderTextureRotated(Renderer, Corner, NULL, &temprect, 90, NULL, SDL_FLIP_NONE);

    temprect = (SDL_FRect){rect.x + Radius, rect.y, rect.w - (Radius * 2), (float)Width};
    SDL_RenderFillRect(Renderer, &temprect);
    temprect = (SDL_FRect){rect.x, rect.y + Radius, (float)Width, rect.h - (Radius * 2)};
    SDL_RenderFillRect(Renderer, &temprect);
    temprect = (SDL_FRect){rect.x + rect.w - Radius + Width, rect.y + Radius, (float)Width, rect.h - (Radius * 2)};
    SDL_RenderFillRect(Renderer, &temprect);
    temprect = (SDL_FRect){rect.x + Radius, rect.y + rect.h - Width, rect.w - (Radius * 2), (float)Width};
    SDL_RenderFillRect(Renderer, &temprect);
    return true;
}


inline bool ui::Render(SDL_Renderer * Renderer, Vector2 WindowSize){
    SDL_SetRenderDrawColor(Renderer, Color.r, Color.g, Color.b, Color.a);
    WindowSize -= Vector2(Margin, Margin);
    // SDL_FRect temp = {0, 0, (float)Radius, (float)Radius};
    // SDL_RenderTexture(Renderer, Corner, NULL, &temp);
    for (int y = 0; y < Rows; y++){
        for (int x = 0; x < Collumns; x++){
            RenderSpace(Renderer, {((float)x/Collumns)*WindowSize.x + Margin, ((float)y/Rows)*WindowSize.y + Margin, (WindowSize.x/Collumns) - Margin, (WindowSize.y/Rows) - Margin});
        }
    }
    return true;
}
