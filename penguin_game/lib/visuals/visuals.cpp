#include <string.h>
#include "visuals.hpp"
#include "../../include/rawio.h"

////////////
// Shapes //
////////////
void drawSquare(int16_t x, int16_t y, int16_t brightness, int size) {
    draw_absolute_vector(x-size/2, y+size/2, 0);
    draw_absolute_vector(x+size/2, y+size/2, brightness);
    draw_absolute_vector(x+size/2, y-size/2, brightness);
    draw_absolute_vector(x-size/2, y-size/2, brightness);
    draw_absolute_vector(x-size/2, y+size/2, brightness);
}

void drawRect(int16_t x, int16_t y, int width_x, int width_y, int16_t brightness) {
    draw_absolute_vector(x-width_x/2, y+width_y/2, 0);
    draw_absolute_vector(x+width_x/2, y+width_y/2, brightness);
    draw_absolute_vector(x+width_x/2, y-width_y/2, brightness);
    draw_absolute_vector(x-width_x/2, y-width_y/2, brightness);
    draw_absolute_vector(x-width_x/2, y+width_y/2, brightness);
}


//////////////////
// 2D GPU Stuff //
//////////////////
vertex rotateVert(int x, int y, float angle) {
    vertex rotatedVert;
    rotatedVert.x = x*cos(angle) - y*sin(angle);
    rotatedVert.y = x*sin(angle) + y*cos(angle);
    return rotatedVert;
} 

////////////////
// Characters //
////////////////
const int16_t n0x[5] = {0,4,0,-4,6};
const int16_t n0y[5] = {6,0,-6,0,0};
const int n0blank[5] = {0,0,0,0,1};

const int16_t n1x[3] = {2,0,4};
const int16_t n1y[3] = {6,-6,0};
const int n1blank[3] = {1,0,1};

const int16_t n2x[7] = {0,4,0,-4,0,4,2};
const int16_t n2y[7] = {6,0,-3,0,-3,0,0};
const int n2blank[7] = {1,0,0,0,0,0,1};

const int16_t n3x[7] = {0,4,0,-4,0,4,2};
const int16_t n3y[7] = {6,0,-6,0,3,0,-3};
const int n3blank[7] = {1,0,0,0,1,0,1};

const int16_t n4x[6] = {0,0,4,0,0,2};
const int16_t n4y[6] = {6,-3,0,3,-6,0};
const int n4blank[6] = {1,0,0,1,0,1};

const int16_t n5x[6] = {4,0,-4,0,4,2};
const int16_t n5y[6] = {0,3,0,3,0,-6};
const int n5blank[6] = {0,0,0,0,0,1};

const int16_t n6x[6] = {0,4,0,-4,0,6};
const int16_t n6y[6] = {3,0,-3,0,6,-6};
const int n6blank[6] = {1,0,0,0,0,1};

const int16_t n7x[4] = {0,4,0,2};
const int16_t n7y[4] = {6,0,-6,0};
const int n7blank[4] = {1,0,0,1};

const int16_t n8x[7] = {0,4,0,-4,0,4,2};
const int16_t n8y[7] = {6,0,-6,0,3,0,-3};
const int n8blank[7] = {0,0,0,0,1,0,1};

const int16_t n9x[6] = {4,-4,0,4,0,2};
const int16_t n9y[6] = {3,0,3,0,-6,0};
const int n9blank[6] = {1,0,0,0,0,1};

const int16_t ax[7] = {0,2,2,0,-4,4,2};
const int16_t ay[7] = {4,2,-2,-4,2,0,-2};
const int ablank[7] = {0,0,0,0,1,0,1};

const int16_t bx[12] = {0,3,1,0,-1,-3,3,1,0,-1,-3,6};
const int16_t by[12] = {6,0,-1,-1,-1,0,0,-1,-1,-1,0,0};
const int bblank[12] = {0,0,0,0,0,0,1,0,0,0,0,1};

const int16_t cx[5] = {0,4,-4,4,2};
const int16_t cy[5] = {6,0,-6,0,0};
const int cblank[5] = {0,0,1,0,1};

const int16_t dx[7] = {0,2,2,0,-2,-2,6};
const int16_t dy[7] = {6,0,-2,-2,-2,0,0};
const int dblank[7] = {0,0,0,0,0,0,1};

