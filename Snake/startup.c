/*
 * 	startup.c
 *
 */
 
 #include "defines.h"
 #include "delay.h"
 #include "asciidisplay.h"
 #include "graphicdisplay.h"
 #include "keyb.h"
 #include "geometricobject.h"
 
 #include "image.xbm"
 #include "queue.h"
 
 #define RIGHT 6
 #define LEFT 4
 #define DOWN 8
 #define UP 2
 #define QUIT 0xA
 #define RESTART 0xD
 
static volatile int points, snakeSize;

__attribute__((naked)) __attribute__((section (".start_section")) )
void startup ( void )
{
__asm__ volatile(" LDR R0,=0x2001C000\n");		/* set stack */
__asm__ volatile(" MOV SP,R0\n");
__asm__ volatile(" BL main\n");					/* call main */
__asm__ volatile(".L1: B .L1\n");				/* never return */
};

void set_object_speed(POBJECT o, int speedx, int speedy){
    o->dirx = speedx;
    o->diry = speedy;
}

void draw_object(POBJECT o){
    for (int i = 0; i<o->geo->numpoints; i++)
        pixel(o->geo->px[i].x+o->posx,o->geo->px[i].y+o->posy);
        
}

void clear_object(POBJECT o){
    for (int i = 0; i < o->geo->numpoints; i++)
        pixel(o->geo->px[i].x+o->posx,o->geo->px[i].y+o->posy);
        
}

void move_object(POBJECT o){
    //clear_object(o);
    
    o->posx = o->posx + o->dirx;
    o->posy = o->posy + o->diry;
    
    /*Position static if direction makes ball go out of bounds*/
    //Bounce left wall
    if((o->posx+o->dirx)<1){
        o->dirx *= -1;
        o->posx = 1;
    }
    //Bounce right wall
    if((o->posx+o->dirx)>128){
        o->dirx *= -1;
        o->posx=127;
    }
    //bounce upper wall
    if((o->posy+o->diry)<1){
        o->diry *= -1;
        o->posy = 1;
    }
    //bounce lower wall
    if((o->posy+o->diry)>64){
        o->diry *= -1;
        o->posy = 63;
    }
    draw_object(o);
}

GEOMETRY snakehead_geometry = {
    10,
    4,4,
    {
              {0,1},{0,2},
              {1,1},{1,2},
        {2,0},            {2,3},
        {3,0},{3,1},{3,2},{3,3}
    }
};


GEOMETRY snakebody_geometry = {
    16,
    4,4,
    {
        {0,0},{0,1},{0,2},{0,3},
        {1,0},{1,1},{1,2},{1,3},
        {2,0},{2,1},{2,2},{2,3},
        {3,0},{3,1},{3,2},{3,3}
    }
};

GEOMETRY food_geometry = {
    12,
    4,4,
    {
        {0,0},{0,1},{0,2},{0,3},
        {1,0},           {1,3},
        {2,0},           {2,3},
        {3,0},{3,1},{3,2},{3,3}
    }
};

/*
static OBJECT snakebody[100] =   
{
    &snakebody_geometry,
    0,0,
    1,1,
    draw_object,
    clear_object,
    move_object,
    set_object_speed
};
*/
static OBJECT food_obj =   
{
    &food_geometry,
    0,0,
    1,1,
    draw_object,
    clear_object,
    move_object,
    set_object_speed
};

static void load_sprite(sprite* s, unsigned char* data, int width, int height){
    s->width = width;
    s->height = height;
    s->data = data;
}

void draw_sprite(sprite* s, int x, int y){
    int i,j,k, width_in_bytes;
    
    if(s->width % 8 == 0){
        width_in_bytes = s->width/8;
    }else{
        width_in_bytes = s->width/8 + 1;
    }
    
    for (i = 0; i < s->height; i++){
        for (j = 0; j < width_in_bytes; j++){
            unsigned char byte = s->data[i*width_in_bytes +j];
            for (k = 0; k < 8; k++){
                if (byte & (1<<k))
                    pixel(8 * j + k + x + 1, i + y + 1);
            }
        }
    }
}


