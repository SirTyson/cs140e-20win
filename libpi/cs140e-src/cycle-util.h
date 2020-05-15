// engler, cs140e: simple utilities for using the cycle counters.
#ifndef __CYCLE_UTIL_H__
#define __CYCLE_UTIL_H__
#include "rpi.h"
#include "cs140e-src/cycle-count.h"

// delay <n> cycles assuming we started counting cycles at time
// <start>.
static inline void delay_ncycles(unsigned start, unsigned n) {
    // this handles wrap-around, but doing so adds two instructions,
    // which makes the delay not as tight.
    while((cycle_cnt_read() - start) < n)
        ;
}

#define ON0 0x2020001c
#define CLR0 0x20200028

// static inline void
// init_gpio (void)
// {
//     asm (
//         "mov r5,#0x20000000\n\t"
//         "orr r5,r5,#0x00200000\n\t"
//         "orr r5,r5,#0x0000001c\n\t"
//         "mov r6,#0x20000000\n\t"
//         "orr r6,r6,#0x00200000\n\t"
//         "orr r6,r6,#0x00000028"
//     );
// }


static inline void
gpio_write_raw (unsigned pin, unsigned v)
{
    // if (v)
    //     *(volatile unsigned *)(r5) = (1 << pin);
    // else
    //     *(volatile unsigned *)(r6) = (1 << pin);
}

static inline void
gpio_write_on0 (unsigned val)
{
    //asm ("str r1, [r5]");
    *(volatile unsigned *)(ON0) = val;
}

static inline void 
gpio_write_clr0 (unsigned val)
{
    //asm ("str r1, [r6]");
    *(volatile unsigned *)(CLR0) = val;
}

static inline void 
write_on0_until (unsigned val, unsigned start, unsigned ncycles)
{
    gpio_write_on0 (val);
    delay_ncycles (start, ncycles);
}

static inline void 
write_clr0_until (unsigned val, unsigned start, unsigned ncycles)
{
    gpio_write_clr0 (val);
    delay_ncycles (start, ncycles);
}

// write value <v> to GPIO <pin>: return when <ncycles> have passed since
// time <start>
//  
// can make a big difference to make an inlined version of GPIO_WRITE_RAW
// after you do: if you look at the generated assembly, there's some
// room for tricks.  also, remove the pin check.
static inline void
write_cyc_until(unsigned pin, unsigned v, unsigned start, unsigned ncycles) {
    gpio_write_raw (pin, v);
    delay_ncycles (start, ncycles);
}   

// We don't seem to need this, so removing for the moment.  can
// drop in and experiment.
//
// we can't do direct loads and stores when faking things out, 
// so redefine it when running on Unix
// #ifdef RPI_UNIX
#if 1
#   define GPIO_READ_RAW gpio_read
#else
    // trying to eek out what we can in terms of speed so that we
    // do not get 
    static inline unsigned GPIO_READ_RAW(unsigned pin) {
        unsigned GPIO_BASE  = 0x20200000UL;
    
        volatile unsigned *gpio_lev0  = (void*)(GPIO_BASE + 0x34);
        unsigned off = (pin%32);
        // this can sign extend?
        return (*gpio_lev0 >> off) & 1;
    }
#endif

// wait until <pin>=<v> or until we spin for <ncycles>
static inline int 
wait_until_cyc(unsigned pin, unsigned v, unsigned s, unsigned ncycles) {
    while(1) {
        // use GPIO_READ_RAW
        if(gpio_read(pin) == v)
            return 1;
        if((cycle_cnt_read() - s) >= ncycles)
            return 0;
    }
}
#endif
