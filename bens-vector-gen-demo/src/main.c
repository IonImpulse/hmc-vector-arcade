#include <stdint.h>
#include <math.h>

#define PRCI_BASE 0x10008000
typedef struct {
    volatile uint32_t hfrosccfg;
    volatile uint32_t hfxosccfg;
    volatile uint32_t pllcfg;
    volatile uint32_t plloutdiv;
    volatile uint32_t procmoncfg;
} PRCI_type;
#define PRCI ((PRCI_type*) PRCI_BASE)

#define GPIO_BASE 0x10012000
typedef struct {
    volatile uint32_t input_val;
    volatile uint32_t input_en;
    volatile uint32_t output_en;
    volatile uint32_t output_val;
    volatile uint32_t pue;
    volatile uint32_t ds;
    volatile uint32_t rise_ie;
    volatile uint32_t rise_ip;
    volatile uint32_t fall_ie;
    volatile uint32_t fall_ip;
    volatile uint32_t high_ie;
    volatile uint32_t high_ip;
    volatile uint32_t low_ie;
    volatile uint32_t low_ip;
    volatile uint32_t iof_en;
    volatile uint32_t iof_sel;
    volatile uint32_t out_xor;
} GPIO_type;
#define GPIO ((GPIO_type*) GPIO_BASE)

#define PWM0_BASE 0x10015000
#define PWM1_BASE 0x10025000
#define PWM2_BASE 0x10035000
typedef struct {
    volatile uint32_t pwmcfg;
    volatile uint32_t reserved0;
    volatile uint32_t pwmcount;
    volatile uint32_t reserved1;
    volatile uint32_t pwms;
    volatile uint32_t reserved2;
    volatile uint32_t reserved3;
    volatile uint32_t reserved4;
    volatile uint32_t pwmcmp0;
    volatile uint32_t pwmcmp1;
    volatile uint32_t pwmcmp2;
    volatile uint32_t pwmcmp3;
} PWM_type;
#define PWM0 ((PWM_type*) PWM0_BASE)
#define PWM1 ((PWM_type*) PWM1_BASE)
#define PWM2 ((PWM_type*) PWM2_BASE)

void strobeLatch() {
    GPIO->output_val &= ~(1<<11);
    //for (volatile int i=0; i<1e2; i++) {}
    GPIO->output_val |= (1<<11);
    //for (volatile int i=0; i<1e2; i++) {}
}

void sendByte(const char reg, const char byte) {
    uint32_t high_bits = ((byte&0xFC)<<16) | ((byte&0x03)<<12) | 
                         ((reg&0xF)<<2);
    uint32_t low_bits = ~0xFC303C | high_bits;
    GPIO->output_val |= high_bits;
    GPIO->output_val &= low_bits;
    strobeLatch();
}

int rampDir=1;
int x,y;

void updateRamp(int ramp) {
    ramp = (ramp < 0x7FF) ? ramp : 0x7FF;
    ramp = rampDir ? -ramp : ramp;
    ramp ^= 0x800; // invert MSB to be consistent with AM6012PC DAC
    sendByte(6, ramp>>4);
    sendByte(7, ramp&0xF);
    // Strobe Ramp Latches
    // (so that ramp updates all at once)
    GPIO->output_val |= (1<<10);
    GPIO->output_val &= ~(1<<10);
}

void waitRampDone() {
    if (rampDir) while ((GPIO->input_val & 0x2)==0) {}
    else         while ((GPIO->input_val & 0x1)==0) {}
    rampDir ^= 1;
}


int approx_distance( int dx, int dy )
// magic copied from https://flipcode.com/archives/Fast_Approximate_Distance_Functions.shtml
{
   int min, max;

   if ( dx < 0 ) dx = -dx;
   if ( dy < 0 ) dy = -dy;

   if ( dx < dy )
   {
      min = dx;
      max = dy;
   } else {
      min = dy;
      max = dx;
   }
   // coefficients equivalent to ( 123/128 * max ) and ( 51/128 * min )
   return ((( max << 8 ) + ( max << 3 ) - ( max << 4 ) - ( max << 1 ) +
            ( min << 7 ) - ( min << 5 ) + ( min << 3 ) - ( min << 1 )) >> 8 );
} 

void updatePos(const int newX, const int newY) {
    int distance = approx_distance(newX-x+512, newY-y+512);
    x = newX+512;
    y = newY+512;
    if (rampDir==0) {
        sendByte(5, x>>2);
        sendByte(2, y>>2);
        sendByte(4, ((x&0x3)<<2)|(y&0x3));
    } else {
        sendByte(3, x>>2);
        sendByte(0, y>>2);
        sendByte(1, ((x&0x3)<<2)|(y&0x3));
    }
    //int rampSpeed = (15*1024)/distance;
    int rampSpeed = (32*1024)/distance;
    updateRamp(rampSpeed);
    waitRampDone();
}


