#include <stdint.h>
#include <math.h>
#include "../include/rawio.h"
#include "../lib/visuals/visuals.hpp"

/////////////////
// Global Vars //
/////////////////
struct InputState inputs;
size_t frameCount = 0;
enum {MENU,GAME} global_game_state;
#define WATER_HEIGHT 200
// Sorry not everything is perfectly equivalent between simulated and hardware implementations
#define SIMULATION 1

/////////////
// Sprites //
/////////////
const float sine_lookup[100] = {0.0,0.062791,0.125333,0.187381,0.24869,0.309017,0.368125,0.425779,0.481754,0.535827,0.587785,0.637424,0.684547,0.728969,0.770513,0.809017,0.844328,0.876307,0.904827,0.929776,0.951057,0.968583,0.982287,0.992115,0.998027,1.0,0.998027,0.992115,0.982287,0.968583,0.951057,0.929776,0.904827,0.876307,0.844328,0.809017,0.770513,0.728969,0.684547,0.637424,0.587785,0.535827,0.481754,0.425779,0.368125,0.309017,0.24869,0.187381,0.125333,0.062791,0.0,-0.062791,-0.125333,-0.187381,-0.24869,-0.309017,-0.368125,-0.425779,-0.481754,-0.535827,-0.587785,-0.637424,-0.684547,-0.728969,-0.770513,-0.809017,-0.844328,-0.876307,-0.904827,-0.929776,-0.951057,-0.968583,-0.982287,-0.992115,-0.998027,-1.0,-0.998027,-0.992115,-0.982287,-0.968583,-0.951057,-0.929776,-0.904827,-0.876307,-0.844328,-0.809017,-0.770513,-0.728969,-0.684547,-0.637424,-0.587785,-0.535827,-0.481754,-0.425779,-0.368125,-0.309017,-0.24869,-0.187381,-0.125333,-0.062791};

const size_t pengbody_numVertices[11] = {21, 21, 22, 21, 19, 18, 20, 21, 22, 22, 20};
const vertB pengbody_f0[21] = {{84,0,0x0}, {18,-5,0xc0}, {-18,-12,0xc0}, {-5,-12,0xc0}, {-11,-16,0xc0}, {-10,-10,0xc0}, {-128,1,0xc0}, {-45,21,0xc0}, {-21,-14,0xc0}, {-12,-5,0xc0}, {6,10,0xc0}, {-11,-3,0xc0}, {29,18,0xc0}, {9,16,0xc0}, {-17,5,0xc0}, {39,3,0xc0}, {27,18,0xc0}, {51,2,0xc0}, {42,-4,0xc0}, {41,0,0xc0}, {18,-12,0xc0}};
const vertB pengbody_f1[21] = {{83,3,0x0}, {14,-5,0xc0}, {-14,-11,0xc0}, {-13,-17,0xc0}, {-3,-15,0xc0}, {-9,-11,0xc0}, {-123,0,0xc0}, {-37,16,0xc0}, {-14,6,0xc0}, {-17,-11,0xc0}, {3,-8,0xc0}, {-13,8,0xc0}, {20,21,0xc0}, {3,12,0xc0}, {-23,7,0xc0}, {52,3,0xc0}, {24,15,0xc0}, {44,3,0xc0}, {50,-3,0xc0}, {37,0,0xc0}, {19,-11,0xc0}};
const vertB pengbody_f2[22] = {{81,6,0x0}, {14,-4,0xc0}, {-15,-12,0xc0}, {-10,-16,0xc0}, {-7,-19,0xc0}, {-14,-10,0xc0}, {-115,0,0xc0}, {-33,5,0xc0}, {-5,18,0xc0}, {-12,-1,0xc0}, {-2,-14,0xc0}, {7,-8,0xc0}, {-14,4,0xc0}, {7,32,0xc0}, {-23,3,0xc0}, {22,4,0xc0}, {29,8,0xc0}, {23,16,0xc0}, {45,6,0xc0}, {50,-2,0xc0}, {34,1,0xc0}, {20,-10,0xc0}};
const vertB pengbody_f3[21] = {{81,6,0x0}, {13,-3,0xc0}, {-13,-13,0xc0}, {-10,-16,0xc0}, {-5,-16,0xc0}, {-21,-13,0xc0}, {-111,0,0xc0}, {-27,7,0xc0}, {3,-7,0xc0}, {-12,1,0xc0}, {-3,25,0xc0}, {-42,1,0xc0}, {29,8,0xc0}, {8,2,0xc0}, {12,14,0xc0}, {14,-1,0xc0}, {19,14,0xc0}, {43,9,0xc0}, {63,0,0xc0}, {18,1,0xc0}, {22,-13,0xc0}};
const vertB pengbody_f4[19] = {{82,5,0x0}, {14,-4,0xc0}, {-14,-12,0xc0}, {-8,-15,0xc0}, {-5,-18,0xc0}, {-17,-10,0xc0}, {-118,-1,0xc0}, {-21,2,0xc0}, {-5,20,0xc0}, {-5,3,0xc0}, {-51,-2,0xc0}, {47,14,0xc0}, {5,13,0xc0}, {16,-2,0xc0}, {19,15,0xc0}, {41,8,0xc0}, {63,0,0xc0}, {21,0,0xc0}, {19,-11,0xc0}};
const vertB pengbody_f5[18] = {{82,6,0x0}, {20,-7,0xc0}, {-15,-13,0xc0}, {-9,-15,0xc0}, {-7,-16,0xc0}, {-14,-10,0xc0}, {-123,0,0xc0}, {-15,9,0xc0}, {-11,20,0xc0}, {-53,2,0xc0}, {47,9,0xc0}, {9,14,0xc0}, {12,-3,0xc0}, {18,14,0xc0}, {38,7,0xc0}, {63,0,0xc0}, {30,-3,0xc0}, {9,-6,0xc0}};
const vertB pengbody_f6[20] = {{90,2,0x0}, {18,-5,0xc0}, {-15,-12,0xc0}, {-11,-16,0xc0}, {-8,-14,0xc0}, {-12,-8,0xc0}, {-119,-1,0xc0}, {-25,14,0xc0}, {-10,-6,0xc0}, {5,12,0xc0}, {-5,16,0xc0}, {-44,7,0xc0}, {40,3,0xc0}, {11,14,0xc0}, {10,-5,0xc0}, {20,11,0xc0}, {33,4,0xc0}, {58,0,0xc0}, {41,-3,0xc0}, {11,-9,0xc0}};
const vertB pengbody_f7[21] = {{96,-1,0x0}, {13,-3,0xc0}, {-14,-12,0xc0}, {-12,-16,0xc0}, {-5,-9,0xc0}, {-10,-10,0xc0}, {-124,-2,0xc0}, {-31,14,0xc0}, {1,14,0xc0}, {-17,-25,0xc0}, {0,8,0xc0}, {11,24,0xc0}, {-36,15,0xc0}, {33,0,0xc0}, {10,9,0xc0}, {8,-4,0xc0}, {21,11,0xc0}, {34,3,0xc0}, {60,-3,0xc0}, {38,-2,0xc0}, {20,-11,0xc0}};
const vertB pengbody_f8[22] = {{94,-2,0x0}, {14,-3,0xc0}, {-14,-13,0xc0}, {-5,-8,0xc0}, {-8,-8,0xc0}, {-14,-19,0xc0}, {-130,-1,0xc0}, {-28,11,0xc0}, {0,14,0xc0}, {-11,-17,0xc0}, {-14,-2,0xc0}, {16,26,0xc0}, {-1,6,0xc0}, {-21,6,0xc0}, {21,2,0xc0}, {11,11,0xc0}, {10,-3,0xc0}, {23,15,0xc0}, {34,3,0xc0}, {58,-3,0xc0}, {41,-4,0xc0}, {17,-10,0xc0}};
const vertB pengbody_f9[22] = {{90,-1,0x0}, {18,-4,0xc0}, {-17,-15,0xc0}, {-7,-12,0xc0}, {-16,-21,0xc0}, {-130,-1,0xc0}, {-28,11,0xc0}, {0,11,0xc0}, {-7,1,0xc0}, {-11,-17,0xc0}, {-17,0,0xc0}, {20,24,0xc0}, {1,8,0xc0}, {-17,3,0xc0}, {20,3,0xc0}, {7,6,0xc0}, {15,1,0xc0}, {22,17,0xc0}, {34,1,0xc0}, {58,-5,0xc0}, {41,-5,0xc0}, {12,-7,0xc0}};
const vertB pengbody_f10[20] = {{88,0,0x0}, {14,-3,0xc0}, {-13,-14,0xc0}, {-11,-17,0xc0}, {-13,-20,0xc0}, {-131,1,0xc0}, {-25,15,0xc0}, {-12,10,0xc0}, {-15,-15,0xc0}, {-15,-4,0xc0}, {0,5,0xc0}, {23,19,0xc0}, {5,11,0xc0}, {-14,6,0xc0}, {37,6,0xc0}, {27,16,0xc0}, {51,2,0xc0}, {37,-5,0xc0}, {40,-3,0xc0}, {17,-9,0xc0}};
const vertB* pengbody_frames[11] = {pengbody_f0, pengbody_f1, pengbody_f2, pengbody_f3, pengbody_f4, pengbody_f5, pengbody_f6, pengbody_f7, pengbody_f8, pengbody_f9, pengbody_f10};
sprite pengbody(11,pengbody_numVertices,pengbody_frames);