const int16_t ex[7] = {4,-4,0,4,-1,-3,6};
const int16_t ey[7] = {0,0,6,0,-3,0,-3};
const int eblank[7] = {0,1,0,0,1,0,1};

const int16_t fx[5] = {0,4,-1,-3,6};
const int16_t fy[5] = {6,0,-3,0,-3};
const int fblank[5] = {0,0,1,0,1};

const int16_t gx[8] = {0,4,0,-2,2,0,-4,6};
const int16_t gy[8] = {6,0,-2,-2,0,-2,0,0};
const int gblank[8] = {0,0,0,1,0,0,0,1};

const int16_t hx[6] = {0,0,4,0,0,2};
const int16_t hy[6] = {6,-3,0,3,-6,0};
const int hblank[6] = {0,1,0,1,0,1};

const int16_t ix[6] = {4,-4,4,-2,0,4};
const int16_t iy[6] = {0,6,0,0,-6,0};
const int iblank[6] = {0,1,0,1,0,1};

const int16_t jx[5] = {0,2,2,0,2};
const int16_t jy[5] = {2,-2,0,6,-6};
const int jblank[5] = {1,0,0,0,1};

const int16_t kx[5] = {0,3,-3,3,3};
const int16_t ky[5] = {6,0,-3,-3,0};
const int kblank[5] = {0,1,0,0,1};

const int16_t lx[4] = {0,0,4,2};
const int16_t ly[4] = {6,-6,0,0};
const int lblank[4] = {1,0,0,1};

const int16_t mx[5] = {0,2,2,0,2};
const int16_t my[5] = {6,-2,2,-6,0};
const int mblank[5] = {0,0,0,0,1};

const int16_t nx[4] = {0,4,0,2};
const int16_t ny[4] = {6,-6,6,-6};
const int nblank[4] = {0,0,0,1};

const int16_t ox[5] = {0, 4, 0, -4, 6};
const int16_t oy[5] = {6, 0, -6, 0, 0};
const int oblank[5] = {0,0,0,0,1};

const int16_t px[5] = {0,4,0,-4,6};
const int16_t py[5] = {6,0,-3,0,-3};
const int pblank[5] = {0,0,0,0,1};

const int16_t qx[8] = {0,4,0,-2,-2,2,2,2};
const int16_t qy[8] = {6,0,-4,-2,0,2,-2,0};
const int qblank[8] = {0,0,0,0,0,1,0,1};

const int16_t rx[7] = {0,4,0,-4,1,3,2};
const int16_t ry[7] = {6,0,-3,0,0,-3,0};
const int rblank[7] = {0,0,0,0,1,0,1};

const int16_t sx[6] = {4,0,-4,0,4,2};
const int16_t sy[6] = {0,3,0,3,0,-6};
const int sblank[6] = {0,0,0,0,0,1};

const int16_t tx[5] = {0,4,-2,0,4};
const int16_t ty[5] = {6,0,0,-6,0};
const int tblank[5] = {1,0,1,0,1};

const int16_t ux[5] = {0,0,4,0,2};
const int16_t uy[5] = {6,-6,0,6,-6};
const int ublank[5] = {1,0,0,0,1};

const int16_t vx[4] = {0,2,2,2};
const int16_t vy[4] = {6,-6,6,-6};
const int vblank[4] = {1,0,0,1};

const int16_t wx[6] = {0,0,3,3,0,2};
const int16_t wy[6] = {6,-6,2,-2,6,-6};
const int wblank[6] = {1,0,0,0,0,1};

const int16_t xx[4] = {4,-4,4,2};
const int16_t xy[4] = {6,0,-6,0};
const int xblank[4] = {0,1,0,1};

const int16_t yx[6] = {2,0,-2,4,-2,4};
const int16_t yy[6] = {0,4,2,0,-2,-4};
const int yblank[6] = {1,0,0,1,0,1};

const int16_t zx[5] = {0,4,-4,4,2};
const int16_t zy[5] = {6,0,-6,0,0};
const int zblank[5] = {1,0,0,0,1};

const int16_t spacex[1] = {6};
const int16_t spacey[1] = {0};
const int spaceblank[1] = {1};

const int16_t minusx[3] = {0,4,2};
const int16_t minusy[3] = {3,0,-3};
const int minusblank[3] = {1,0,1};

