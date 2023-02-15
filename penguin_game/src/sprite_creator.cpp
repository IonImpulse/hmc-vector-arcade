/* sprite_creator.cpp
 * By Ben Bracker Jun 2022
 * Assists in the creation of 2D vector sprite animations.
 * Intended to only run on a computer, not an arcade machine.
 * Notable depedency: SDL2 (Simple Direct-media Layer 2)
 */

#include <stdint.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <SDL2/SDL.h>
#include <math.h>
#include "../include/rawio.h"
#include "../include/simulated_io.h"
#include "../lib/visuals/visuals.hpp"

/////////////////
// Global Vars //
/////////////////
struct InputState inputs;
uint32_t timeCount=0;

//////////////////
// Helper Stuff //
//////////////////
void makeWindowTransparent(float transparency);
// implemented in simulated_io.cpp

int keyIsPressed(SDL_Scancode keyCode) {
    return inputs.keys[keyCode];
}

int keyJustPressed(SDL_Scancode keyCode) {
    return inputs.keys[keyCode] && !inputs.prevKeys[keyCode];
}

int shiftPressed() {
    return keyIsPressed(SDL_GetScancodeFromKey(SDLK_LSHIFT)) || keyIsPressed(SDL_GetScancodeFromKey(SDLK_RSHIFT));
}

int ctrlPressed() {
    return keyIsPressed(SDL_GetScancodeFromKey(SDLK_LCTRL)) || keyIsPressed(SDL_GetScancodeFromKey(SDLK_RCTRL));
}

int altPressed() {
    return keyIsPressed(SDL_GetScancodeFromKey(SDLK_LALT)) || keyIsPressed(SDL_GetScancodeFromKey(SDLK_RALT));
}

float distance(int dX, int dY) {
    return std::sqrt(dX*dX+dY*dY);
}

class spriteCreatorClass{
    public:
        size_t currVertIndex=0;
        size_t currFrameIndex=0;
        void render(int lineBlink) {
            uint8_t bright;
            vertB& lastV = currFrame->at(currFrame->size()-1);
            draw_absolute_vector(lastV.x*zoom,lastV.y*zoom,0);
            for (size_t iv=0; iv<currFrame->size(); iv++) {
                vertB& currVert = currFrame->at(iv);
                bright = (lineBlink && (iv==currVertIndex)) ? (currVert.bright+0x7F)%0xFF : currVert.bright;
                bright = (iv==0 && shapeOpen) ? 0 : bright;
                draw_absolute_vector(currVert.x*zoom,currVert.y*zoom,bright);
            }
        }

        void renderOnion(std::vector<vertB>* frame) {
            vertB& lastV = frame->at(frame->size()-1);
            draw_absolute_vector(lastV.x*zoom,lastV.y*zoom,0);
            for (size_t iv=0; iv<frame->size(); iv++) {
                vertB& currVert = frame->at(iv);
                uint8_t bright = (iv==0 && shapeOpen) ? 0 : currVert.bright;
                draw_absolute_vector(currVert.x*zoom,currVert.y*zoom,bright>>2);
            }
        }

        void renderPrevOnion() {
            std::vector<vertB>* prevFrame = &frames.at((currFrameIndex+frames.size()-1)%frames.size());
            renderOnion(prevFrame);
        }

        void renderNextOnion() {
            std::vector<vertB>* nextFrame = &frames.at((currFrameIndex+1)%frames.size());
            renderOnion(nextFrame);
        }

        void renderAllOnion() {
            for (size_t frameIndex=0; frameIndex<frames.size(); frameIndex++) {
                if (frameIndex!=currFrameIndex) {
                    std::vector<vertB>* frame = &frames.at(frameIndex);
                    renderOnion(frame);
                }
            }
        }


        void zoomIn(float zShiftAmt) {
            zoom += zShiftAmt;
            zoom = (zoom>1.0) ? 1.0 : zoom;
        }
        void zoomOut(float zShiftAmt) {
            zoom -= zShiftAmt;
            zoom = (zoom<0.2) ? 0.2 : zoom;
        }
        float zoomLvl() {return zoom;}

        size_t numVertices() {
            return currFrame->size();
        }

        void insertVertex(vertB vert) {
            currFrame->insert(currFrame->begin()+currVertIndex,vert);
            currVertIndex++;
            currVert = &currFrame->at(currVertIndex);
        }

        void selectVertex(size_t iv) {
            currVertIndex = iv%currFrame->size();
            currVert = &currFrame->at(currVertIndex);
        }

