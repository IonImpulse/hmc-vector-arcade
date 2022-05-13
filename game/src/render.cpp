#include <string>
#include <iostream>

#include "../include/rawio.h"

#include "../include/render.h"

#define HUD_BRIGHT 80

void renderHUD(int hp,int level) {
    //render health
    draw_absolute_vector(-500 ,300, 0    );
    renderA(2);
    renderC(2);
     draw_absolute_vector(-500,330, 0    );
    for (int i = 1; i <= hp; i++ ) {
        draw_relative_vector(40,0,HUD_BRIGHT);
        draw_relative_vector(-40,10,0);
        draw_relative_vector(40,0,HUD_BRIGHT);
        draw_relative_vector(-40,10,0);
    }


    //level
    draw_absolute_vector(-500 ,-500, 0    );
    renderL(2);
    renderE(2);
    renderV(2);
    renderE(2);
    renderL(2);
    renderSPACE(2);
    for (int i = 1; i <= level; i++ ) {
        draw_relative_vector(0,24,HUD_BRIGHT);
        draw_relative_vector(14,-24,0);
    }


}

void renderCharge(bool shooting) {
    if (!shooting) {
        draw_absolute_vector(-490,250, 0    );
        renderLightning(3);
    }
}


void renderTitle(int x, int y, int size) {
    draw_absolute_vector(x     ,        y, 0    );
    renderL(size);
    renderE(size);
    renderV(size);
    renderI(size);
    renderA(size);
    renderT(size);
    renderH(size);
    renderA(size);
    renderN(size);
}

void renderGameOver(int x, int y, int size )  {
        draw_absolute_vector(x     ,        y, 0    );
    renderG(size);
    renderA(size);
    renderM(size);
    renderE(size);
    renderSPACE(size);
    renderO(size);
    renderV(size);
    renderE(size);
    renderR(size);
}
//expects pointer to start in the right place
void renderL(int size) { 
    draw_relative_vector(0     , 12*size , 0    );
    draw_relative_vector(0     , -12*size, HUD_BRIGHT );
    draw_relative_vector(8*size,    0    , HUD_BRIGHT );
    draw_relative_vector(4*size, 0       ,    0 );
}

void renderE(int size) {
    draw_relative_vector(8*size , 0*size , HUD_BRIGHT   );
    draw_relative_vector(-8*size , 0*size , 0   );
    draw_relative_vector(0*size , 12*size , HUD_BRIGHT  );
    draw_relative_vector(8*size , 0*size , HUD_BRIGHT );
    draw_relative_vector(-2*size , -6*size , 0  );
    draw_relative_vector(-6*size , 0*size , HUD_BRIGHT  );
    draw_relative_vector(12*size , -6*size , 0);
}

void renderV( int size) {
    draw_relative_vector(0*size , 12*size , 0 );
    draw_relative_vector(4*size , -12*size , HUD_BRIGHT   );
    draw_relative_vector(4*size , 12*size , HUD_BRIGHT   );
    draw_relative_vector(4*size , -12*size , 0  );
}


void renderI( int size) {
    draw_relative_vector(8*size , 0*size , HUD_BRIGHT  );
    draw_relative_vector(-8*size , 12*size , 0  );
    draw_relative_vector(8*size , 0*size , HUD_BRIGHT  );
    draw_relative_vector(-4*size , 0*size , 0  );
    draw_relative_vector(0*size , -12*size , HUD_BRIGHT  );
    draw_relative_vector(8*size , 0*size , 0  );
}

void renderA( int size) {
    draw_relative_vector(0*size , 8*size , HUD_BRIGHT  );
    draw_relative_vector(4*size , 4*size , HUD_BRIGHT  );
    draw_relative_vector(4*size , -4*size , HUD_BRIGHT  );
    draw_relative_vector(0*size , -8*size , HUD_BRIGHT  );
    draw_relative_vector(-8*size , 4*size , 0  );
    draw_relative_vector(8*size , 0*size , HUD_BRIGHT  );
    draw_relative_vector(4*size , -4*size , 0  );

}