const size_t fishSprite_numVertices[1] = {9};
const vertB fishSprite_f0[9] = {{30,0,0x0}, {-30,-15,0xc0}, {-30,10,0xc0}, {-25,-10,0xc0}, {12,15,0xc0}, {-12,15,0xc0}, {25,-10,0xc0}, {30,10,0xc0}, {30,-15,0xc0}};
const vertB* fishSprite_frames[1] = {fishSprite_f0};
sprite fishSprite(1,fishSprite_numVertices,fishSprite_frames);

const size_t sun_numVertices[1] = {24};
const vertB sun_f0[24] = {{-26,-18,0x0}, {11,-11,0xff}, {15,-3,0xff}, {15,4,0xff}, {-4,-1,0x0}, {-23,0,0x30}, {-5,4,0x0}, {36,0,0x50}, {5,5,0x0}, {-47,0,0x70}, {-3,5,0x0}, {54,0,0x90}, {1,7,0x0}, {-58,0,0xb0}, {44,-21,0x0}, {11,11,0xff}, {4,15,0xff}, {-4,15,0xff}, {-11,11,0xff}, {-15,4,0xff}, {-15,-3,0xee}, {-10,-11,0xff}, {-4,-15,0xff}, {3,-15,0xff}};
const vertB* sun_frames[1] = {sun_f0};
sprite sun(1,sun_numVertices,sun_frames);

