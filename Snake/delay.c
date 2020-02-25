#include <defines.h>
#include <delay.h>


void delay_250ns ( void){
    #ifdef SIMULATOR
        return;
    #endif
    /* SystemCoreClock = 168000000 */ 
    *STK_CTRL = 0;
    *STK_LOAD = ( (168/4) -1 );
    *STK_VAL = 0;
    *STK_CTRL = 5;
    while( (*STK_CTRL & 0x10000 )== 0 ) {} 
    *STK_CTRL = 0;
}

void delay_500ns(void){
    delay_250ns();
    delay_250ns();
}

void delay_mikro(unsigned int us){
    while(us--){
        delay_250ns();
        delay_250ns();
        delay_250ns();
        delay_250ns();
    }
}

void delay_milli(unsigned int ms){
    #ifdef  SIMULATOR
        ms = ms / 1000;
        ms++;
    #endif
    
    while(ms--){
        delay_mikro(1000);
    }
}