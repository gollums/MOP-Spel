#include <defines.h>
#include <keyb.h>

void activate_row(int i){
    *GPIO_D_ODR_HIGH = 1 << (i+4);
}

int read_coloumn(){
    int j; 
    char input = *GPIO_D_IDR_HIGH;
    for(j = 0; j<4 ; j++){
        if (input&(1<<j)){
            return j;
        }
    }
    return -1;
}

/*rader in, kolumner ut*/
unsigned char keyb(void){
    unsigned char key_value[] = {0x06,0x5B,0x4F,0x77,0x66,0x6D,0x7D,0x7C,0x07,0x7F,0x6F,0x39,0x79,0x3F,0x71,0xBF};
    unsigned char keyVal[] = {1,2,3,0xA,4,5,6,0xB,7,8,9,0xC,0xE,0,0xF,0xD};
    
        
    int row, coloumn;
    for (row = 0; row < 4; row++){
        activate_row(row);
        coloumn = read_coloumn();
        if (coloumn != -1){
            return keyVal[coloumn+row*4];
        }
    }
    return 0x00;
}

void out7seg(unsigned char c){
    
    *GPIO_D_ODR_LOW = c; //0x00 -> 0xFF
    
}