#define xy(x,y) ((x)+((y)<<8))
#define xof(i) ((i)&0xff)
#define yof(i) ((i)>>8)

#define TEXT_W 40
#define TEXT_H 25
#define PIX_W 320
#define PIX_H 200
#define TILE_SIZE 10
#define TILE_W (PIX_W/TILE_SIZE)
#define TILE_H (PIX_H/TILE_SIZE)

#define COL_WHITE 0x0f
#define COL_GREY 0x07

#define EAST	0
#define SOUTH	1
#define WEST	2
#define NORTH	3