        void selectPrevVertex() {
            selectVertex(currVertIndex+currFrame->size()-1);
        }

        void selectNextVertex() {
            selectVertex(currVertIndex+1);
        }

        void selectNearestVertex(int x, int y) {
            float minDist = INFINITY;
            size_t minDistInd = 0;
            for (size_t iv=0; iv<currFrame->size(); iv++) {
                float dist = distance(x-currFrame->at(iv).x, y-currFrame->at(iv).y);
                if (dist<minDist) {
                    minDist = dist;
                    minDistInd = iv;
                }
            }
            selectVertex(minDistInd);
        }

        void selectFrame(size_t frameIndex) {
            currFrameIndex = frameIndex%frames.size();
            currFrame = &frames.at(currFrameIndex);
            selectVertex(currVertIndex);
        }

        void selectPrevFrame() {
            selectFrame(currFrameIndex+frames.size()-1);
        }

        void selectNextFrame() {
            selectFrame(currFrameIndex+1);
        }

        void setVertex(vertB vert) {
            *currVert = vert;
        }
        
        void setVertexPos(int x, int y) {
            currVert->x = x;
            currVert->y = y;
        }

        void setVertexBright(uint8_t bright) {
            currVert->bright = bright;
        }

        vertB getVertex() {
            return *currVert;
        }

        void deleteVertex() {
            if (currFrame->size()>1) {
                currFrame->erase(currFrame->begin()+currVertIndex);
                currVertIndex-=1;
                currVertIndex%=currFrame->size();
            }
        }

        void moveAllVertices(int dX, int dY) {
            for (size_t iv=0; iv<currFrame->size(); iv++) {
                currFrame->at(iv).x += dX;
                currFrame->at(iv).y += dY;
            }
        }

        void moveAllVerticesInAllFrames(int dX, int dY) {
            for (size_t f=0; f<frames.size(); f++) {
                size_t numVert = frames.at(f).size();
                for (size_t currVert=0; currVert<numVert; currVert++) {
                    frames.at(f).at(currVert).x += dX;
                    frames.at(f).at(currVert).y += dY;
                }
            }
        }

        void chooseFirstVertex() {
            std::vector<vertB> tmpFrame;
            for (size_t iv=currVertIndex; iv<currFrame->size()+currVertIndex; iv++) {
                tmpFrame.push_back(currFrame->at(iv % currFrame->size()));
            }
            for (size_t iv=0; iv<currFrame->size(); iv++) {
                currFrame->at(iv) = tmpFrame.at(iv);
            }
            selectVertex(0);
        }

        void chooseFirstFrame() {
            std::vector<std::vector<vertB>> tmpFrames;
            for (size_t frameIndex=currFrameIndex; frameIndex<frames.size()+currFrameIndex; frameIndex++) {
                tmpFrames.push_back(frames.at(frameIndex % frames.size()));
            }
            for (size_t frameIndex=0; frameIndex<frames.size(); frameIndex++) {
                frames.at(frameIndex) = tmpFrames.at(frameIndex);
            }
            selectFrame(0);
        }

        void swapCurrAndNextFrame() {
            if (currFrameIndex<(frames.size()-1)) {
                std::vector<vertB> tmpFrame;
                tmpFrame = *currFrame;
                frames.at(currFrameIndex) = frames.at(currFrameIndex+1);
                frames.at(currFrameIndex+1) = tmpFrame;
                selectFrame(currFrameIndex+1);
            }
        }

        void swapCurrAndPrevFrame() {
            if (currFrameIndex>0) {
                std::vector<vertB> tmpFrame;
                tmpFrame = *currFrame;
                frames.at(currFrameIndex) = frames.at(currFrameIndex-1);
                frames.at(currFrameIndex-1) = tmpFrame;
                selectFrame(currFrameIndex-1);
            }
        }

        void reverseVertexOrdering() {
            std::vector<vertB> tmpFrame;
            for (size_t iv=0; iv<currFrame->size(); iv++) {
                tmpFrame.push_back(currFrame->at(currFrame->size()-iv-1));
            }
            for (size_t iv=0; iv<currFrame->size(); iv++) {
                currFrame->at(iv) = tmpFrame.at(iv);
            }
        }

