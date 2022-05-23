#include <unistd.h>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <string>
#include <csignal>
#include <termios.h>
#include <iostream>
#include <SDL2/SDL.h>

#include "../include/rawio.h"

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Joystick* controller = NULL;
SDL_Event event;

void closeSDL() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_JoystickClose(controller);
    SDL_Quit();
    exit(0);
}

struct InputState get_inputs() {
    static struct InputState inputs;
    inputs.buttons = 0;

    while( SDL_PollEvent( &event ) != 0 ) {
        switch (event.type) {
            case SDL_QUIT:
                closeSDL();
                break;
            case SDL_JOYAXISMOTION:
                if (event.jaxis.which == 0) { // controller 0
                    switch (event.jaxis.axis) {
                        case 0: // x
                            inputs.xpos = (event.jaxis.value / 65536.0)+0.5;
                            break;
                        case 1: // y
                            inputs.ypos = (event.jaxis.value / 65536.0)+0.5;
                            break;
                    }
                }
                break;
            case SDL_JOYBUTTONDOWN:
            case SDL_JOYBUTTONUP:
                if (event.jbutton.which == 0) {
                    switch (event.jbutton.button) {
                        case 4: // L1 bumper
                            inputs.buttons = (event.jbutton.state==SDL_PRESSED);
                            break;
                        case 5: // R1 bumper
                            inputs.buttons = (event.jbutton.state==SDL_PRESSED);
                            break;
                    }
                }
                break;
        }
    }
    return inputs;
}


void initialize_input_output() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK)<0) {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        exit(1);
    }
    window = SDL_CreateWindow("HMC Vector Arcade", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1024, 1024, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
        exit(1);
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf( "Renderer could not be created! SDL_Error: %s\n", SDL_GetError() );
        SDL_DestroyWindow(window);
        window = NULL;
        exit(1);
    }
    SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_ADD);
    if(SDL_NumJoysticks() < 1) {
        printf( "Warning: No joysticks connected!\n" );
    } else {
        controller = SDL_JoystickOpen(0);
        if (controller == NULL) {
            printf( "Unable to open game controller! SDL Error: %s\n", SDL_GetError() );
            exit(1);
        }
    }
}

int16_t currX = 0;
int16_t currY = 0;

void draw_buffer_switch() {
    SDL_RenderPresent(renderer);
    SDL_SetRenderDrawColor(renderer,0,0,0,255);
    SDL_RenderClear(renderer);
    //SDL_UpdateWindowSurface(window);
}

void drawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2) {
    SDL_RenderDrawLine(renderer,x1+512,-y1+512,x2+512,-y2+512);
}

void draw_absolute_vector(int16_t x_position, int16_t y_position, int16_t brightness) {
    SDL_SetRenderDrawColor(renderer,200,200,255,brightness);
    drawLine(currX,currY,x_position,y_position);
    currX = x_position;
    currY = y_position;
}

void load_abs_pos(int16_t x_position, int16_t y_position) {
    currX = x_position;
    currY = y_position;
}

void draw_relative_vector(int16_t delta_x, int16_t delta_y, int16_t brightness) {
    SDL_SetRenderDrawColor(renderer,200,200,255,brightness);
    drawLine(currX,currY,currX+delta_x,currY+delta_y);
    currX += delta_x;
    currY += delta_y;
}

void draw_end_buffer() {
}

bool is_halted() {
    // assume vector simulator is always ready to switch buffers
    return true;
}

uint32_t startTime;
uint32_t endTime;
void start_timer(uint32_t milliseconds) {
    startTime = SDL_GetTicks();
    endTime = startTime + milliseconds/10;
}

bool timer_done() {
    return (SDL_GetTicks() > endTime);
}

void printChar(char data) {
    std::cout<<data;
}

void sendString(const char* txStr) {
    std::cout<<txStr;
}

void requestChiptune(chiptuneType type, int soundID) {
    // not implemented in sim
}
