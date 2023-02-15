#ifndef VISUALS_H
#define VISUALS_H
#include "../../include/rawio.h"
#include <stdint.h>
#include <math.h>

struct vertex {
    int x;
    int y;
};

struct vertB {
    int16_t x;
    int16_t y;
    uint8_t bright;
};

void drawSquare(int16_t x, int16_t y, int16_t brightness, int size);
void drawRect(int16_t x, int16_t y, int width_x, int width_y, int16_t brightness);
void drawChar(const char c, int16_t brightness, int size);
void drawString(const char* string, int16_t brightness, int size);
vertex rotateVert(int x, int y, float angle);

class sprite {
    public:
        vertB* renderedFrame;
        sprite(size_t numFrames_init, const size_t* numVertices_init, const vertB** frames_init)
            : renderedFrame(nullptr),renderToMem(0),numFrames(numFrames_init), numVertices(numVertices_init), frames(frames_init) {
                selectFrame(0);
            }

        // Use this version if you want to modify rendered frame before actually drawing it to screen
        // renderedFrame_init needs to be large enough to accomodate the largest frame
        sprite(size_t numFrames_init, const size_t* numVertices_init, const vertB** frames_init, vertB* renderedFrame_init)
            : renderedFrame(renderedFrame_init),renderToMem(1),numFrames(numFrames_init), numVertices(numVertices_init), frames(frames_init) {
                selectFrame(0);
            }
        
        void render(int16_t xStart, int16_t yStart) {
            draw_absolute_vector(xStart,yStart,0);
            load_abs_pos(xStart,yStart);
            size_t currNumVertices = numVertices[currFrame];
            for (size_t v=0; v<currNumVertices; v++) {
                const vertB& vert = frames[currFrame][v];
                draw_relative_vector(vert.x,vert.y,vert.bright);
            }
        }

        void rotatedRender(int16_t xStart, int16_t yStart, float angle) {
            // according to how sprite creator works, first frame is always a blank
            int16_t unrotatedAbsX=0;
            int16_t unrotatedAbsY=0;
            int16_t rotatedAbsX;
            int16_t rotatedAbsY;
            for (size_t v=0; v<currNumVertices; v++) {
                const vertB& vert = frames[currFrame][v];
                unrotatedAbsX += vert.x;
                unrotatedAbsY += vert.y;
                rotatedAbsX = unrotatedAbsX*cos(angle) - unrotatedAbsY*sin(angle);
                rotatedAbsY = unrotatedAbsX*sin(angle) + unrotatedAbsY*cos(angle);
                if (renderToMem) {
                    addToMem(rotatedAbsX+xStart,rotatedAbsY+yStart,vert.bright,v);
                } else {
                    draw_absolute_vector(rotatedAbsX+xStart,rotatedAbsY+yStart,vert.bright);
                }
            }
        }

        void addToMem(int16_t x, int16_t y, int8_t bright, size_t index) {
            vertB renderedVert;
            renderedVert.x = x;
            renderedVert.y = y;
            renderedVert.bright = bright;
            renderedFrame[index] = renderedVert;
        }

        void drawFromMem() {
            if (!renderToMem) {
                sendString("Dingus! You didn't give an array to render to in this sprite's constructor!");
                return;
            }
            for (size_t v=0; v<currNumVertices; v++) {
                const vertB& vert = renderedFrame[v];
                draw_absolute_vector(vert.x,vert.y,vert.bright);
            }
        }

        void incrementFrame() {
            currFrame++;
            currFrame%=numFrames;
            currNumVertices = numVertices[currFrame];
        }

        void selectFrame(size_t index) {
            currFrame=(index%numFrames);
            currNumVertices = numVertices[currFrame];
        }

        size_t getCurrNumVertices() {
            return currNumVertices;
        }

    private:
        int renderToMem;
        const size_t numFrames;
        const size_t* numVertices;
        const vertB** frames;
        size_t currFrame;
        size_t currNumVertices;
};
#endif