// blink 0 thru MAX_BLINK_HEAD
#define MAX_BLINK_HEAD 3
// neutral
#define NEUTRAL_HEAD 4
// mouth NEUTRAL_HEAD+1 thru MAX_MOUTH_HEAD
#define MAX_MOUTH_HEAD 14
const size_t pengHead_numVertices[15] = {18, 18, 18, 18, 18, 18, 20, 20, 20, 20, 21, 21, 21, 21, 21};
const vertB pengHead_f0[18] = {{12,20,0x0}, {9,-2,0xc0}, {-2,-9,0x0}, {-1,-2,0xff}, {-3,0,0xff}, {-2,2,0xff}, {2,1,0xff}, {2,0,0xff}, {2,-1,0xff}, {2,9,0x0}, {9,-6,0xc0}, {16,-3,0xc0}, {6,-3,0xc0}, {-26,0,0xc0}, {26,-1,0xc0}, {-5,-2,0xc0}, {-20,-1,0xc0}, {-9,-2,0xc0}};
const vertB pengHead_f1[18] = {{12,20,0x0}, {9,-2,0xc0}, {-2,-9,0x0}, {-1,-1,0xff}, {-3,0,0xff}, {-2,1,0xff}, {2,1,0xff}, {2,0,0xff}, {2,-1,0xff}, {2,9,0x0}, {9,-6,0xc0}, {16,-3,0xc0}, {6,-3,0xc0}, {-26,0,0xc0}, {26,-1,0xc0}, {-5,-2,0xc0}, {-20,-1,0xc0}, {-9,-2,0xc0}};
const vertB pengHead_f2[18] = {{12,20,0x0}, {9,-2,0xc0}, {-2,-9,0x0}, {-1,-1,0xff}, {-3,0,0xff}, {-2,1,0xff}, {2,0,0xff}, {2,0,0xff}, {2,0,0xff}, {2,9,0x0}, {9,-6,0xc0}, {16,-3,0xc0}, {6,-3,0xc0}, {-26,0,0xc0}, {26,-1,0xc0}, {-5,-2,0xc0}, {-20,-1,0xc0}, {-9,-2,0xc0}};
const vertB pengHead_f3[18] = {{12,20,0x0}, {9,-2,0xc0}, {-2,-9,0x0}, {-1,0,0xff}, {-3,0,0xff}, {-2,0,0xff}, {2,0,0xff}, {2,0,0xff}, {2,0,0xff}, {2,9,0x0}, {9,-6,0xc0}, {16,-3,0xc0}, {6,-3,0xc0}, {-26,0,0xc0}, {26,-1,0xc0}, {-5,-2,0xc0}, {-20,-1,0xc0}, {-9,-2,0xc0}};
const vertB pengHead_f4[18] = {{12,20,0x0}, {9,-2,0xc0}, {-2,-9,0x0}, {-1,-3,0xff}, {-3,0,0xff}, {-2,2,0xff}, {2,2,0xff}, {2,0,0xff}, {2,-2,0xff}, {2,9,0x0}, {9,-6,0xc0}, {16,-3,0xc0}, {6,-3,0xc0}, {-26,0,0xc0}, {26,-1,0xc0}, {-5,-2,0xc0}, {-20,-1,0xc0}, {-9,-2,0xc0}};
const vertB pengHead_f5[18] = {{12,20,0x0}, {9,-2,0xc0}, {-2,-9,0x0}, {-1,-3,0xff}, {-3,0,0xff}, {-2,2,0xff}, {2,2,0xff}, {2,0,0xff}, {2,-2,0xff}, {2,9,0x0}, {9,-6,0xc0}, {16,-3,0xc0}, {6,-3,0xc0}, {-26,0,0xc0}, {26,-2,0xc0}, {-5,-2,0xc0}, {-20,-1,0xc0}, {-9,-2,0xc0}};
const vertB pengHead_f6[20] = {{12,20,0x0}, {9,-2,0xc0}, {-2,-9,0x0}, {-1,-3,0xff}, {-3,0,0xff}, {-2,2,0xff}, {2,2,0xff}, {2,0,0xff}, {2,-2,0xff}, {2,9,0x0}, {9,-6,0xc0}, {16,-3,0xc0}, {6,-3,0xc0}, {-22,1,0xc0}, {-3,0,0xc0}, {3,-1,0xc0}, {22,-2,0xc0}, {-6,-2,0xc0}, {-20,0,0xc0}, {-9,-1,0xc0}};
const vertB pengHead_f7[20] = {{12,20,0x0}, {9,-2,0xc0}, {-2,-9,0x0}, {-1,-3,0xff}, {-3,0,0xff}, {-2,2,0xff}, {2,2,0xff}, {2,0,0xff}, {2,-2,0xff}, {2,9,0x0}, {9,-6,0xc0}, {16,-3,0xc0}, {6,-3,0xc0}, {-22,1,0xc0}, {-4,-2,0xc0}, {2,-1,0xc0}, {24,-2,0xc0}, {-7,-2,0xc0}, {-18,0,0xc0}, {-9,-1,0xc0}};
const vertB pengHead_f8[20] = {{12,20,0x0}, {9,-2,0xc0}, {-2,-9,0x0}, {-1,-3,0xff}, {-3,0,0xff}, {-2,2,0xff}, {2,2,0xff}, {2,0,0xff}, {2,-2,0xff}, {2,9,0x0}, {9,-6,0xc0}, {16,-3,0xc0}, {6,-3,0xc0}, {-22,1,0xc0}, {-4,-2,0xc0}, {1,-1,0xc0}, {26,-3,0xc0}, {-7,-1,0xc0}, {-18,0,0xc0}, {-9,0,0xc0}};
const vertB pengHead_f9[20] = {{12,20,0x0}, {9,-2,0xc0}, {-2,-9,0x0}, {-1,-3,0xff}, {-3,0,0xff}, {-2,2,0xff}, {2,2,0xff}, {2,0,0xff}, {2,-2,0xff}, {2,9,0x0}, {9,-6,0xc0}, {16,-3,0xc0}, {6,-3,0xc0}, {-22,1,0xc0}, {-4,-2,0xc0}, {1,-3,0xc0}, {24,-4,0xc0}, {-7,-1,0xc0}, {-17,1,0xc0}, {-9,0,0xc0}};
const vertB pengHead_f10[21] = {{12,20,0x0}, {9,-2,0xc0}, {-2,-9,0x0}, {-1,-3,0xff}, {-3,0,0xff}, {-2,2,0xff}, {2,2,0xff}, {2,0,0xff}, {2,-2,0xff}, {2,9,0x0}, {9,-6,0xc0}, {16,-3,0xc0}, {6,-3,0xc0}, {-22,1,0xc0}, {-4,-2,0xc0}, {0,-2,0xc0}, {1,-1,0xc0}, {22,-5,0xc0}, {-7,-1,0xc0}, {-16,2,0xc0}, {-9,0,0xc0}};
const vertB pengHead_f11[21] = {{12,20,0x0}, {9,-2,0xc0}, {-2,-9,0x0}, {-1,-3,0xff}, {-3,0,0xff}, {-2,2,0xff}, {2,2,0xff}, {2,0,0xff}, {2,-2,0xff}, {2,9,0x0}, {9,-6,0xc0}, {16,-3,0xc0}, {6,-3,0xc0}, {-23,1,0xc0}, {-5,-2,0xc0}, {0,-2,0xc0}, {2,-2,0xc0}, {22,-7,0xc0}, {-7,0,0xc0}, {-15,2,0xc0}, {-9,1,0xc0}};
const vertB pengHead_f12[21] = {{12,20,0x0}, {9,-2,0xc0}, {-2,-9,0x0}, {-1,-3,0xff}, {-3,0,0xff}, {-2,2,0xff}, {2,2,0xff}, {2,0,0xff}, {2,-2,0xff}, {2,9,0x0}, {9,-6,0xc0}, {16,-3,0xc0}, {6,-3,0xc0}, {-23,1,0xc0}, {-5,-2,0xc0}, {0,-3,0xc0}, {2,-2,0xc0}, {22,-7,0xc0}, {-8,-1,0xc0}, {-14,3,0xc0}, {-9,3,0xc0}};
const vertB pengHead_f13[21] = {{12,20,0x0}, {9,-2,0xc0}, {-2,-9,0x0}, {-1,-3,0xff}, {-3,0,0xff}, {-2,2,0xff}, {2,2,0xff}, {2,0,0xff}, {2,-2,0xff}, {2,9,0x0}, {9,-6,0xc0}, {16,-3,0xc0}, {6,-3,0xc0}, {-23,1,0xc0}, {-5,-2,0xc0}, {-1,-3,0xc0}, {1,-2,0xc0}, {23,-10,0xc0}, {-9,0,0xc0}, {-12,4,0xc0}, {-8,3,0xc0}};
const vertB pengHead_f14[21] = {{12,20,0x0}, {9,-2,0xc0}, {-2,-9,0x0}, {-1,-3,0xff}, {-3,0,0xff}, {-2,2,0xff}, {2,2,0xff}, {2,0,0xff}, {2,-2,0xff}, {2,9,0x0}, {9,-6,0xc0}, {16,-3,0xc0}, {6,-3,0xc0}, {-23,1,0xc0}, {-5,-2,0xc0}, {-2,-3,0xc0}, {1,-2,0xc0}, {21,-10,0xc0}, {-8,0,0xc0}, {-12,3,0xc0}, {-7,5,0xc0}};
const vertB* pengHead_frames[15] = {pengHead_f0, pengHead_f1, pengHead_f2, pengHead_f3, pengHead_f4, pengHead_f5, pengHead_f6, pengHead_f7, pengHead_f8, pengHead_f9, pengHead_f10, pengHead_f11, pengHead_f12, pengHead_f13, pengHead_f14};
vertB pengHead_renderedFrame[21];
sprite pengHead(15,pengHead_numVertices,pengHead_frames,pengHead_renderedFrame);

const size_t pengEye_numVertices[1] = {6};
const vertB pengEye_f0[6] = {{18,8,0x0}, {3,1,0xff}, {0,3,0xff}, {-3,0,0xff}, {-1,-2,0xff}, {2,-2,0xff}};
const vertB* pengEye_frames[1] = {pengEye_f0};
sprite pengEye(1,pengEye_numVertices,pengEye_frames);

const size_t pengTail_numVertices[1] = {5};
const vertB pengTail_f0[5] = {{1,-9,0x0}, {-17,4,0xc0}, {-11,14,0xc0}, {13,-5,0xc0}, {15,-3,0xc0}};
const vertB* pengTail_frames[1] = {pengTail_f0};
vertB pengTail_renderedFrame[5];
sprite pengTail(1,pengTail_numVertices,pengTail_frames,pengTail_renderedFrame);