        void reverseFrameOrdering() {
            std::vector<std::vector<vertB>> tmpFrames;
            for (size_t framedIndex=0; framedIndex<frames.size(); framedIndex++) {
                tmpFrames.push_back(frames.at(frames.size()-framedIndex-1));
            }
            for (size_t framedIndex=0; framedIndex<frames.size(); framedIndex++) {
                frames.at(framedIndex) = tmpFrames.at(framedIndex);
            }
        }

        size_t numFrames() {
            return frames.size();
        }

        void newFrame() {
            frames.insert(frames.begin()+currFrameIndex,*currFrame);
            currFrameIndex++;
            currFrame = &frames.at(currFrameIndex);
        }

        void toggleShapeClosure() {
            shapeOpen ^= 1;
        }

        void deleteFrame() {
            if (frames.size()>1) {
                frames.erase(frames.begin()+currFrameIndex);
                currFrameIndex-=1;
                currFrameIndex%=frames.size();
            }
        }

        void loadBinary(const char* spriteBinPathInput) {
            // Check if file exists
            std::string spriteBinPath = spriteBinPathInput;
            size_t binPos = spriteBinPath.rfind(".bin");
            if (binPos==std::string::npos) {
                std::cerr << "Warning: was expecting sprite binary " << spriteBinPath << " to end in \".bin\"\n";
            }
            FILE *fp = fopen(spriteBinPathInput, "rb");
            if(fp == NULL){
                std::cerr << "Load failed: file " << spriteBinPathInput << " not found!\n";
                return;
            }
            // Cool it exists
            // Set sprite name and folder accordingly
            size_t folderPos = spriteBinPath.rfind("/");
            if (folderPos == std::string::npos) {
                spriteName = spriteBinPath.substr(0,binPos);
                spriteFolder = "";
                std::cout << "Found sprite " << spriteName << " in folder ./ \n";
            } else {
                folderPos += 1;
                spriteName = spriteBinPath.substr(folderPos,binPos-folderPos);
                spriteFolder = spriteBinPath.substr(0,folderPos);
                std::cout << "Found sprite " << spriteName << " in folder " << spriteFolder <<"\n";
            }
            // Now attempt load
            frames.clear();
            int success=1;
            size_t numFrames;
            success &= fread(&numFrames,sizeof(size_t),1,fp);
            for (size_t f=0; f<numFrames; f++) {
                std::vector<vertB> frame;
                frame.clear();
                size_t numVertices;
                success &= fread(&numVertices,sizeof(size_t),1,fp);
                for (size_t currVert=0; currVert<numVertices; currVert++) {
                    vertB vert;
                    success &= fread(&vert,sizeof(vertB),1,fp);
                    frame.push_back(vert);
                }
                frames.push_back(frame);
            }
            success &= fread(&shapeOpen,sizeof(shapeOpen),1,fp);
            shapeOpen &= 0x1;
            success &= fread(&zoom,sizeof(zoom),1,fp);
            fclose(fp);
            if (!success) {
                printf("Warning: read errors detected.\n");
            }
            std::cout << "Loaded sprite " << spriteName << "!\n";
            hasName=1;
        }

        void save() {
            if (!hasName) {
                saveAs();
            } else {
                _saveText();
                _saveBinary();
            }
        }

        void saveAs() {
            draw_absolute_vector(-350,200,0);
            drawString("please see terminal to give sprite name",0xC0,3);
            draw_buffer_switch();
            printf("Enter folder to save to (blank for cwd): ");
            std::getline(std::cin, spriteFolder);
            printf("Enter sprite name: ");
            std::cin >> spriteName;
            if (spriteFolder.length() == 0) spriteFolder = "./";
            if (spriteFolder.back() != '/') spriteFolder.push_back('/');
            _saveText();
            _saveBinary();
            hasName=1;
        }

    private:
        std::vector<std::vector<vertB>> frames {
            { {50,50,0xC0}, {50,-50,0xC0}, {-50,-50,0xC0}, {-50,50,0xC0} },
            { {45,45,0xC0}, {45,-45,0xC0}, {-45,-45,0xC0}, {-45,45,0xC0} },
            { {40,40,0xC0}, {40,-40,0xC0}, {-40,-40,0xC0}, {-40,40,0xC0} }
        };
        std::vector<vertB>* currFrame = &frames.at(0);
        vertB* currVert = &currFrame->at(0);
        int shapeOpen=1;
        float zoom=1;
        std::string spriteName;
        std::string spriteFolder;
        int hasName=0;

