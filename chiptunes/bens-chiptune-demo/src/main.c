#include <stdint.h>

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

void sendByte(const char byte) {
    uint32_t high_bits = ((byte&0xC0)<<3) | (byte&0x3F);
    uint32_t low_bits = ~0x63F | high_bits;
    GPIO->output_val |= high_bits;
    GPIO->output_val &= low_bits;
}

void resetCounter() {
    GPIO->output_val |= 1<<11;
    GPIO->output_val &= ~(1<<11);
}

void incrementCounter() {
    GPIO->output_val |= 1<<23;
    GPIO->output_val &= ~(1<<23);
}

void strobeSID() {
    GPIO->output_val &= ~(1<<22);

    for (volatile int i=0; i<1e2; i++) {}
    GPIO->output_val |= 1<<19;
    for (volatile int i=0; i<1e2; i++) {}
    GPIO->output_val &= ~(1<<19);
    for (volatile int i=0; i<1e2; i++) {}

    GPIO->output_val |= 1<<22;
    for (volatile int i=0; i<1e2; i++) {}
}

void setSIDreg(const char addr, const char byte) {
    resetCounter();
    for (int i=0; i<addr; i++) incrementCounter();
    sendByte(byte);
    strobeSID();
}

void startSIDclock() {
    // Pin 19; PWM1_1
    PWM1->pwmcmp0=8;
    PWM1->pwmcmp1=4;
    PWM1->pwmcfg|=1<<9; // pwmzerocmp
    PWM1->pwmcfg|=1<<12; // pwmenalways
    GPIO->iof_en|=1<<19;
    GPIO->iof_sel|=1<<19;
    //GPIO->out_xor|=1<<19;
}
int main(void) {
    GPIO->output_en |= -1;
    startSIDclock();
    while (1) {
        for (int j=0; j<256; j++) {
            setSIDreg(0,0x63); // freq
            setSIDreg(1,0x6);
            setSIDreg(2,0); // pulse width
            setSIDreg(3,j>>4);
            setSIDreg(4,0x41); // waveform control
            setSIDreg(5,0x18); // attack decay
            setSIDreg(6,0x31); // sustain release

            setSIDreg(7,0x14);
            setSIDreg(8,0x6);
            setSIDreg(9,0);
            setSIDreg(10,j>>4);
            setSIDreg(11,0x41);
            setSIDreg(12,0x18);
            setSIDreg(13,0x31);

            setSIDreg(14,0xC8);
            setSIDreg(15,0x4);
            setSIDreg(16,0);
            setSIDreg(17,j>>4);
            setSIDreg(18,0x41);
            setSIDreg(19,0x18);
            setSIDreg(20,0x31);

            setSIDreg(21,0); // filter freq
            setSIDreg(22,0);
            setSIDreg(23,0); // dunno
            setSIDreg(24,0x8F); // filter type, volume
            for (volatile int i=0; i<2e4; i++) {}
        }
        for (volatile int i=0; i<1e6; i++) {}
    }
}