const size_t pengTopBod_numVertices[1] = {5};
const vertB pengTopBod_f0[5] = {{-55,25,0x0}, {11,4,0xc0}, {22,6,0xc0}, {36,1,0xc0}, {36,-5,0xc0}};
const vertB* pengTopBod_frames[1] = {pengTopBod_f0};
vertB pengTopBod_renderedFrame[5];
sprite pengTopBod(1,pengTopBod_numVertices,pengTopBod_frames,pengTopBod_renderedFrame);

const size_t pengBotBod_numVertices[1] = {5};
const vertB pengBotBod_f0[5] = {{43,-7,0x0}, {-18,-3,0xc0}, {-51,0,0xc0}, {-22,7,0xc0}, {-5,10,0xc0}};
const vertB* pengBotBod_frames[1] = {pengBotBod_f0};
vertB pengBotBod_renderedFrame[5];
sprite pengBotBod(1,pengBotBod_numVertices,pengBotBod_frames,pengBotBod_renderedFrame);

const size_t pengFoot_numVertices[1] = {10};
const vertB pengFoot_f0[10] = {{-7,7,0x0}, {-10,-5,0xc0}, {-13,2,0xc0}, {3,-4,0xc0}, {-6,-3,0xc0}, {6,-3,0xc0}, {-4,-4,0xc0}, {11,-2,0xc0}, {8,5,0xc0}, {14,0,0xc0}};
const vertB* pengFoot_frames[1] = {pengFoot_f0};
vertB pengFoot_renderedFrame[10];
sprite pengFoot(1,pengFoot_numVertices,pengFoot_frames,pengFoot_renderedFrame);

#define DOWNMOST_FLAP 18
#define MIDDLE_FLAP 12
#define UPMOST_FLAP 0
const size_t pengFlip_numVertices[27] = {9, 9, 9, 8, 8, 8, 8, 8, 8, 8, 8, 8, 10, 10, 10, 10, 10, 10, 10, 9, 9, 9, 9, 8, 8, 8, 8};
const vertB pengFlip_f0[9] = {{-17,1,0x0}, {-6,10,0xc0}, {-14,12,0xc0}, {-19,11,0xc0}, {-27,7,0xc0}, {29,2,0xc0}, {22,-9,0xc0}, {20,-15,0xc0}, {11,-21,0xc0}};
const vertB pengFlip_f1[9] = {{-17,1,0x0}, {-6,8,0xc0}, {-11,10,0xc0}, {-17,11,0xc0}, {-25,7,0xc0}, {23,1,0xc0}, {27,-9,0xc0}, {16,-14,0xc0}, {11,-16,0xc0}};
const vertB pengFlip_f2[9] = {{-17,1,0x0}, {-10,10,0xc0}, {-9,8,0xc0}, {-12,7,0xc0}, {-21,8,0xc0}, {20,0,0xc0}, {19,-5,0xc0}, {21,-16,0xc0}, {9,-13,0xc0}};
const vertB pengFlip_f3[8] = {{-17,1,0x0}, {-15,13,0xc0}, {-20,9,0xc0}, {-17,6,0xc0}, {20,0,0xc0}, {19,-7,0xc0}, {18,-12,0xc0}, {12,-13,0xc0}};
const vertB pengFlip_f4[8] = {{-17,1,0x0}, {-17,12,0xc0}, {-19,8,0xc0}, {-21,4,0xc0}, {18,1,0xc0}, {22,-4,0xc0}, {22,-11,0xc0}, {12,-12,0xc0}};
const vertB pengFlip_f5[8] = {{-17,1,0x0}, {-18,11,0xc0}, {-18,5,0xc0}, {-29,4,0xc0}, {23,3,0xc0}, {25,-5,0xc0}, {21,-10,0xc0}, {11,-10,0xc0}};
const vertB pengFlip_f6[8] = {{-17,1,0x0}, {-18,9,0xc0}, {-30,7,0xc0}, {-21,1,0xc0}, {24,3,0xc0}, {25,-3,0xc0}, {25,-10,0xc0}, {12,-9,0xc0}};
const vertB pengFlip_f7[8] = {{-17,1,0x0}, {-18,7,0xc0}, {-29,6,0xc0}, {-26,0,0xc0}, {24,5,0xc0}, {28,-2,0xc0}, {25,-9,0xc0}, {13,-8,0xc0}};
const vertB pengFlip_f8[8] = {{-17,1,0x0}, {-22,6,0xc0}, {-30,2,0xc0}, {-22,0,0xc0}, {20,6,0xc0}, {32,-1,0xc0}, {23,-7,0xc0}, {16,-9,0xc0}};
const vertB pengFlip_f9[8] = {{-17,1,0x0}, {-22,4,0xc0}, {-28,0,0xc0}, {-25,0,0xc0}, {21,6,0xc0}, {33,0,0xc0}, {27,-6,0xc0}, {11,-5,0xc0}};
const vertB pengFlip_f10[8] = {{-17,1,0x0}, {-26,0,0xc0}, {-29,0,0xc0}, {-21,0,0xc0}, {19,3,0xc0}, {23,1,0xc0}, {36,-2,0xc0}, {13,-4,0xc0}};
const vertB pengFlip_f11[8] = {{-17,1,0x0}, {-21,-1,0xc0}, {-33,0,0xc0}, {-22,-1,0xc0}, {20,-1,0xc0}, {17,0,0xc0}, {35,1,0xc0}, {21,1,0xc0}};
const vertB pengFlip_f12[10] = {{-17,1,0x0}, {-13,-1,0xc0}, {-12,-1,0xc0}, {-32,-3,0xc0}, {-16,-2,0xc0}, {15,-1,0xc0}, {18,0,0xc0}, {30,2,0xc0}, {11,3,0xc0}, {16,2,0xc0}};
const vertB pengFlip_f13[10] = {{-17,1,0x0}, {-12,-2,0xc0}, {-13,-2,0xc0}, {-30,-5,0xc0}, {-17,-4,0xc0}, {15,-1,0xc0}, {18,0,0xc0}, {28,4,0xc0}, {12,4,0xc0}, {16,4,0xc0}};
const vertB pengFlip_f14[10] = {{-17,1,0x0}, {-11,-2,0xc0}, {-12,-4,0xc0}, {-31,-9,0xc0}, {-17,-4,0xc0}, {13,-2,0xc0}, {21,1,0xc0}, {26,8,0xc0}, {15,7,0xc0}, {13,4,0xc0}};
const vertB pengFlip_f15[10] = {{-17,1,0x0}, {-11,-3,0xc0}, {-13,-7,0xc0}, {-28,-11,0xc0}, {-17,-3,0xc0}, {13,-3,0xc0}, {23,2,0xc0}, {20,8,0xc0}, {17,10,0xc0}, {14,5,0xc0}};
const vertB pengFlip_f16[10] = {{-17,1,0x0}, {-12,-5,0xc0}, {-14,-10,0xc0}, {-28,-12,0xc0}, {-15,-3,0xc0}, {11,-5,0xc0}, {27,5,0xc0}, {18,11,0xc0}, {17,13,0xc0}, {14,5,0xc0}};
const vertB pengFlip_f17[10] = {{-17,1,0x0}, {-14,-8,0xc0}, {-13,-12,0xc0}, {-21,-11,0xc0}, {-17,-6,0xc0}, {14,-4,0xc0}, {20,6,0xc0}, {16,11,0xc0}, {11,12,0xc0}, {20,12,0xc0}};
const vertB pengFlip_f18[10] = {{-17,1,0x0}, {-14,-10,0xc0}, {-15,-16,0xc0}, {-20,-15,0xc0}, {-12,-10,0xc0}, {17,0,0xc0}, {17,7,0xc0}, {12,10,0xc0}, {13,17,0xc0}, {18,14,0xc0}};
const vertB pengFlip_f19[9] = {{-17,1,0x0}, {-15,-9,0xc0}, {-19,-18,0xc0}, {-23,-20,0xc0}, {-5,-7,0xc0}, {18,2,0xc0}, {22,15,0xc0}, {19,22,0xc0}, {20,12,0xc0}};
const vertB pengFlip_f20[9] = {{-17,1,0x0}, {-16,-8,0xc0}, {-19,-14,0xc0}, {-17,-15,0xc0}, {-10,-14,0xc0}, {14,1,0xc0}, {23,19,0xc0}, {22,17,0xc0}, {21,11,0xc0}};
const vertB pengFlip_f21[9] = {{-17,1,0x0}, {-18,-6,0xc0}, {-19,-11,0xc0}, {-16,-13,0xc0}, {-12,-14,0xc0}, {14,1,0xc0}, {23,16,0xc0}, {23,15,0xc0}, {23,9,0xc0}};
const vertB pengFlip_f22[9] = {{-17,1,0x0}, {-20,-5,0xc0}, {-19,-10,0xc0}, {-17,-11,0xc0}, {-12,-10,0xc0}, {14,0,0xc0}, {22,13,0xc0}, {25,13,0xc0}, {24,8,0xc0}};
const vertB pengFlip_f23[8] = {{-17,1,0x0}, {-21,-4,0xc0}, {-32,-12,0xc0}, {-21,-12,0xc0}, {16,-1,0xc0}, {23,10,0xc0}, {26,11,0xc0}, {25,8,0xc0}};
const vertB pengFlip_f24[8] = {{-17,1,0x0}, {-22,-3,0xc0}, {-33,-9,0xc0}, {-19,-7,0xc0}, {18,-1,0xc0}, {19,4,0xc0}, {29,7,0xc0}, {25,7,0xc0}};
const vertB pengFlip_f25[8] = {{-17,1,0x0}, {-25,-2,0xc0}, {-31,-6,0xc0}, {-20,-4,0xc0}, {19,-2,0xc0}, {17,2,0xc0}, {28,5,0xc0}, {29,5,0xc0}};
const vertB pengFlip_f26[8] = {{-17,1,0x0}, {-20,-2,0xc0}, {-35,-3,0xc0}, {-21,-3,0xc0}, {17,-1,0xc0}, {21,1,0xc0}, {30,3,0xc0}, {25,4,0xc0}};
const vertB* pengFlip_frames[27] = {pengFlip_f0, pengFlip_f1, pengFlip_f2, pengFlip_f3, pengFlip_f4, pengFlip_f5, pengFlip_f6, pengFlip_f7, pengFlip_f8, pengFlip_f9, pengFlip_f10, pengFlip_f11, pengFlip_f12, pengFlip_f13, pengFlip_f14, pengFlip_f15, pengFlip_f16, pengFlip_f17, pengFlip_f18, pengFlip_f19, pengFlip_f20, pengFlip_f21, pengFlip_f22, pengFlip_f23, pengFlip_f24, pengFlip_f25, pengFlip_f26};
sprite pengFlip(27,pengFlip_numVertices,pengFlip_frames);