        void _saveText() {
            std::string fileName;
            fileName.append(spriteFolder);
            fileName.append(spriteName);
            fileName.append(".txt");
            const char* spritename = spriteName.c_str();
            const char* filename = fileName.c_str();
            FILE *fp = fopen(filename, "w");
            if (fp == NULL) {
                printf("Save failed!\nCannot open %s with write permissions.\n", filename);
                return;
            }
            fprintf(fp, "const size_t %s_numVertices[%lu] = {", spritename,frames.size());
            size_t maxFrameSize = 0;
            for (size_t f=0; f<frames.size(); f++) {
                size_t frameSize = frames.at(f).size()+(shapeOpen==0);
                fprintf(fp,"%lu",frameSize);
                maxFrameSize = (frameSize > maxFrameSize) ? frameSize : maxFrameSize;
                if (f==frames.size()-1) fprintf(fp, "};\n");
                else fprintf(fp, ", ");
            }
            // need to convert from absolute encoding to relative encoding so that sprite
            // can be easily relocated during game
            for (size_t f=0; f<frames.size(); f++) {
                size_t numVert = frames.at(f).size();
                int prevX = 0;
                int prevY = 0;
                // always adds a blank line from (0,0) to first real vertex
                // conditionally adds a line from last vertex to first vertex based on shapeOpen
                size_t frameSize = numVert+(shapeOpen==0);
                fprintf(fp, "const vertB %s_f%lu[%lu] = {", spritename,f,frameSize);
                for (size_t currVert=0; currVert<frameSize; currVert++) {
                    const vertB& vert = frames.at(f).at(currVert%numVert);
                    fprintf(fp, "{%d,%d,0x%x}", (int)((vert.x-prevX)*zoom), (int)((vert.y-prevY)*zoom), (currVert==0) ? 0 : vert.bright);
                    prevX = vert.x;
                    prevY = vert.y;
                    if (currVert==(frameSize-1)) fprintf(fp,"};\n");
                    else fprintf(fp, ", ");
                }
            }
            fprintf(fp, "const vertB* %s_frames[%lu] = {", spritename,frames.size());
            for (size_t f=0; f<frames.size(); f++) {
                fprintf(fp, "%s_f%lu", spritename,f);
                if (f==frames.size()-1) fprintf(fp,"};\n");
                else fprintf(fp,", ");
            }
            fprintf(fp, "sprite %s(%lu,%s_numVertices,%s_frames);\n", spritename,frames.size(),spritename,spritename);
            fprintf(fp, "// Comment out the above line and uncomment the below lines if you want the render buffer.\n");
            fprintf(fp, "// If you use the render buffer, be sure to call first call rotatedRender(),\n");
            fprintf(fp, "// then perform your desired modifications to renderedFrame,\n");
            fprintf(fp, "// then lastly call drawFromMem()\n");
            fprintf(fp, "//\n");
            fprintf(fp, "// vertB %s_renderedFrame[%lu];\n", spritename,maxFrameSize);
            fprintf(fp, "// sprite %s(%lu,%s_numVertices,%s_frames,%s_renderedFrame);\n", spritename,frames.size(),spritename,spritename,spritename);
            fclose(fp);
            printf("Saved text %s\n", filename);
        }

        void _saveBinary() {
            std::string fileName;
            fileName.append(spriteFolder);
            fileName.append(spriteName);
            fileName.append(".bin");
            const char* filename = fileName.c_str();
            FILE* fp = fopen(filename, "wb");
            if (fp == NULL) {
                printf("Save failed!\n Cannot open %s with write permissions.", filename);
                return;
            }
            size_t numFrames = frames.size();
            fwrite(&numFrames,sizeof(size_t),1,fp);
            for (size_t f=0; f<numFrames; f++) {
                size_t numVertices = frames.at(f).size();
                fwrite(&numVertices,sizeof(size_t),1,fp);
                for (size_t currVert=0; currVert<numVertices; currVert++) {
                    fwrite(&frames.at(f).at(currVert),sizeof(vertB),1,fp);
                }
            }
            fwrite(&shapeOpen,sizeof(shapeOpen),1,fp);
            fwrite(&zoom,sizeof(zoom),1,fp);
            fclose(fp);
            printf("Saved binary %s\n", filename);
        }
};

spriteCreatorClass spriteCreator;

