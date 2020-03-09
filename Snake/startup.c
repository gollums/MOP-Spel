/*
 * 	startup.c
 *
 */
 #include <stdlib.h>
 #include <stdio.h>

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
 
static volatile int points, snakeSize, dead;
int seed;

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
    
    /*Dead if hit wall*/
    if((o->posx)<0){
        dead = 1;
    }
    if((o->posx)>128){
        dead = 1;
    }
    if((o->posy)<0){
        dead = 1;
    }
    if((o->posy)>64){
        dead = 1;
    }
    draw_object(o);
}

GEOMETRY snakehead_geometry_LEFT = {
    10,
    4,4,
    {
              {0,1},{0,2},
              {1,1},{1,2},
        {2,0},            {2,3},
        {3,0},{3,1},{3,2},{3,3}
    }
};

GEOMETRY snakehead_geometry_RIGHT = {
    10,
    4,4,
    {
        {0,0},{0,1},{0,2},{0,3},
        {1,0},            {1,3},
              {2,1},{2,2},
              {3,1},{3,2}
    }
};

GEOMETRY snakehead_geometry_DOWN = {
    10,
    4,4,
    {
        {0,0},{0,1},
        {1,0},      {1,2},{1,3},
        {2,0},      {2,2},{2,3},
        {3,0},{3,1}
    }
};

GEOMETRY snakehead_geometry_UP = {
    10,
    4,4,
    {
                    {0,2},{0,3},
        {1,0},{1,1},      {1,3},
        {2,0},{2,1},      {2,3},
                    {3,2},{3,3}
 
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
        {1,0},            {1,3},
        {2,0},            {2,3},
        {3,0},{3,1},{3,2},{3,3}
    }
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

void draw_snake(POBJECT s){
    if (s[0].dirx != 0 || s[0].diry != 0)
            follow_leader(s);
    for (int i = 0; i <= snakeSize - 1; i++){
        s[i].move(&s[i]);
    }
}

void draw_food(POBJECT f){
    f->move(&food_obj);
}

void follow_leader(POBJECT s){
    int next_x = s[0].posx;
    int next_y = s[0].posy;
    for(int i = 0; i < snakeSize; i++){
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
                s[0].geo = &snakehead_geometry_RIGHT;
            }
            break;
        case LEFT:
            if(s[0].dirx == 0){
                s->set_speed(s,-4,0);
                s[0].geo = &snakehead_geometry_LEFT;
            }
            break;
        case UP:
            if(s[0].diry == 0){
                s->set_speed(s,0,-4);
                s[0].geo = &snakehead_geometry_UP;
            }
            break;
        case DOWN:
            if(s[0].diry == 0){
                s->set_speed(s,0,4);
                s[0].geo = &snakehead_geometry_DOWN;
            }
            break;
    }
}

void snake_hit_self(POBJECT s){
    OBJECT head = s[0];
    for(int i = 1; i < snakeSize - 1; i++){
        if (head.posx == s[i].posx && head.posy == s[i].posy) dead = 1;
    }
}

void inc_snake(POBJECT s){
    s[snakeSize] = s[snakeSize-1];
    snakeSize++;
}

void snake_eat_food(POBJECT food, POBJECT snake){
    if(snake[0].posx == food->posx && snake[0].posy == food->posy){
        points++;
        new_pos_food(food, snake);
        inc_snake(snake);
    }
}

void new_pos_food(POBJECT f, POBJECT s){
    int rand_x = randint(128), rand_y = randint(64);
    
    for(int i = 0; i < snakeSize - 1; i++){
        if (s[i].posx != rand_x && s[i].posy != rand_y){

            while(1){
                if (rand_x % 4 != 0) rand_x++;
                if (rand_y % 4 != 0) rand_y++;
                
                if(rand_x % 4 == 0 && rand_y % 4 == 0) break;
            }
            f->posx = rand_x+1;
            f->posy = rand_y+1;
        }
    }
}

int randint(int n) {
    int m = 2^31, a = 1103515245, c = 12345;
    seed = (a * seed + c) % m;
    return seed % n;
}

static int print_score(void)
{
    uint8_t ascii;
    int local_score = points;

    if (local_score > 99)
        local_score = 99;

    /* tiotalen */
    if (local_score >= 10) {
        ascii = '0';

        while (local_score >= 10) {
            ascii++;
            local_score = local_score - 10;
        }

        ascii_write_char(ascii);
    }

    /* ettalet */
    ascii_write_char(local_score + '0');
    if (local_score == 0) return 1;
    else return 0;
}

static void print_score_all(void) {
    ascii_gotoxy(1, 1);
    ascii_print("You have: ");

    if (print_score()) {
        if(dead){
            ascii_gotoxy(1, 2);
            ascii_print("GAME OVER");
        }else{
            ascii_gotoxy(1, 2);
            ascii_print("");
        }
    }
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
            s[i].geo = &snakehead_geometry_RIGHT;
        } else{ 
            s[i].geo = &snakebody_geometry;
        }
        s[i].posx = ((j+1) * 4) + 1;
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

void init_food(POBJECT f){
    f->geo = &food_geometry;
    f->draw = draw_object; 
    f->clear = clear_object;
    f->move = move_object;
    f->set_speed = set_object_speed;
}

void restart_game(){
        clear_backbuffer();
        graphic_draw_screen();
        points = 0;
        snakeSize = 0;
        seed = 123456789;
        dead = 0;
}

void start(){
    ascii_gotoxy(1,2);
    char *s;
    char e[] = "Press 5 to start";
    s = e;
    while(*s)
        ascii_write_char(*s++);
        
    while(keyb() != 5){}
    clear_display();
}

void main(int argc, char **argv){
    int game = 1, restart = 2;
    char key_stroke;
    POBJECT food = &food_obj;
    OBJECT snake[100];
        
    init_app();
    graphic_init();
    ascii_init();
    
    while(game){
        #ifndef SIMULATOR
            graphic_clear_screen(); //JAG FÅR VARA MED
        #endif

        if(restart == 2){
            restart_game();
            restart = 1;
            init_snake(snake);
            init_food(food);
            new_pos_food(food, snake);
        }
        
        start();
        
        while(restart == 1){
            print_score_all();
            
            if (dead){
                restart = 2;
            }
            clear_backbuffer();
            
            draw_snake(snake);
            draw_food(food);

            
            graphic_draw_screen();
            delay_milli(100);
            key_stroke = keyb();
            switch(key_stroke){
                case RIGHT: turn(RIGHT, snake); break; //dirx = +, diry = 0
                case LEFT: turn(LEFT, snake); break; //dirx = -, diry = 0
                case UP: turn(UP, snake); break; //dirx = 0, diry = -
                case DOWN: turn(DOWN, snake); break; //dirx = 0, diry = +
                case QUIT: game = 0; restart = 0; break;
                case RESTART: restart = 2; break;
            }
            
            snake_eat_food(food, snake);
            snake_hit_self(snake);
        }
    }
}