/////////////////////////
// Background Graphics //
/////////////////////////

void renderMountains(int scrollX) {
    // Ha whoops looks like real hardware isn't ready for this
    #define POINTS 142
    const int16_t mountY[POINTS] = {5, -2, 5, 24, 45, 41, 48, 38, 23, 29, 6, 11, 35, 51, 40, 11, -12, -22, -26, -14, -16, -10, -9, -25, -14, -14, -27, -26, -24, -10, 8, 19, 47, 31, 31, 20, 17, -3, -21, -18, -29, -31, -13, -9, 1, 1, -6, 1, -18, -28, -45, -37, -43, -40, -49, -54, -53, -45, -53, -55, -58, -60, -57, -57, -42, -20, -2, 0, 24, 31, 38, 50, 40, 33, 38, 32, 9, 9, -12, -15, -34, -21,6, 0, 2, 9, 8, -21, -32, -45, -58, -53, -53, -58, -54, -54, -73, -62, -67, -85, -73, -75, -68, -60, -70, -71, -82, -83, -77, -60, -23, -11, 3, 4, 27, 42, 61, 72, 69, 60, 51, 55, 35, 12, 14, -6, 5, 41, 49, 61, 42, 37, 21, 13, -19, -40, -45, -37, -53, -55, -43, -25};
    const int16_t mountX[POINTS] = {10, 13, 34, 27, 45, 20, 12, 29, 12, 4, 38, 22, 38, 20, 22, 15, 29, 6, 17, 30, 13, 3, 8, 18, 10, 14, 7, 24, 18, 34, 7, 27, 37, 11, 15, 8, 22, 15, 24, 32, 11, 18, 19, 13, 4, 10, 2, 33, 20, 25, 23, 9, 13, 25, 17, 19, 26, 13, 9, 13, 18, 21, 10, 27, 35, 13, 38, 26, 51, 10, 31, 19, 13, 11, 21, 12, 48, 36, 25, 29, 29, 35,18, 7, 21, 7, 18, 32, 26, 26, 18, 10, 17, 7, 13, 22, 16, 12, 13, 17, 8, 14, 8, 18, 8, 17, 45, 20, 39, 50, 20, 39, 17, 47, 49, 13, 56, 15, 15, 4, 20, 13, 30, 56, 34, 33, 33, 43, 48, 32, 15, 20, 13, 33, 9, 31, 43, 11, 17, 14, 24, 27}; // pls don't make spacing larger than 100
    static int totalWidth;
    totalWidth = 0;
    for (int i = 0; i<POINTS; i++) {
        totalWidth += mountX[i];
    }
    // wrap around mountains
    scrollX = ((scrollX % totalWidth) + totalWidth) % totalWidth;
    // find first point
    int i = 0;
    int sumX = 0;
    while (sumX < scrollX) {
        sumX += mountX[i];
        i++;
    }
    // ah just start offscreen; hardware clipping should take care of it, I think
    int16_t currX = sumX-scrollX - 512 - 200 +  mountX[(i+POINTS)%POINTS];
    int16_t currY = mountY[(i+POINTS)%POINTS] + 360;
    i++;
    draw_absolute_vector(currX,currY,0x0);
    load_abs_pos(currX,currY);
    while (currX < 512) {
        currX += mountX[(i+POINTS)%POINTS];
        currY = mountY[(i+POINTS)%POINTS] + 360;
        i++;
        draw_absolute_vector(currX,currY,0xc0);
    }
}