void printHelp() {
    printf(
        "========================= How to Use Sprite Creator ============================\n"
        "# INVOCATION\n"
        "    ./sprite_creator [<path>/<sprite>.bin]\n"
        "    Give an existing sprite binary in the commandline to automatically load it.\n"
        "\n"
        "# CONTROLS\n"
        "  ---- File I/O ----\n"
        "    Save                     CTRL-S\n"
        "    Save As                  CTRL-SHIFT-S\n"
        "    Load sprite              CTRL-O\n"
        "    Exit                     CTRL-Q\n"
        "  ---- Basic Sprite Editing ----\n"
        "    Select frame             Q,E\n"
        "    Insert frame             +\n"
        "    Delete frame             -\n"
        "    Select vertex            A,D or mouse move\n"
        "    Insert vertex            W or mouse right click\n"
        "    Delete vertex            X or mouse middle click\n"
        "    Move vertex              arrow keys or mouse left click\n"
        "      - quickly              SHIFT-(arrow keys)\n"
        "      - all vertices         \n"
        "          - in curr frame    CTRL-(arrow keys)\n"
        "          - in all frames    ALT-(arrow keys)\n"
        "    Change brightness        9,0 or mouse scroll\n"
        "      - quickly              SHIFT-(9,0)\n"
        "  ---- Vertex/Frame Ordering ----\n"
        "    Reverse vertex ordering  R\n"
        "    Reverse frame ordering   CTRL-R\n"
        "    Set first vertex         F\n"
        "    Set first frame          CTRL-F\n"
        "    Swap curr and next frame N\n"
        "    Swap curr and prev frame SHIFT-N\n"
        "    (Dis)connect first       \n"
        "      and last vertices      C\n"
        "  ---- Editor Appearance ----\n"
        "    Window transparency      T\n"
        "    Toggle crosshairs        Y\n"
        "    Toggle onionskinning     O\n"
        "    Zoom Out/In              [,] or SHIFT+mouse scroll\n"
        "      - quickly              SHIFT-([,])\n"
        "\n# ADVICE\n"
        "    Adding a new frame creates a copy of the currently selected frame.\n"
        "    So first make a 2D drawing in frame 1.\n"
        "    Test it in-game; make sure it's really good!\n"
        "    Only then add new frames to animate the sprite.\n"
        "\n"
        "    Use mouse left-click to get a vertex in generally the right spot.\n"
        "    Then use arrow keys to fine-tune vertex's position.\n"
        "\n"
        "    Set brightness to 0 to draw invisible lines.\n"
        "\n"
        "    The window is adjustably transparent so that you can trace\n"
        "    GIFs or videos you place behind it.\n"
        "\n"
        "    Saving a sprite named \"foo\" will create \"foo.bin\" and \"foo.txt\".\n"
        "    Use the binary to reload the sprite into the sprite creator.\n"
        "    Use the text file to add the sprite to game code.\n"
        "\n"
        "    Zooming affects only the text output, not the binary output.\n"
        "    Do design work in 1:1 zoom, and if needed, zoom out before saving\n"
        "    to create a small sprite in the text file.\n"
        "\n"
        "    There is no undo!\n"
        "    Practice making a modest sprite before creating a masterpiece.\n"
        "    And save often!\n"
        "================================================================================\n");
}

//////////////
// GUI Code //
//////////////

