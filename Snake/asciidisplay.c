#include <defines.h>
#include <asciidisplay.h>

void ascii_init(void){ /**/
    /*function set*/
    funtion_set();
    
    /*Display Control*/
    display_control();
    
    /*Clear Display*/
    clear_display();
    
    /*Entry mode set*/
    entry_mode_set();
}

/*adressera ASCII-display, ettställ de bitar som är 1 i x*/
void ascii_ctrl_bit_set(unsigned char x){
    *GPIO_E_ODR_LOW |= ( B_SELECT | x );
}

/*adressera ASCII-display, nollställ de bitar som är 1 i x*/
void ascii_ctrl_bit_clear(unsigned char x){
    *GPIO_E_ODR_LOW &= ( B_SELECT | ~x);
    
}

void ascii_write_cmd(unsigned char command){
    ascii_ctrl_bit_clear(B_RS);
    ascii_ctrl_bit_clear(B_RW);
    ascii_write_controller(command);
}

void ascii_write_data(unsigned char data){
    ascii_ctrl_bit_set(B_RS);
    ascii_ctrl_bit_clear(B_RW);
    ascii_write_controller(data);
}

void ascii_write_controller(unsigned char byte){
    ascii_ctrl_bit_set(B_E);
    *GPIO_E_ODR_HIGH = byte;
    delay_250ns();
    ascii_ctrl_bit_clear(B_E);
}

unsigned char ascii_read_controller(void){
    ascii_ctrl_bit_set(B_E);
    delay_250ns();
    delay_250ns();
    char retVal = *GPIO_E_IDR_HIGH;
    ascii_ctrl_bit_clear(B_E);
    return retVal;
}

unsigned char ascii_read_status(void){
    *(GPIO_E_MODER+0x02) = 0x0000;
    ascii_ctrl_bit_clear(B_RS);
    ascii_ctrl_bit_set(B_RW);
    char retStat = ascii_read_controller(); 
    *(GPIO_E_MODER+0x02) = 0x5555;
    return retStat;
}

unsigned char ascii_read_data(void){
    *(GPIO_E_MODER+0x02) = 0x0000;
    ascii_ctrl_bit_set(B_RS);
    ascii_ctrl_bit_set(B_RW);
    char retData = ascii_read_controller();
    *(GPIO_E_MODER+0x02) = 0x5555;
    return retData;
}

void clear_display(){
    while((ascii_read_status() & 0x80) == 0x80){}
    delay_mikro(8);
    ascii_write_cmd(1); /*clear display*/
    delay_milli(2);
}

void funtion_set(){
    while((ascii_read_status() & 0x80) == 0x80){}
    delay_mikro(8);
    ascii_write_cmd(0x38); //0b00111000
    delay_mikro(40);
}

void display_control(){
    while((ascii_read_status() & 0x80) == 0x80){}
    delay_mikro(8);
    ascii_write_cmd(0xE);
    delay_mikro(40);
}

void entry_mode_set(){
    while((ascii_read_status() & 0x80) == 0x80){}
    delay_mikro(8);
    ascii_write_cmd(6);
    delay_mikro(40);
}

void ascii_write_char(unsigned char c){
    //while((ascii_read_status() & 0x80) == 0x80){} //Kommentera bort om det ska fungera...
    delay_mikro(8);
    ascii_write_data(c);
    delay_mikro(45);
}

void ascii_gotoxy(int x, int y){ /**/
    unsigned char address;
    if(y!=1)
        address = 0x40 + (x-1);
    else address=x-1;
    
    ascii_write_cmd( 0x80 | address);
    delay_mikro( 45 );
}