void renderWaves(int scrollX) {
    const int period=400;
    const int spacing=64;
    scrollX = -(scrollX%period + period)%period;
    int currX = scrollX % spacing-512-10;
    int timeShift = (frameCount%(2*period))/2;
    draw_absolute_vector(currX,WATER_HEIGHT,0);
    load_abs_pos(currX,WATER_HEIGHT);
    while (currX < 512+spacing) {
        //int currY = 10*sin(2*M_PI*(currX-scrollX+timeShift)/period)+WATER_HEIGHT;
        // Lookup table sine might save floating point unit some grief.
        int currY = 10*sine_lookup[(((int)(currX-scrollX+timeShift)*100/period)+100) % 100]+WATER_HEIGHT;
        draw_absolute_vector(currX,currY,0x80);
        currX += spacing;
    }
}

///////////////////////////
// Game Objects 'n Stuff //
///////////////////////////
float gravity = 0.045;

int dumbDistance(int x1, int y1, int x2, int y2) {
    // dumb but computationally easier than actual pythagorus stuff (I think...)
    int dx = abs(x1 - x2);
    int dy = abs(y1 - y2);
    return dx + dy;
}

class physicsBox {
    public:
        float x = 0;
        float y = 0;
        float vx = 0;
        float vy = 0;
        float damping = 0.999;
        int bounced;
        int underwater;
        float currSpeed;

        physicsBox(int width, int height) : _width(width), _height(height) {}

        void update() {
            currSpeed = speed();
            underwater = y<WATER_HEIGHT;
            if (underwater) {
                vx *= damping;
                vy *= damping;
                vy += 0.003; // buoyant force or something
            } else {
                vy -= gravity;
            }
            x += vx;
            y += vy;
            _bounceOffWalls();
        }

        void render(int scrollX) {
            drawRect(x-scrollX,y,_width,_height,0x20);
        }

        void impulse(float ix, float iy) {
            vx+=ix;
            vy+=iy;
        }

        float speed() {
            return sqrt(vx*vx+vy*vy);
        }

        float dir() {
            return atan2(vy,vx);
        }

    private:
        int _width;
        int _height;
        

        void _bounceOffWalls() {
            bounced = 0;
            /*if ((x-_width/2) <= -512) {
                vx = abs(vx)*0.7;
                x = -512 + _width/2;
                bounced = 1;
            } else if ((x+_width/2 >= 512)) {
                vx = -abs(vx)*0.7;
                x = 512 - _width/2;
                bounced = 1;
            }*/
            if ((y-_height/2) <= -512) {
                vy = abs(vy)*0.3;
                y = -512 + _height/2;
                bounced = 1;
            } /*else if ((y+_height/2 >= 512)) {
                vy = -abs(vy)*0.7;
                y = 512 - _height/2;
                bounced = 1;
            }*/
        }
};

enum flapState {resting,charging,discharging,returning};
enum headState {blinking,unblinking,neutral,catching,uncatching};
int airtime = 0;
class penguin : public physicsBox {
    public:
        float headAngle;
        float bodyAngle;
        vertex pengMouthLocation;

        penguin() : physicsBox(70,50) {}

        void update(int flapPressed, int closestFishDistance, int eaten) {
            physicsBox::update();

            // physics stuff
            _smoothedSpeed=0;
            for (int i=7; i>0; i--) {
                _pastSpeeds[i] = _pastSpeeds[i-1];
                _smoothedSpeed += _pastSpeeds[i];
            }
            _pastSpeeds[0] = currSpeed;
            _smoothedSpeed = currSpeed;
            _smoothedSpeed /= 8; 
            float velDir = dir();
            if (bodyAngle>M_PI) bodyAngle = bodyAngle - 2*M_PI;
            if (bodyAngle<-M_PI) bodyAngle = bodyAngle + 2*M_PI;

            // Head state machine
            const int catchDistance = 400;
            if (closestFishDistance<catchDistance) {
                _headState = catching;
            }
            switch (_headState) {
                case blinking:
                    if (_headFrame<MAX_BLINK_HEAD) {
                        if (frameCount % 15 == 1) _headFrame++;
                    } else {
                        _headState = unblinking;
                    }
                    break;
                case unblinking:
                    if (_headFrame>0) {
                         if (frameCount % 15 == 1) _headFrame--;
                    } else {
                        // *** make this randomized
                        _headState = neutral;
                        _headFrame = NEUTRAL_HEAD;
                    }
                    break;
                case neutral:
                    if ((frameCount % 3000 == 1) || bounced) {
                        _headState = blinking;
                        _headFrame = 0;
                    }
                    break;
                case catching:
                    if (eaten) {
                        _headState = uncatching;
                    } else if (closestFishDistance>=catchDistance) {
                        _headState = neutral;
                    } else {
                        _headFrame = (MAX_MOUTH_HEAD - NEUTRAL_HEAD)/(1+0.3*(MAX_MOUTH_HEAD - NEUTRAL_HEAD)*closestFishDistance*closestFishDistance/(catchDistance*catchDistance)) + NEUTRAL_HEAD;
                        _headFrame = (_headFrame > MAX_MOUTH_HEAD) ? MAX_MOUTH_HEAD : _headFrame;
                    }
                    break;
                case uncatching:
                    if (_headFrame>NEUTRAL_HEAD) {
                         if (frameCount % 8 == 1) _headFrame--;
                    }
            }


            // Penguin has no control out of water
            if (underwater) {
                if (_stunTimer != 0) {
                    _stunTimer--;
                    bodyAngle+=_stunSpin*_stunTimer;
                } else {
                    if (bounced) {
                        _stunTimer = abs(vy)*200;
                        _stunSpin = 0.00005;
                    }
                    // Body angle steers velocity
                    //      Goal is to generally have velocity in same
                    //      direction as body angle, but allow for 
                    //      a little drifting if we have sharp turn.
                    //      (this math is a little jank but it shouldn't be a real problem
                    //       unless we really violate law of conservation of energy by a lot)
                    // should turn this off after being overwater to that we sink upon returning
                    float steerFactor = 0.02*(currSpeed)*(abs(cos(bodyAngle - velDir)));
                    vx = (steerFactor)*currSpeed*cos(bodyAngle)+(1.0-steerFactor)*vx;
                    vy = (steerFactor)*currSpeed*sin(bodyAngle)+(1.0-steerFactor)*vy;

                    // Head angle steers body angle
                    //      Body gradually rotates to follow head
                    //      If moving fast, body rotates more quickly
                    bodyAngle += 0.003*(3.5+currSpeed)*(headAngle - bodyAngle);

                    // Flapping state machine
                    int restingFlap = MIDDLE_FLAP - 1 + 0.5*(DOWNMOST_FLAP-MIDDLE_FLAP)/(1+35.0*_smoothedSpeed*_smoothedSpeed);
                    if (flapPressed) {
                        _flapState = charging;
                    }
                    switch (_flapState) {
                        case charging:
                            if (flapPressed) {
                                if (_flapCount>UPMOST_FLAP) {
                                    if ((frameCount % 4) == 1) _flapCount--;
                                    impulse(-0.003*cos(bodyAngle),-0.003*sin(bodyAngle));
                                }
                            } else {
                                _flapState = discharging;
                                // the longer the charge, the more the power
                                _flapCharge = DOWNMOST_FLAP - _flapCount;
                            }
                            break;
                        case discharging:
                            if (_flapCount<DOWNMOST_FLAP) {
                                if ((frameCount % 3) == 1) _flapCount++;
                                // make middle flap position more potent
                                float flapPower = 0.0002*(8+_flapCharge)*(DOWNMOST_FLAP-abs(_flapCount-MIDDLE_FLAP));
                                impulse(flapPower*cos(headAngle),flapPower*sin(headAngle));
                            } else {
                                _flapState = returning;
                            }
                            break;
                        case returning:
                            if (_flapCount>restingFlap) {
                                if ((frameCount % 12) == 1) _flapCount--;
                            } else if (_flapCount<restingFlap) {
                                if ((frameCount % 12) == 1) _flapCount++;
                            }
                            break;
                        default:
                            break;
                    }
                }
                // Damping
                //   This is nonlinear because I don't want much damping at slow speeds
                //   but I do want there to be a noticeable limit for how fast you can go
                damping = 0.99 + 7.0*0.01/(7.0+currSpeed*currSpeed);
                airtime=0;
            // not underwater
            } else {
                airtime+=1;
            }
            // tail -- sticks up when slow, bounces with flap, also maybe steers just a little
            tailAngle = bodyAngle+0.8-1.2/(1+2.5*_smoothedSpeed)-0.2*(2+_smoothedSpeed)*(headAngle - bodyAngle);
            // foot -- sticks down when slow, bounces with flap, also maybe steers
            footAngle = bodyAngle-0.4+1.0/(1+25.0*_smoothedSpeed*_smoothedSpeed)-0.4*(headAngle - bodyAngle);
            // mouth position (used for eating detection)
            vertex headComponent = rotateVert(45,0,headAngle);
            pengMouthLocation.x = x + pengNeckBone.x + headComponent.x;
            pengMouthLocation.y = y + pengNeckBone.y + headComponent.y;
        }