void draw_snake(POBJECT s){
    for (int i = 0; i <= snakeSize - 1; i++){
        s[i].move(&s[i]);
    }
}

void follow_leader(POBJECT s){
    int next_x = s[0].posx;
    int next_y = s[0].posy;
    for(int i = snakeSize; i > 0; i--){
        int current = s[i].posx;
        s[i].posx = next_x;
        next_x = current;
        
        current = s[i].posy;
        s[i].posy = next_y;
        next_y = current;
    }
}

void turn(int dir, POBJECT s){
    int next_x = 0, next_y = 0;
     switch(dir){
        case RIGHT:
            if(s[0].dirx == 0){
                s->set_speed(s,4,0);
            }
            break;
        case LEFT:
            if(s[0].dirx == 0){
                s->set_speed(s,-4,0);
            }
            break;
        case UP:
            if(s[0].diry == 0){
                s->set_speed(s,0,-4);
            }
            break;
        case DOWN:
            if(s[0].diry == 0){
                s->set_speed(s,0,4);
            }
            break;
    }
    follow_leader(s);
    /*if hit food*/
}

void init_app(void){/**/
    #ifdef USBDM
        /*starta klockor på port D & E*/
        * ( (unsigned long *) 0x40023830) = 0x18;
        __asm volatile("  LDR R0,=0x08000209\n BLX R0 \n");
    #endif
    
    *GPIO_E_MODER = 0x55555555;
    *GPIO_E_OTYPER = 0x00000000;
    *GPIO_E_OSPEEDR = 0x00000000;
    
    *GPIO_D_MODER = 0;
    *GPIO_D_MODER = 0x55000000;
    *GPIO_D_PUPDR = 0;
    *GPIO_D_PUPDR |= 0xAA000000;
    *GPIO_D_OTYPER = 0;
}

void init_snake(POBJECT s){
    int j = 0;
    for (int i = 3; i >= 0; i--, j++){
        if(i == 0){ 
            s[i].geo = &snakehead_geometry;
        } else{ 
            s[i].geo = &snakebody_geometry;
        }
        s[i].posx = (j+1) * 4;
        s[i].posy = 1;
        s[i].dirx = 0;
        s[i].diry = 0;
        s[i].move = move_object;
        s[i].clear = clear_object;
        s[i].draw = draw_object;
        s[i].set_speed = set_object_speed;
        snakeSize++;
    }
}

void restart_game(){
        clear_backbuffer();
        graphic_draw_screen();
        points = 0;
        snakeSize = 0;
}

void main(int argc, char **argv){
    int game = 1, restart = 2;
    char key_stroke;
    
    POBJECT food = &food_obj;
    OBJECT snake[200];
    
        
    init_app();
    graphic_init();
    ascii_init();
    
    while(game){
        #ifndef SIMULATOR
            graphic_clear_screen(); //JAG FÅR VARA MED
        #endif
        
        //load_sprite(&s , image_bits, image_width, image_height);
        //draw_sprite(&s, 20, 20);
        
        //while(keyb() != 5){
            //char[] *s = "You have :" 
            //ascii_write_char(*s++)
        //}
        
        if(restart == 2){
            restart_game();
            restart = 1;
            init_snake(snake);
        }

        while(restart == 1){
            clear_backbuffer();
            //graphic_draw_screen();
            /*KOD HÄR*/
            
            draw_snake(snake);
                        
            graphic_draw_screen();

            //delay_milli(40);
            key_stroke = keyb();
            switch(key_stroke){
                case RIGHT: turn(RIGHT, snake); break; /*dirx = +, diry = 0*/
                case LEFT: turn(LEFT, snake); break; /*dirx = -, diry = 0*/
                case UP: turn(UP, snake); break; /*dirx = 0, diry = -*/
                case DOWN: turn(DOWN, snake); break; /*dirx = 0, diry = +*/
                case QUIT: game = 0; restart = 0; break;
                case RESTART: restart = 2; break;
            }

        }
    }
}

