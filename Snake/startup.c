/*
 * 	startup.c
 *
 */
 
 #include <defines.h>
 #include <delay.h>
 #include <asciidisplay.h>
 #include <graphicdisplay.h>
 #include <keyb.h>
 
__attribute__((naked)) __attribute__((section (".start_section")) )
void startup ( void )
{
__asm__ volatile(" LDR R0,=0x2001C000\n");		/* set stack */
__asm__ volatile(" MOV SP,R0\n");
__asm__ volatile(" BL main\n");					/* call main */
__asm__ volatile(".L1: B .L1\n");				/* never return */
}

typedef struct tPoint{
    unsigned char x;
    unsigned char y;
}POINT;

#define MAX_POINTS 20

typedef struct tGeometry{
    int numpoints;
    int sizex;
    int sizey;
    POINT px[MAX_POINTS];
} GEOMETRY, *PGEOMETRY;

typedef struct tObj{
    PGEOMETRY geo;
    int dirx,diry;
    int posx,posy;
    void (* draw) (struct tObj *);
    void (* clear) (struct tObj *);
    void (* move) (struct tObj *);
    void (* set_speed) (struct tObj *, int, int);
} OBJECT, *POBJECT;

GEOMETRY ball_geometry = {
    12,
    4,4,
    {
              {0,1},{0,2},
        {1,0},{1,1},{1,2},{1,3},
        {2,0},{2,1},{2,2},{2,3},
              {3,1},{3,2}
    }
};

void set_object_speed(POBJECT o, int speedx, int speedy){
    o->dirx = speedx;
    o->diry = speedy;
}

void draw_object(POBJECT o){
    for (int i = 0; i<o->geo->numpoints; i++)
        pixel(o->geo->px[i].x+o->posx,o->geo->px[i].y+o->posy,1);
        
}

void clear_object(POBJECT o){
    for (int i = 0; i < o->geo->numpoints; i++)
        pixel(o->geo->px[i].x+o->posx,o->geo->px[i].y+o->posy,0);
        
}

void move_object(POBJECT o){
    clear_object(o);
    
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

static OBJECT ball = 
{
    &ball_geometry,
    0,0,
    1,1,
    draw_object,
    clear_object,
    move_object,
    set_object_speed
};



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

void main(int argc, char **argv){
     POBJECT p = &ball;
    char c;
    
    init_app();
    graphic_init();
    
    #ifndef SIMULATOR
        graphic_clear_screen(); //JAG FÅR VARA MED
    #endif
    
    //p->set_speed(p,2,0);
    while(1){
        p->move(p);
        delay_milli(40);
        c = keyb();
        switch(c){
            case 6: p->set_speed(p,2,0); break;
            case 4: p->set_speed(p,-2,0); break;
            case 2: p->set_speed(p,0,-2); break;
            case 8: p->set_speed(p,0,2); break;
             
        }
    }
}