int main(void) {
    GPIO->iof_en=0; // reset GPIO register to 0
    GPIO->output_en |= (-1 & (~0x3));
    GPIO->input_en |= 0x3;
    GPIO->pue |= 0x3;
    sendByte(0,0x55);
    PRCI->hfrosccfg &= ~5; // set internal ring oscillator to ~12MHz
    PRCI->hfrosccfg |= 5;
    PRCI->hfrosccfg |= 1<<30; // enable ring oscillator
    while ((PRCI->hfrosccfg & 1<<31)==0) {} // wait for oscillator
    PRCI->pllcfg &= ~(1<<17); // feed PLL with internal oscillator
    PRCI->pllcfg &= ~0x7; // set R divisor to 1
    PRCI->pllcfg |= 1<<1;
    PRCI->pllcfg &= ~(31<<4); // set F multiplication to 64
    PRCI->pllcfg |= 31<<4;
    PRCI->pllcfg |= (1<<10); // set Q divisor to 2
    PRCI->pllcfg &= ~(1<<18); // un-bypass PLL
    while ((PRCI->pllcfg & 1<<31)==0) {} // wait for lock
    PRCI->pllcfg |= 1<<16; // select PLL as clock source

    sendByte(0,0xCC);
    for (volatile int i=0; i<1e7; i++) {}
    x=0;
    y=0;
    while (1) {
        
        /*for (volatile int i=-400; i<400; i++) {
            for (volatile int j=0;j<10;j++) {
                if (abs(i%50)>25) 
                    updatePos(100+i, 0);
                else
                    updatePos(50+i, 0);
                updatePos(80+i, 30);
                updatePos(20+i, 40);
                updatePos(-50+i, 10);
                updatePos(-100+i, -50);
                updatePos(-90+i, 0);
                updatePos(-100+i, 50);
                updatePos(-50+i, -10);
                updatePos(-30+i, -20);
                updatePos(20+i, -40);
                updatePos(80+i, -30);
                updatePos(511,511);
                updatePos(-511,511);
                updatePos(-511,-511);
                updatePos(511,-511);
            }
        }*/
        int i = -400;
        updatePos(100+i, 0);
        updatePos(80+i, 30);
        updatePos(20+i, 40);
        updatePos(-50+i, 10);
        updatePos(-100+i, -50);
        updatePos(-90+i, 0);
        updatePos(-100+i, 50);
        updatePos(-50+i, -10);
        updatePos(-30+i, -20);
        updatePos(20+i, -40);
        updatePos(80+i, -30);
        updatePos(511,511);
        updatePos(-511,511);
        updatePos(-511,-511);
        updatePos(511,-511);/*
        i=-400;
        updatePos(100+i, 0);
        updatePos(80+i, 30);
        updatePos(20+i, 40);
        updatePos(-50+i, 10);
        updatePos(-100+i, -50);
        updatePos(-90+i, 0);
        updatePos(-100+i, 50);
        updatePos(-50+i, -10);
        updatePos(-30+i, -20);
        updatePos(20+i, -40);
        updatePos(80+i, -30);
        updatePos(511,511);
        updatePos(-511,511);
        updatePos(-511,-511);
        updatePos(511,-511);
        i=400;
        updatePos(100+i, 0);
        updatePos(80+i, 30);
        updatePos(20+i, 40);
        updatePos(-50+i, 10);
        updatePos(-100+i, -50);
        updatePos(-90+i, 0);
        updatePos(-100+i, 50);
        updatePos(-50+i, -10);
        updatePos(-30+i, -20);
        updatePos(20+i, -40);
        updatePos(80+i, -30);
        updatePos(511,511);
        updatePos(-511,511);
        updatePos(-511,-511);
        updatePos(511,-511);
        updatePos(511,511);
        updatePos(-511,511);
        updatePos(-511,-511);
        updatePos(511,511);
        updatePos(511,-511);
        updatePos(0,0);*/
        /*updatePos(511,0);
        updatePos(0,0);
        updatePos(-256,0);
        updatePos(-384,0);
        updatePos(-448,0);
        updatePos(-480,0);
        updatePos(-496,0);
        updatePos(-504,0);
        updatePos(-508,0);
        updatePos(-510,0);*/
        /*updatePos(10,11);
        updatePos(-10,10);
        updatePos(-10,-10);
        updatePos(10,-10);*/
    }
}