        void render(int scrollX) {
            draw_absolute_vector(x-scrollX,y,0);
            load_abs_pos(x-scrollX,y);
            //physicsBox::render(scrollX);
            // blink
            pengHead.selectFrame(_headFrame);
            // initial render to memory
            pengBotBod.rotatedRender(x-scrollX,y,bodyAngle-0.011*(MIDDLE_FLAP-_flapCount));
            pengTopBod.rotatedRender(x-scrollX,y,bodyAngle-0.011*(MIDDLE_FLAP-_flapCount));
            pengNeckBone = rotateVert(45,11,bodyAngle-0.011*(MIDDLE_FLAP-_flapCount));
            pengHead.rotatedRender(x-scrollX+pengNeckBone.x,y+pengNeckBone.y,headAngle);
            vertex pengTailBone = rotateVert(-60,27,bodyAngle);
            pengTail.rotatedRender(x-scrollX+pengTailBone.x,y+pengTailBone.y,tailAngle);
            vertex pengFootBone = rotateVert(-48,3,bodyAngle);
            pengFoot.rotatedRender(x-scrollX+pengFootBone.x,y+pengFootBone.y,footAngle);
            // hackery to connect body parts
            size_t pengBotBodEnd = pengBotBod.getCurrNumVertices()-1;
            size_t pengTopBodEnd = pengTopBod.getCurrNumVertices()-1;
            size_t pengHeadEnd = pengHead.getCurrNumVertices()-1;
            size_t pengTailEnd = pengTail.getCurrNumVertices()-1;
            size_t pengFootEnd = pengFoot.getCurrNumVertices()-1;
            // connect head to top body squishily
            pengHead.renderedFrame[0].bright = 0xC0;
            pengTopBod.renderedFrame[pengTopBodEnd].x = (pengTopBod.renderedFrame[pengTopBodEnd-1].x + pengTopBod.renderedFrame[pengTopBodEnd].x + pengHead.renderedFrame[0].x)/3;
            pengTopBod.renderedFrame[pengTopBodEnd].y = (pengTopBod.renderedFrame[pengTopBodEnd-1].y + pengTopBod.renderedFrame[pengTopBodEnd].y + pengHead.renderedFrame[0].y)/3;
            // connect bot body to head squishily
            pengBotBod.renderedFrame[0].bright = 0xC0;
            pengBotBod.renderedFrame[0].x = (pengBotBod.renderedFrame[0].x + pengHead.renderedFrame[pengHeadEnd].x)/2;
            pengBotBod.renderedFrame[0].y = (pengBotBod.renderedFrame[0].y + pengHead.renderedFrame[pengHeadEnd].y)/2;
            // connect tail to bot body squishily
            pengTail.renderedFrame[0].bright = 0xC0;
            pengTail.renderedFrame[0].x = (2*pengTail.renderedFrame[0].x + 1*pengBotBod.renderedFrame[pengBotBodEnd].x)/3;
            pengTail.renderedFrame[0].y = (2*pengTail.renderedFrame[0].y + 1*pengBotBod.renderedFrame[pengBotBodEnd].y)/3;
            // connect top body to tail squishily
            pengTopBod.renderedFrame[0].x = pengTail.renderedFrame[pengTailEnd].x;
            pengTopBod.renderedFrame[0].y = pengTail.renderedFrame[pengTailEnd].y;
            // connect foot to bot bod squishily
            pengFoot.renderedFrame[0].x = pengBotBod.renderedFrame[pengBotBodEnd].x;
            pengFoot.renderedFrame[0].y = pengBotBod.renderedFrame[pengBotBodEnd].y;
            pengFoot.renderedFrame[1].x = 0.3*pengFoot.renderedFrame[0].x+0.7*pengFoot.renderedFrame[1].x;
            pengFoot.renderedFrame[1].y = 0.3*pengFoot.renderedFrame[0].y+0.7*pengFoot.renderedFrame[1].y;
            pengFoot.renderedFrame[pengFootEnd].x = pengBotBod.renderedFrame[pengBotBodEnd-1].x;
            pengFoot.renderedFrame[pengFootEnd].y = pengBotBod.renderedFrame[pengBotBodEnd-1].y;
            pengFoot.renderedFrame[pengFootEnd-1].x = 0.3*pengFoot.renderedFrame[pengFootEnd].x+0.7*pengFoot.renderedFrame[pengFootEnd-1].x;
            pengFoot.renderedFrame[pengFootEnd-1].y = 0.3*pengFoot.renderedFrame[pengFootEnd].y+0.7*pengFoot.renderedFrame[pengFootEnd-1].y;
            // actually draw penguin
            pengTopBod.drawFromMem();
            pengHead.drawFromMem();
            pengBotBod.drawFromMem();
            pengTail.drawFromMem();
            //pengEye.rotatedRender(x-scrollX+pengNeckBone.x,y+pengNeckBone.y,headAngle);
            pengFoot.drawFromMem();
            // flipper
            pengFlip.selectFrame(_flapCount);
            vertex pengFlipperBone = rotateVert(33,14,bodyAngle-0.05*(headAngle-bodyAngle));
            pengFlip.rotatedRender(x-scrollX+pengFlipperBone.x,y+pengFlipperBone.y,bodyAngle-0.05*(headAngle-bodyAngle));
            //draw_absolute_vector(pengMouthLocation.x-scrollX,pengMouthLocation.y,0xFF);
        }