void drawChar(const char c, int16_t brightness, int size) {
    // Font data was extracted from Atari Gravitar
    int len = 0;
    const int16_t* x;
    const int16_t* y;
    const int* blank;
    switch(c) {
        case '0':
            len=5;
            x=n0x;
            y=n0y;
            blank=n0blank;
            break;
        case '1':
            len=3;
            x=n1x;
            y=n1y;
            blank=n1blank;
            break;
        case '2':
            len=7;
            x=n2x;
            y=n2y;
            blank=n2blank;
            break;
        case '3':
            len=7;
            x=n3x;
            y=n3y;
            blank=n3blank;
            break;
        case '4':
            len=6;
            x=n4x;
            y=n4y;
            blank=n4blank;
            break;
        case '5':
            len=6;
            x=n5x;
            y=n5y;
            blank=n5blank;
            break;
        case '6':
            len=6;
            x=n6x;
            y=n6y;
            blank=n6blank;
            break;
        case '7':
            len=4;
            x=n7x;
            y=n7y;
            blank=n7blank;
            break;
        case '8':
            len=7;
            x=n8x;
            y=n8y;
            blank=n8blank;
            break;
        case '9':
            len=6;
            x=n9x;
            y=n9y;
            blank=n9blank;
            break;
        case 'a':
            len=7;
            x=ax;
            y=ay;
            blank=ablank;
            break;
        case 'b':
            len=12;
            x=bx;
            y=by;
            blank=bblank;
            break;
        case 'c':
            len=5;
            x=cx;
            y=cy;
            blank=cblank;
            break;
        case 'd':
            len=7;
            x=dx;
            y=dy;
            blank=dblank;
            break;
        case 'e':
            len=7;
            x=ex;
            y=ey;
            blank=eblank;
            break;
        case 'f':
            len=5;
            x=fx;
            y=fy;
            blank=fblank;
            break;
        case 'g':
            len=8;
            x=gx;
            y=gy;
            blank=gblank;
            break;
        case 'h':
            len=6;
            x=hx;
            y=hy;
            blank=hblank;
            break;
        case 'i':
            len=6;
            x=ix;
            y=iy;
            blank=iblank;
            break;
        case 'j':
            len=5;
            x=jx;
            y=jy;
            blank=jblank;
            break;
        case 'k':
            len=5;
            x=kx;
            y=ky;
            blank=kblank;
            break;
        case 'l':
            len=4;
            x=lx;
            y=ly;
            blank=lblank;
            break;
        case 'm':
            len=5;
            x=mx;
            y=my;
            blank=mblank;
            break;
        case 'n':
            len=4;
            x=nx;
            y=ny;
            blank=nblank;
            break;
        case 'o':
            len=5;
            x=ox;
            y=oy;
            blank=oblank;
            break;
        case 'p':
            len=5;
            x=px;
            y=py;
            blank=pblank;
            break;
        case 'q':
            len=8;
            x=qx;
            y=qy;
            blank=qblank;
            break;
        case 'r':
            len=7;
            x=rx;
            y=ry;
            blank=rblank;
            break;
        case 's':
            len=6;
            x=sx;
            y=sy;
            blank=sblank;
            break;
        case 't':
            len=5;
            x=tx;
            y=ty;
            blank=tblank;
            break;
        case 'u':
            len=5;
            x=ux;
            y=uy;
            blank=ublank;
            break;
        case 'v':
            len=4;
            x=vx;
            y=vy;
            blank=vblank;
            break;
        case 'w':
            len=6;
            x=wx;
            y=wy;
            blank=wblank;
            break;
        case 'x':
            len=4;
            x=xx;
            y=xy;
            blank=xblank;
            break;
        case 'y':
            len=6;
            x=yx;
            y=yy;
            blank=yblank;
            break;
        case 'z':
            len=5;
            x=zx;
            y=zy;
            blank=zblank;
            break;
        case '-':
            len=3;
            x=minusx;
            y=minusy;
            blank=minusblank;
            break;
        default:
            len=1;
            x=spacex;
            y=spacey;
            blank=spaceblank;
            break;
    }
    for (int i=0;i<len;i++) {
        draw_relative_vector(size*x[i],size*y[i],blank[i] ? 0 : brightness);
    }
}

void drawString(const char* string, int16_t brightness, int size) {
    int len = strlen(string);
    for (int i=0; i<len; i++) {
        drawChar(string[i],brightness,size);
    }
}