void renderT( int size){ 
    draw_relative_vector(0*size , 12*size , 0  );
    draw_relative_vector(8*size , 0*size , HUD_BRIGHT  );
    draw_relative_vector(-4*size , 0*size , 0  );
    draw_relative_vector(0*size , -12*size , HUD_BRIGHT  );
    draw_relative_vector(8*size , 0*size , 0 );

}

void renderH( int size){ 
    draw_relative_vector(0*size , 12*size , HUD_BRIGHT  );
    draw_relative_vector(0*size , -6*size , 0  );
    draw_relative_vector(8*size , 0*size , HUD_BRIGHT  );
    draw_relative_vector(0*size , 6*size , 0  );
    draw_relative_vector(0*size , -12*size , HUD_BRIGHT  );
    draw_relative_vector(4*size , 0*size , 0  );

}

void renderN( int size){
    draw_relative_vector(0*size , 12*size , HUD_BRIGHT );
    draw_relative_vector(8*size , -12*size , HUD_BRIGHT  );
    draw_relative_vector(0*size , 12*size , HUD_BRIGHT  );
    draw_relative_vector(4*size , -12*size , 0 );
}

void renderG(int size) {
    draw_relative_vector(0*size , 12*size , HUD_BRIGHT  );
    draw_relative_vector(8*size , 0*size , HUD_BRIGHT  );
    draw_relative_vector(0*size , -4*size , HUD_BRIGHT  );
    draw_relative_vector(-4*size , -4*size , 0  );
    draw_relative_vector(4*size , 0*size , HUD_BRIGHT  );
    draw_relative_vector(0*size , -4*size , HUD_BRIGHT  );
    draw_relative_vector(-8*size , 0*size , HUD_BRIGHT  );
    draw_relative_vector(12*size , 0*size , 0  );


}
void renderM(int size) {
    draw_relative_vector(0*size , 12*size , HUD_BRIGHT  );
    draw_relative_vector(4*size , -4*size , HUD_BRIGHT  );
    draw_relative_vector(4*size , 4*size , HUD_BRIGHT  );
    draw_relative_vector(0*size , -12*size , HUD_BRIGHT  );
    draw_relative_vector(4*size , 0*size , 0  );

}
void renderSPACE(int size) {
        draw_relative_vector(8*size , 0*size , HUD_BRIGHT  );
        draw_relative_vector(4*size , 0*size , 0  );


}
void renderO(int size) {
    draw_relative_vector(0*size , 12*size , HUD_BRIGHT  );
    draw_relative_vector(8*size , 0*size , HUD_BRIGHT  );
    draw_relative_vector(0*size , -12*size , HUD_BRIGHT  );
    draw_relative_vector(-8*size , 0*size , HUD_BRIGHT  );
    draw_relative_vector(12*size , 0*size , 0  );

}
void renderR(int size) {
    draw_relative_vector(0*size , 12*size , HUD_BRIGHT  );
    draw_relative_vector(8*size , 0*size , HUD_BRIGHT  );
    draw_relative_vector(0*size , -6*size , HUD_BRIGHT  );
    draw_relative_vector(-8*size , 0*size , HUD_BRIGHT  );
    draw_relative_vector(2*size , 0*size , 0  );
    draw_relative_vector(6*size , -6*size , HUD_BRIGHT  );
    draw_relative_vector(4*size , 0*size , 0  );
}

void renderC(int size) {
    draw_relative_vector(0*size , 12*size , HUD_BRIGHT  );
    draw_relative_vector(8*size , 0*size , HUD_BRIGHT  );
    draw_relative_vector(-8*size , -12*size , 0  );
    draw_relative_vector(8*size , 0*size , HUD_BRIGHT  );
    draw_relative_vector(4*size , 0*size , 0  );

}

void renderLightning (int size) {
    draw_relative_vector(0*size , 12*size , 0  );
    draw_relative_vector(8*size , 0*size , HUD_BRIGHT  );
    draw_relative_vector(-6*size , -3*size , HUD_BRIGHT  );
    draw_relative_vector(4*size , -2*size , HUD_BRIGHT  );
    draw_relative_vector(-2*size , -2*size , HUD_BRIGHT );
    draw_relative_vector(6*size , -3*size , 0 );

}

