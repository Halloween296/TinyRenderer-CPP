#pragma once
#include <SDL.h>
#include <vector>
#include <iostream>
#include <cstring>
#include <limits>
#include "Maths.hpp"

class Platform {
public:
    std::vector<uint32_t> framebuffer; //ldr
    std::vector<Vec3> hdrbuffer; //hdr
    std::vector<float> zBuffer;

    Platform(const char* title, int width, int height) 
        : width(width), height(height) 
    {
        int reW = width * 2;
        int reH = height * 2;
        //SSAA
        zBuffer.resize(reW * reH);

        if (SDL_Init(SDL_INIT_VIDEO) != 0) {
            throw std::runtime_error("SDLFailed");
        }

        window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                                  width, height, SDL_WINDOW_SHOWN);
        
        // SDL 渲染器 Tex到窗口
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        
        //允许逐帧修改像素数据
        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, 
                                    SDL_TEXTUREACCESS_STREAMING, width, height);

        // 初始化 
        framebuffer.resize(width * height);
        hdrbuffer.resize(reW * reH);
    }

    ~Platform() {
        SDL_DestroyTexture(texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    // 清空屏幕
    void ClearBuffer() {
        std::fill(hdrbuffer.begin(), hdrbuffer.end(), Vec3(0, 0, 0));
        //std::fill(framebuffer.begin(), framebuffer.end(), color);
        std::fill(zBuffer.begin(), zBuffer.end(), std::numeric_limits<float>::infinity());
    }
    
    //画点测试 
    
    //void PutPixel(int x, int y, uint32_t color) {
    //    if (x >= 0 && x < width && y >= 0 && y < height) {
    //        framebuffer[y * width + x] = color;
    //    }
    //}

    void PutPixelHDR(int x,int y,Vec3 color) {
        if (x < 0 || x >= width * 2 || y < 0 || y >= height * 2)return;
        hdrbuffer[y * width * 2 + x] = color;
    }

    // 交换缓冲区rt
    void SwapBuffers() {
        SDL_UpdateTexture(texture, nullptr, framebuffer.data(), width * sizeof(uint32_t));
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
    }

    bool ProcessInput() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) return false;
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) return false;
        }
        return true;
    }

    int GetWidth() const { return width; }
    int GetHeight() const { return height; }
    int GetreW() const { return width * 2; }
    int GetreH() const { return height * 2; }
private:
    int width, height;
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* texture = nullptr;
    
 
};