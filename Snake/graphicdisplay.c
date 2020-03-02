#include <defines.h>
#include <asciidisplay.h>
#include <graphicdisplay.h>


void graphic_init(void){
    graphic_ctrl_bit_set(B_E);
    delay_mikro(10);
    graphic_ctrl_bit_clear(B_CS1|B_CS2|B_RST|B_E);
    delay_milli(30);
    graphic_ctrl_bit_set(B_RST);
    delay_milli(100);
    graphic_write_command(LCD_OFF, B_CS1|B_CS2);
    graphic_write_command(LCD_ON, B_CS1|B_CS2);
    graphic_write_command(LCD_DISP_START, B_CS1|B_CS2);
    graphic_write_command(LCD_SET_ADD, B_CS1|B_CS2);
    graphic_write_command(LCD_SET_PAGE, B_CS1|B_CS2);
    select_controller(0);
}

void graphic_ctrl_bit_clear(uint8_t x){
    //*GPIO_E_ODR_LOW &= ( B_SELECT | ~x);
    //ascii_ctrl_bit_clear(x);
    uint8_t c;
    c =*GPIO_E_ODR_LOW;
    c &= ~x;
    c &= ~B_SELECT;
    *GPIO_E_ODR_LOW=c;

}

void graphic_ctrl_bit_set(uint8_t x){
    //*GPIO_E_ODR_LOW |= (B_SELECT | x );
    //ascii_ctrl_bit_set(x);
    uint8_t c;
    c =*GPIO_E_ODR_LOW;
    c |= x; 
    c &=(~B_SELECT);
    *GPIO_E_ODR_LOW= c;
}

void select_controller(uint8_t controller){
    switch(controller){
        case 0: 
            graphic_ctrl_bit_clear(B_CS1|B_CS2);
            break;
        
        case B_CS1:
            graphic_ctrl_bit_set(B_CS1);
            graphic_ctrl_bit_clear(B_CS2);
            break;
        
        case B_CS2:
            graphic_ctrl_bit_clear(B_CS1);
            graphic_ctrl_bit_set(B_CS2);
            break;
            
        case B_CS1 | B_CS2:
            graphic_ctrl_bit_set(B_CS1|B_CS2);
            break;
    }
}

void graphic_wait_ready(void){
    graphic_ctrl_bit_clear(B_E);
    *GPIO_E_MODER = 0x00005555;
    graphic_ctrl_bit_clear(B_RS);
    graphic_ctrl_bit_set(B_RW);
    delay_500ns();
    while(1){
        graphic_ctrl_bit_set(B_E);
        delay_500ns();
        graphic_ctrl_bit_clear(B_E);
        delay_500ns();
        //if ((*GPIO_D_IDR_HIGH & LCD_BUSY) == 0) break; //Fastnar här...
    }
    //graphic_ctrl_bit_set(B_E); // Ej på föreläsning
    *GPIO_E_MODER = 0x55555555;
    
}

unsigned char graphic_read(unsigned char controller){
    graphic_ctrl_bit_clear(B_E);
    *GPIO_E_MODER = 0x00005555;
    graphic_ctrl_bit_set(B_RS|B_RW);
    select_controller(controller);              //EJ i Boken
    delay_500ns();
    graphic_ctrl_bit_set(B_E);
    delay_500ns();
    unsigned char retval = *GPIO_E_IDR_HIGH;
    graphic_ctrl_bit_clear(B_E);
    *GPIO_E_MODER = 0x55555555;
    
    if (controller & B_CS1){
        select_controller(B_CS1);
        //graphic_wait_ready();
    }
    if (controller & B_CS2){
        select_controller(B_CS2);
        //graphic_wait_ready();
    }
    return retval;
}

uint8_t graphic_read_data(uint8_t controller){
    (void) graphic_read(controller);
    return graphic_read(controller);
}

void graphic_write(uint8_t value, uint8_t controller){
    *GPIO_E_ODR_HIGH = value;
    select_controller(controller);
    delay_500ns();
    graphic_ctrl_bit_set(B_E);
    delay_500ns();
    graphic_ctrl_bit_clear(B_E);
    
    if(controller & B_CS1){
        select_controller(B_CS1);
        //graphic_wait_ready();
    }
    if(controller & B_CS2){
        select_controller(B_CS2);
        //graphic_wait_ready();
    }
    
    /* INTE på föreläsning*/
    *GPIO_E_ODR_HIGH = 0;
    graphic_ctrl_bit_set(B_E);
    select_controller(0);
      
}

void graphic_write_command(uint8_t cmd, uint8_t controller){
    graphic_ctrl_bit_clear(B_E);
    select_controller(controller);
    graphic_ctrl_bit_clear(B_RS);
    graphic_ctrl_bit_clear(B_RW);
    graphic_write(cmd, controller);
}

void graphic_write_data(uint8_t data, uint8_t controller){
    graphic_ctrl_bit_clear(B_E);
    select_controller(controller);
    graphic_ctrl_bit_set(B_RS);
    graphic_ctrl_bit_clear(B_RW);
    graphic_write(data, controller);
}

void graphic_clear_screen(void){
    for (int page = 0; page < 8; page++){
        graphic_write_command(LCD_SET_PAGE | page, B_CS1|B_CS2);
        graphic_write_command(LCD_SET_ADD | 0, B_CS1|B_CS2);
        for (int add = 0; add < 63; add++){
            graphic_write_data(0, B_CS1|B_CS2);
        }
    }
}

void clear_backbuffer(){
    int i;
    for (i = 0; i < 1024; i++)
        backBuffer[i] = 0;
}

void pixel(unsigned x, unsigned y){
    uint8_t mask;
    int index = 0;
    if ((x >128)||(x <1)||(y >64)||(y <1)) return;

    mask = 1 << ((y-1)%8);
    
    if(x > 64){
        x -= 65;
        index = 512;
    }
    
    index += x + ((y-1)/8)*64;
    
    backBuffer[index] |= mask;
}

void graphic_draw_screen(void){
    uint8_t i,  j, controller, c;
    unsigned int k = 0;
    
    for (c = 0; c < 2; c++){
        controller = (c==0) ? B_CS1 : B_CS2;
        for (j = 0; j < 8; j++){
            graphic_write_command(LCD_SET_PAGE | j, controller);
            graphic_write_command(LCD_SET_ADD | 0, controller);
            for(i = 0; i <= 63; i++, k++){
                graphic_write_data(backBuffer[k],controller);  
            }
        }
    }
}

/*void pixel(unsigned int x, unsigned int y, unsigned int set){
    unsigned int index = 0;
    uint8_t mask, controller;
    
    if ((x >128)||(x <1)||(y >64)||(y <1)) return;

    index = (y-1)/8;
    
    switch((y-1)%8){
        case 0: mask = 1; break;
        case 1: mask = 2; break;
        case 2: mask = 4; break;
        case 3: mask = 8; break;
        case 4: mask = 0x10; break;
        case 5: mask = 0x20; break;
        case 6: mask = 0x40; break;
        case 7: mask = 0x80; break;
    }
    
    if (set == 0){
        mask = ~mask;
    }
    if(x>64){
        controller = B_CS2;
        x = x - 65;
    }else{
        controller = B_CS1;
        x = x - 1;
    }
    
    graphic_write_command(LCD_SET_ADD | x, controller);
    graphic_write_command(LCD_SET_PAGE | index, controller);
    uint8_t temp = graphic_read_data(controller);
    graphic_write_command(LCD_SET_ADD | x, controller);
    
    if (set == 1){
        mask |= temp;
    }else{
        mask &= temp;
    }
    graphic_write_data(mask,controller);  
}*/