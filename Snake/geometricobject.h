typedef struct tPoint{
    unsigned char x;
    unsigned char y;
}POINT;

#define MAX_POINTS 16

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

typedef struct{
    unsigned char width;
    unsigned char height;
    unsigned char* data;
}sprite;