    private:
        int _stunTimer=0;
        float _stunSpin=0;
        float tailAngle;
        float footAngle;
        float _smoothedSpeed=0;
        float _pastSpeeds[8] = {0,0,0,0,0,0,0,0};
        int _flapCount=13;
        int _flapCharge=0;
        int _headFrame=0;
        vertex pengNeckBone;
        flapState _flapState = returning;
        headState _headState = unblinking;
};

int noms = 0;
class fish {
    public:
        int alive = 0;
        float x = -100;
        float y = 0;
        float maxSpeed = 2;
        int distanceToPeng=0;
        fish() : fish(55, 40) {}
        fish(int width, int height) : _width(width), _height(height) {}
        void render(int scrollX) {
            //drawRect(x-scrollX,y,_width,_height,0x40);
            fishSprite.render(x-scrollX,y);
        }
        int update(int pengX, int pengY) {
            int eaten = 0;
            if (alive) {
                distanceToPeng = dumbDistance(x,y,pengX,pengY);
                float vx = 1.8*(x-pengX)/(1+distanceToPeng);
                vx = (vx < -maxSpeed) ? -maxSpeed : vx; 
                vx = (vx > maxSpeed) ? maxSpeed : vx; 
                x+=vx;
                if (x-_width/2<pengX && pengX<x+_width/2 && y-_height/2<pengY && pengY<y+_height/2) {
                    noms +=1;
                    kill();
                    eaten = 1;
                    return eaten;
                }
            }
            return 0;
        }
        void kill() {
            alive = 0;
        }
    private:
        int _width,_height;
};

////////////////
// Game Logic //
////////////////
penguin myPeng = penguin();
#define MAX_NUM_FISHES 10
fish fishes[MAX_NUM_FISHES];
int fishAliveCount;

void updateMenu() {
    #ifdef SIMULATION
        static int bootup = 600;
    #else
        static int bootup = 4000;
    #endif
    bootup-=1;
    char tmp[20];
    load_abs_pos(-300,100);
    sprintf(tmp,"leviathan");
    drawString(tmp,0xa0,12);
    renderMountains(frameCount/10);
    sun.render(-240,475);
    if (bootup == 0) {
        global_game_state = GAME;
        frameCount = 0;
    }
    frameCount++;
    draw_end_buffer();
}

void updateGame() {    
    static float scrollX = 0;
    static int eattimer = 0;
    // ===== Game logic updates =====
    // fish
    fishAliveCount = 0;
    int closestFishDistance = 1000;
    int eaten = 0;
    for (int i=0; i<MAX_NUM_FISHES; i++) {
        fish& aFish = fishes[i];
        if (aFish.alive) {
            fishAliveCount+=1;
            eaten |= aFish.update(myPeng.pengMouthLocation.x, myPeng.pengMouthLocation.y);
            if (eaten) {
                eattimer = 100;
            }
            closestFishDistance = (closestFishDistance < aFish.distanceToPeng) ? closestFishDistance : aFish.distanceToPeng;
            if (abs(aFish.x-myPeng.x)>2000) {
                aFish.kill();
                //char tmp[100];
                //sprintf(tmp,"FISH: %d      ",(int) (aFish.x));
                //sendString(tmp);
                //char tmp2[100];
                //sprintf(tmp2,"PENG: %d\n\r",(int) (myPeng.x));
                //sendString(tmp2);
                }
                // despawn if far away

        }
    }
    for (int i=0; fishAliveCount<4; i++) {
        fish& aFish = fishes[i];
        if (!aFish.alive) {
            fishAliveCount+=1;
            aFish.x = myPeng.x +512+100;
            aFish.maxSpeed = 5+i*0.8;
            if (myPeng.vx < 0) {
                aFish.x = myPeng.x -512-100;
                }
            aFish.y = -200 + 60*i;
            aFish.alive = 1;  
        }
    }
    // penguin
    myPeng.headAngle = M_PI*inputs.joyX/4+myPeng.bodyAngle;
    //myPeng.headAngle = 0.7*M_PI*inputs.joyY+myPeng.bodyAngle+0.6;
    myPeng.update(inputs.buttons & 0x1, closestFishDistance, eaten);

    // ===== Render stuff =====
    // scrolling
    #define APPROX_SCROLL_BOUNDARY 280 // so like when the penguin is close to APPROX_SCROLL_BOUNDARY units away from the center of the screen, the scrolling becomes powerful
    float multiplier = abs((APPROX_SCROLL_BOUNDARY*APPROX_SCROLL_BOUNDARY)-((myPeng.x-scrollX)*(myPeng.x-scrollX)));
    scrollX += 300*(myPeng.x-scrollX)*(0.00001+1.0/(1+multiplier));
    //renderMountains(scrollX/16);
    renderWaves(scrollX);

    // dynamic elemensts
    myPeng.render(scrollX);
    for (int i=0; i<MAX_NUM_FISHES; i++) {
        fish& aFish = fishes[i];
        if (aFish.alive) {
            if ((aFish.x-scrollX)>-580 && (aFish.x-scrollX)<580) aFish.render(scrollX);
        }
    }
    // background
    sun.render(-240+(frameCount>>10),475+(frameCount>>12));
    
    // print buttons
    if (inputs.buttons>1) {
        char tmp[100];
        load_abs_pos(300,470);
        sprintf(tmp,"b%03x",(int)inputs.buttons);
        drawString(tmp,0x40,4);
    }

    // print nom
    if (eattimer>0) {
        char tmp[100];
        load_abs_pos(-40,0);
        sprintf(tmp,"nom");
        drawString(tmp,0xa0,(noms+1)<10?(noms+1):10);
        eattimer-=1;
    }
    // print nom
    if (airtime>50) {
        char tmp[100];
        load_abs_pos(-30,400);
        sprintf(tmp,"fly");
        drawString(tmp,0xa0,5);
        eattimer-=1;
    }
    char tmp[100];
    load_abs_pos(-80,-480);
    sprintf(tmp,"noms: %d",noms);
    drawString(tmp,0x40,4);
    frameCount++;
    draw_end_buffer();
}

///////////////
// Main Code //
///////////////
int main() {
    // Startup
    initialize_input_output();
    sendString("Welcome to HMC Vector Arcade!\n\r");
    global_game_state = MENU;

    // Render loop
    while (1) {

        start_frame_timer();

        inputs = get_inputs();
        switch(global_game_state) {
            case MENU:
                updateMenu();
                break;
            case GAME:
                updateGame();
        }
        
        //if (timer_done()) sendString("Frame computation too long!\n\r");

        while(!is_halted()) {} // wait until frame has finished drawing, if it hasn't already
        draw_buffer_switch();

        //if (timer_done()) sendString("Frame render too long!\n\r");
        while (!timer_done()) {}
    }
}
