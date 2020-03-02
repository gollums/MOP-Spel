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

static OBJECT snakehead =   
{
    &snakehead_geometry,
    0,0,
    1,1,
    draw_object,
    clear_object,
    move_object,
    set_object_speed
};

static OBJECT snakebody[510] =   
{
    &snakebody_geometry,
    0,0,
    1,1,
    draw_object,
    clear_object,
    move_object,
    set_object_speed
};

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

void draw_snake(POBJECT h, POBJECT *b){
    h->move(h);
    for(int i = 0; i < size()-1; i++){ 
        move_object(*b);
        *b++;
    }
}

static volatile int points;

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
    
    points = 0;
}

void init_snake(POBJECT h, POBJECT *b, OBJECT *s){
    h->posx = 16;
    h->posy = 1;
    h->move(h);
    for (int i = 0; i < 3; i++){
        *b = &(*s);
        (*b)->posx = (i+1)*4;
        (*b)->posy = 1;
        move_object(*b);
        insert(*b);
        *b++;
        *s++;
    }
}

void restart_game(POBJECT p){
        clear_backbuffer();
        graphic_draw_screen();
        /*p->set_speed(p,0,0);§
        p->posx = 0;
        p->posy = 0;
        */
}

void main(int argc, char **argv){
    int game = 1, restart = 2;
    char key_stroke;
        
    POBJECT head = &snakehead, food = &food_obj;
    POBJECT body[510];
    
    init_app();
    graphic_init();
    
    while(game){
        #ifndef SIMULATOR
            graphic_clear_screen(); //JAG FÅR VARA MED
        #endif
        
        //load_sprite(&s , image_bits, image_width, image_height);
        //draw_sprite(&s, 20, 20);
        
        while(keyb() != 5){}
        
        if(restart == 2){
            init_snake(head, body, snakebody);
            restart_game(head);
            restart = 1;
        }

        while(restart == 1){
            clear_backbuffer();
            //graphic_draw_screen();
            /*KOD HÄR*/
            
            draw_snake(head, body);
                        
            graphic_draw_screen();

            //delay_milli(40);
            key_stroke = keyb();
            switch(key_stroke){
                case RIGHT: turn(RIGHT, head, body); break; /*dirx = +, diry = 0*/
                case LEFT: turn(LEFT, head, body); break; /*dirx = -, diry = 0*/
                case UP: turn(UP, head, body); break; /*dirx = 0, diry = -*/
                case DOWN: turn(DOWN, head, body); break; /*dirx = 0, diry = +*/
                case QUIT: game = 0; restart = 0; break;
                case RESTART: restart = 2; break;
            }
        }
    }
}

void turn(int dir, POBJECT h ,POBJECT *b){
    switch(dir){
        case RIGHT:
            if(h->dirx == 0){
                h->set_speed(h,4,0);
                /*for(int i = 0; i < size()-1; i++){
                    (*b)->set_speed(*b,4,0);
                    *b++;
                }*/
            }
            break;
        case LEFT:
            if(h->dirx == 0){
                h->set_speed(h,-4,0);
                
            }
            break;
        case UP:
            if(h->diry == 0){
                h->set_speed(h,0,-4);
            }
            break;
        case DOWN:
            if(h->diry == 0){
                h->set_speed(h,0,4);
                
            }
            break;
    }
}

