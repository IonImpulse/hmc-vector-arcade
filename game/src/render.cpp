#include <string>
#include <iostream>

#include "../include/rawio.h"

#include "../include/render.h"




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
    draw_relative_vector(0     , -12*size, 1023 );
    draw_relative_vector(8*size,    0    , 1023 );
    draw_relative_vector(4*size, 0       ,    0 );
}

void renderE(int size) {
    draw_relative_vector(8*size , 0*size , 1023   );
    draw_relative_vector(-8*size , 0*size , 0   );
    draw_relative_vector(0*size , 12*size , 1023  );
    draw_relative_vector(8*size , 0*size , 1023 );
    draw_relative_vector(-2*size , -6*size , 0  );
    draw_relative_vector(-6*size , 0*size , 1023  );
    draw_relative_vector(12*size , -6*size , 0);
}

void renderV( int size) {
    draw_relative_vector(0*size , 12*size , 0 );
    draw_relative_vector(4*size , -12*size , 1023   );
    draw_relative_vector(4*size , 12*size , 1023   );
    draw_relative_vector(4*size , -12*size , 0  );
}


void renderI( int size) {
    draw_relative_vector(8*size , 0*size , 1023  );
    draw_relative_vector(-8*size , 12*size , 0  );
    draw_relative_vector(8*size , 0*size , 1023  );
    draw_relative_vector(-4*size , 0*size , 0  );
    draw_relative_vector(0*size , -12*size , 1023  );
    draw_relative_vector(8*size , 0*size , 0  );
}

void renderA( int size) {
    draw_relative_vector(0*size , 8*size , 1023  );
    draw_relative_vector(4*size , 4*size , 1023  );
    draw_relative_vector(4*size , -4*size , 1023  );
    draw_relative_vector(0*size , -8*size , 1023  );
    draw_relative_vector(-8*size , 4*size , 0  );
    draw_relative_vector(8*size , 0*size , 1023  );
    draw_relative_vector(4*size , -4*size , 0  );

}

void renderT( int size){ 
    draw_relative_vector(0*size , 12*size , 0  );
    draw_relative_vector(8*size , 0*size , 1023  );
    draw_relative_vector(-4*size , 0*size , 0  );
    draw_relative_vector(0*size , -12*size , 1023  );
    draw_relative_vector(8*size , 0*size , 0 );

}

void renderH( int size){ 
    draw_relative_vector(0*size , 12*size , 1023  );
    draw_relative_vector(0*size , -6*size , 0  );
    draw_relative_vector(8*size , 0*size , 1023  );
    draw_relative_vector(0*size , 6*size , 0  );
    draw_relative_vector(0*size , -12*size , 1023  );
    draw_relative_vector(4*size , 0*size , 0  );

}

void renderN( int size){
    draw_relative_vector(0*size , 12*size , 1023 );
    draw_relative_vector(8*size , -12*size , 1023  );
    draw_relative_vector(0*size , 12*size , 1023  );
    draw_relative_vector(4*size , -12*size , 0 );
}

void renderG(int size) {
    draw_relative_vector(0*size , 12*size , 1023  );
    draw_relative_vector(8*size , 0*size , 1023  );
    draw_relative_vector(0*size , -4*size , 1023  );
    draw_relative_vector(-4*size , -4*size , 0  );
    draw_relative_vector(4*size , 0*size , 1023  );
    draw_relative_vector(0*size , -4*size , 1023  );
    draw_relative_vector(-8*size , 0*size , 1023  );
    draw_relative_vector(12*size , 0*size , 0  );


}
void renderM(int size) {
    draw_relative_vector(0*size , 12*size , 1023  );
    draw_relative_vector(4*size , -4*size , 1023  );
    draw_relative_vector(4*size , 4*size , 1023  );
    draw_relative_vector(0*size , -12*size , 1023  );
    draw_relative_vector(4*size , 0*size , 0  );

}
void renderSPACE(int size) {
        draw_relative_vector(8*size , 0*size , 1023  );
        draw_relative_vector(4*size , 0*size , 0  );


}
void renderO(int size) {
    draw_relative_vector(0*size , 12*size , 1023  );
    draw_relative_vector(8*size , 0*size , 1023  );
    draw_relative_vector(0*size , -12*size , 1023  );
    draw_relative_vector(-8*size , 0*size , 1023  );
    draw_relative_vector(12*size , 0*size , 0  );

}
void renderR(int size) {
    draw_relative_vector(0*size , 12*size , 1023  );
    draw_relative_vector(8*size , 0*size , 1023  );
    draw_relative_vector(0*size , -6*size , 1023  );
    draw_relative_vector(-8*size , 0*size , 1023  );
    draw_relative_vector(2*size , 0*size , 0  );
    draw_relative_vector(6*size , -6*size , 1023  );
    draw_relative_vector(4*size , 0*size , 0  );


}