void updateGame() {
    static float transparency = 0.8;
    enum crosshairEnum {NO_HAIR,TWO_HAIR,FOUR_HAIR};
    static crosshairEnum crosshairs = TWO_HAIR;
    enum onionEnum {NO_ONION,PREV_ONION,NEXT_ONION,BOTH_ONION,ALL_ONION};
    static onionEnum onion = PREV_ONION;
    char miscText[100];
    vertB currVert = spriteCreator.getVertex();

    // ----- FILE I/O -----

    // Quit with CTRL-Q
    if (keyJustPressed(SDL_SCANCODE_Q)) {
        if (ctrlPressed()) {
            closeSDL();
        }
    }

    // Open with CTRL-O
    if (keyJustPressed(SDL_SCANCODE_O)) {
        if (ctrlPressed()) {
            draw_absolute_vector(-350,200,0);
            drawString("please see terminal to give sprite path",0xC0,3);
            draw_buffer_switch();
            printf("Location to load binary from (include \".bin\"): ");
            char filename[200];
            std::cin >> filename;
            spriteCreator.loadBinary(filename);
        }
    }

    if (keyJustPressed(SDL_SCANCODE_S)) {
        if (ctrlPressed()) {
            if (shiftPressed()) {
                // CTRL-SHIFT-S: save as
                spriteCreator.saveAs();
            } else {
                // CTRL-S: save
                spriteCreator.save();
            }
        } else {
          // S: nothing yet
        }
    }

    // ----- KEYBOARD CONTROLS -----

    // Q: select prev frame
    if (keyJustPressed(SDL_SCANCODE_Q)) spriteCreator.selectPrevFrame();

    // E: select next frame
    if (keyJustPressed(SDL_SCANCODE_E)) spriteCreator.selectNextFrame();

    // A: select prev frame
    if (keyJustPressed(SDL_SCANCODE_A)) spriteCreator.selectPrevVertex();

    // D: select next frame
    if (keyJustPressed(SDL_SCANCODE_D)) spriteCreator.selectNextVertex();

    // W: insert vertex at mouse
    if (keyJustPressed(SDL_SCANCODE_W)) spriteCreator.insertVertex({(int16_t)inputs.mouseX,(int16_t)inputs.mouseY,0xC0});
    
    // X: delete vertex
    if (keyJustPressed(SDL_SCANCODE_X)) spriteCreator.deleteVertex();

    // +: insert frame
    if (keyJustPressed(SDL_SCANCODE_EQUALS)) spriteCreator.newFrame();

    // -: delete frame
    if (keyJustPressed(SDL_SCANCODE_MINUS)) spriteCreator.deleteFrame();

    // SHIFT: speed of arrow movements
    int shiftAmt = shiftPressed() ? 10 : 1;

    if (altPressed()) {
        // ALT+arrow keys: move all vertices in all frames
        if (keyJustPressed(SDL_SCANCODE_UP))         spriteCreator.moveAllVerticesInAllFrames(0,shiftAmt);
        else if (keyJustPressed(SDL_SCANCODE_DOWN))  spriteCreator.moveAllVerticesInAllFrames(0,-shiftAmt);
        else if (keyJustPressed(SDL_SCANCODE_LEFT))  spriteCreator.moveAllVerticesInAllFrames(-shiftAmt,0);
        else if (keyJustPressed(SDL_SCANCODE_RIGHT)) spriteCreator.moveAllVerticesInAllFrames(shiftAmt,0);
    } else if (ctrlPressed()) {
        // CTRL+arrow keys: move all vertices
        if (keyJustPressed(SDL_SCANCODE_UP))         spriteCreator.moveAllVertices(0,shiftAmt);
        else if (keyJustPressed(SDL_SCANCODE_DOWN))  spriteCreator.moveAllVertices(0,-shiftAmt);
        else if (keyJustPressed(SDL_SCANCODE_LEFT))  spriteCreator.moveAllVertices(-shiftAmt,0);
        else if (keyJustPressed(SDL_SCANCODE_RIGHT)) spriteCreator.moveAllVertices(shiftAmt,0);
    } else {
        // arrow keys: move vertex
        if (keyJustPressed(SDL_SCANCODE_UP))         spriteCreator.setVertexPos(currVert.x,currVert.y+shiftAmt);
        else if (keyJustPressed(SDL_SCANCODE_DOWN))  spriteCreator.setVertexPos(currVert.x,currVert.y-shiftAmt);
        else if (keyJustPressed(SDL_SCANCODE_LEFT))  spriteCreator.setVertexPos(currVert.x-shiftAmt,currVert.y);
        else if (keyJustPressed(SDL_SCANCODE_RIGHT)) spriteCreator.setVertexPos(currVert.x+shiftAmt,currVert.y);
    }

    // C: (dis)connect first and last vertex
    if (keyJustPressed(SDL_SCANCODE_C)) spriteCreator.toggleShapeClosure();

    if (keyJustPressed(SDL_SCANCODE_F)) {
        // CTRL-F: choose first frame 
        if (ctrlPressed()) spriteCreator.chooseFirstFrame();
        // F: choose first vertex
        else spriteCreator.chooseFirstVertex();
    }

    if (keyJustPressed(SDL_SCANCODE_R)) {
        // CTRL-R: reverse frame ordering
        if (ctrlPressed()) spriteCreator.reverseFrameOrdering();
        // R: reverse vertex ordering
        else spriteCreator.reverseVertexOrdering();
    }

    if (keyJustPressed(SDL_SCANCODE_N)) {
        // SHIFT-N: swap current and prev frames
        if (shiftPressed()) spriteCreator.swapCurrAndPrevFrame();
        // N: swap current and next frames
        else spriteCreator.swapCurrAndNextFrame();
    }

    // SHIFT: speed of bright change 
    int bShiftAmt = shiftPressed() ? 0x40 : 0x1;

    // 0: increase brightness
    if (keyJustPressed(SDL_SCANCODE_0)) spriteCreator.setVertexBright((currVert.bright+bShiftAmt)%0x100);

    // 9: decrease brightness
    if (keyJustPressed(SDL_SCANCODE_9)) spriteCreator.setVertexBright((currVert.bright-bShiftAmt)%0x100);

    // SHIFT: speed of zoom change 
    float zShiftAmt = shiftPressed() ? 0.2 : 0.01;

    // ]: zoom in
    if (keyJustPressed(SDL_SCANCODE_RIGHTBRACKET)) spriteCreator.zoomIn(zShiftAmt);

    // [: zoom out 
    if (keyJustPressed(SDL_SCANCODE_LEFTBRACKET)) spriteCreator.zoomOut(zShiftAmt);

    // H: help
    if (keyJustPressed(SDL_SCANCODE_H)) printHelp();
    
    // T: change background transparency
    if (keyJustPressed(SDL_SCANCODE_T)) {
        transparency = (transparency <= 0.1) ? 1 : transparency-0.1;
        makeWindowTransparent(transparency);
    }

    // Y: toggle crosshairs
    if (keyJustPressed(SDL_SCANCODE_Y)) {
        switch(crosshairs) {
        case TWO_HAIR:
            crosshairs = FOUR_HAIR;
            break;
        case FOUR_HAIR:
            crosshairs = NO_HAIR;
            break;
        default:
            crosshairs = TWO_HAIR;
            break;
        }
    }

    // O: toggle onionskinning
    if (keyJustPressed(SDL_SCANCODE_O)) {
        switch(onion) {
        case PREV_ONION:
            onion = NEXT_ONION;
            break;
        case NEXT_ONION:
            onion = BOTH_ONION;
            break;
        case BOTH_ONION:
            onion = ALL_ONION;
            break;
        case ALL_ONION:
            onion = NO_ONION;
            break;
        default:
            onion = PREV_ONION;
            break;
        }
    }


    // ----- MOUSE -----

    // Right click: insert vertex
    if (inputs.pressedMouseButtons & SDL_BUTTON_RMASK) {
        spriteCreator.insertVertex({(int16_t)inputs.mouseX,(int16_t)inputs.mouseY,0xC0});
        spriteCreator.selectNearestVertex(inputs.mouseX,inputs.mouseY);
    }
    // Left click: move vertex
    if (inputs.mouseButtons & (SDL_BUTTON_LMASK | SDL_BUTTON_RMASK)) {
        draw_absolute_vector(inputs.mouseX,inputs.mouseY,0);
        draw_relative_vector(-5,-5,0);
        draw_relative_vector(10,10,255);
        draw_relative_vector(0,-10,0);
        draw_relative_vector(-10,10,255);
        spriteCreator.setVertexPos(inputs.mouseX,inputs.mouseY);
    } else if (inputs.mouseMoved) {
        // Mouse move: select vertex
        spriteCreator.selectNearestVertex(inputs.mouseX,inputs.mouseY);
    }
    // Middle click: delete vertex
    if (inputs.pressedMouseButtons & SDL_BUTTON_MMASK) {
        spriteCreator.deleteVertex();
    }
    
    if (inputs.mouseScrollY != 0) {
        if (shiftPressed()) {
            // CTRL+scroll wheel: change zoom
            if (inputs.mouseScrollY > 0) {
                spriteCreator.zoomIn(0.02);
            } else {
                spriteCreator.zoomOut(0.02);
            }
        } else {
            // Scroll wheel: change brightness
            spriteCreator.setVertexBright((currVert.bright+inputs.mouseScrollY)%0x100);
        }
    }


    // ----- GUI -----

    // Title Text
    load_abs_pos(-300,480);
    drawString("welcome to sprite creator",100,4);

    // Frame buttons
    load_abs_pos(-500,-500);
    drawString("prev",keyIsPressed(SDL_SCANCODE_Q) ? 255 : 50, 3);
    load_abs_pos(435,-500);
    drawString("next",keyIsPressed(SDL_SCANCODE_E) ? 255 : 50, 3);
    sprintf(miscText, "frame %ld of %ld", spriteCreator.currFrameIndex, spriteCreator.numFrames()-1);
    load_abs_pos(-120,-500);
    drawString(miscText, 0xC0, 3);

    // Vertex buttons
    load_abs_pos(-500,-470);
    drawString("prev",keyIsPressed(SDL_SCANCODE_A) ? 255 : 50, 3);
    load_abs_pos(435,-470);
    drawString("next",keyIsPressed(SDL_SCANCODE_D) ? 255 : 50, 3);
    sprintf(miscText, "vertex %ld of %ld", spriteCreator.currVertIndex,spriteCreator.numVertices());
    load_abs_pos(-130,-470);
    drawString(miscText, 0xC0, 3);

    // Coordinates box
    sprintf(miscText, "x %d",currVert.x);
    load_abs_pos(-400,-470);
    drawString(miscText,0xC0,3);
    sprintf(miscText, "y %d",currVert.y);
    load_abs_pos(-270,-470);
    drawString(miscText,0xC0,3);
    draw_absolute_vector(-410,-444,0);
    draw_absolute_vector(-154,-444,0x7F);
    draw_absolute_vector(-154,-478,0x7F);
    draw_absolute_vector(-410,-478,0x7F);
    draw_absolute_vector(-410,-444,0x7F);

    // Brightness box
    load_abs_pos(200,-470);
    drawString("bright",currVert.bright,3);
    sprintf(miscText, "0x%x",currVert.bright);
    load_abs_pos(320,-470);
    drawString(miscText,0xCF,3);
    draw_absolute_vector(395,-444,0);
    draw_absolute_vector(190,-444,0x7F);
    draw_absolute_vector(190,-478,0x7F);
    draw_absolute_vector(395,-478,0x7F);
    draw_absolute_vector(395,-444,0x7F);

    // Zoom text
    if (spriteCreator.zoomLvl() != 1) {
        sprintf(miscText, "zoom %d", int(spriteCreator.zoomLvl()*100));
        draw_absolute_vector(-500,480,0);
        drawString(miscText,0x80,3);
    }

    // Highlight current vertex
    int blinkBright = (timeCount % 200 > 100) ? 128 : 255;
    draw_absolute_vector(currVert.x,currVert.y,0);
    draw_relative_vector(-3,-3,0);
    draw_relative_vector(6,6,blinkBright);
    draw_relative_vector(0,-6,0);
    draw_relative_vector(-6,6,blinkBright);

    // Crosshairs
    switch(crosshairs) {
        case TWO_HAIR:
            draw_absolute_vector(-512,0,0);
            draw_absolute_vector(511,0,0x60);
            draw_absolute_vector(0,-445,0);
            draw_absolute_vector(0,475,0x60);
            break;
        case FOUR_HAIR:
            draw_absolute_vector(-512,0,0);
            draw_absolute_vector(511,0,0x60);
            draw_absolute_vector(0,-445,0);
            draw_absolute_vector(0,475,0x60);
            draw_absolute_vector(-445,-445,0);
            draw_absolute_vector(511,511,0x60);
            draw_absolute_vector(445,-445,0);
            draw_absolute_vector(-512,511,0x60);
            break;
        default:
            break;
    }

    // Onion skins
    switch(onion) {
        case PREV_ONION:
            spriteCreator.renderPrevOnion();
            break;
        case NEXT_ONION:
            spriteCreator.renderNextOnion();
            break;  
        case BOTH_ONION:
            spriteCreator.renderPrevOnion();
            spriteCreator.renderNextOnion();
            break;
        case ALL_ONION:
            spriteCreator.renderAllOnion();
        default:
            break;
    }

    // Sprite itself
    spriteCreator.render(timeCount % 200 > 100);

    draw_end_buffer();
    timeCount++;
}

///////////////
// Main Code //
///////////////

int main(int argc, char* argv[]) {
    printf("Welcome to Sprite Creator\n");

    int helped = 0;
    for (int i=1; i<argc; i++) {
        if (!(strcmp(argv[i],"-h") && strcmp(argv[i],"-H") && strcmp(argv[i],"help"))) {
            printHelp();
            helped = 1;
        } else {
            spriteCreator.loadBinary(argv[i]);
            spriteCreator.selectFrame(0);
            spriteCreator.selectVertex(0);
        }
    }
    if (!helped) printf("Press H for help.\n");

    // Startup
    initialize_input_output();
    makeWindowTransparent(0.8);

    // Render loop
    while (1) {
        start_timer(20); // 50 Hz
        
        inputs = get_inputs();

        updateGame();

        draw_buffer_switch();

        while (!timer_done()) {}
    }
}