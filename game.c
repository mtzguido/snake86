#include "img.h"
#include "defs.h"

int x;
unsigned long score=0;
unsigned long hiscore=0;
struct {unsigned int x,y; char st;} snakearr[100] = {0};
int applex = 0; appley = 0;
int bonusx = 0, bonusy = 0, bonust = -1;
int cantbonus = 2;
int wait = 1;
int playing;

/*
 * st = 0, normal
 * st = 1, aca se comio algo
 */

unsigned int snakelen = 0; /* max 100 */
unsigned int snakeoffset = 0; /* 0 <= snakeoffset < snakelen, marca el head */
int snakedir = 0;

#define error() error_f(__LINE__)
void error_f(line)
int line;
{
	printstr("ERR", xy(0,0), COL_WHITE);
	setcur(xy(3,0));
	printlong((long)line, COL_WHITE, 8);

	while (1)
		relax();
}

void drawtile(x,y,img,rot)
int x;
int y;
char img[];
int rot;
{
	int i,j;
	int mi,mj,o;

	switch(rot) {
	default:
	case 0:
		mi = 1;
		mj = 10;
		o = 0;
		break;
	case 1:
		mi = -10;
		mj = 1;
		o = 90;
		break;
	case 2:
		mi = -1;
		mj = -10;
		o = 99;
		break;
	case 3:
		mi = 10;
		mj = -1;
		o = 9;
		break;
	}

	for(i=0;i<TILE_SIZE;i++)
		for(j=0;j<TILE_SIZE;j++)
			drawpix(TILE_SIZE*x+i,
			        TILE_SIZE*y+j,
			        img[mj*j + mi*i + o]
			       );
}

void drawframe(idle)
int idle;
{
	int i;
	int col_hi;

	if(playing && score > hiscore)
		hiscore = score;

	if (!idle) {
		printstr("SC ", xy(0,0), COL_WHITE);
		printlong(score, COL_WHITE, 8);

		printstr("SNAKE86", xy(16,0), COL_WHITE);
	} else {
		/* 40 chars to clear whole row */
		printstr("APRIETA ESPACIO             ",
				xy(0,0), COL_WHITE);
	}

	if (score >= hiscore && playing)
		col_hi = COL_WHITE;
	else
		col_hi = COL_GREY;
	printstr("HI ", xy(29,0), col_hi);
	printlong(hiscore, col_hi, 8);

	for(i=10; i<PIX_H; ++i)
	{
		drawpix(0,i,1);
		drawpix(319,i,1);
	}
	for(i=0; i<PIX_W; ++i)
	{
		drawpix(i,9,1);
		drawpix(i,199,1);
	}
}

void drawblock(x,y,col)
int x;
int y;
char col;
{
	int i,j;
	for(i=0;i<TILE_SIZE;i++)
		for(j=0;j<TILE_SIZE;j++)
			drawpix(TILE_SIZE*x+i,TILE_SIZE*y+j,col);
}

void randpos(x,y)
int *x;
int *y;
{
	do {
		*x = myrand() % TILE_W;
		*y = 1 + (myrand() % (TILE_H -1));
	} while(! freepos(*x, *y));
}

void startup()
{
	int i,j;

	snakelen = 3;
	snakearr[0].x = 7;
	snakearr[0].y = 5;
	snakearr[0].st = 0;
	snakearr[1].x = 6;
	snakearr[1].y = 5;
	snakearr[1].st = 0;
	snakearr[2].x = 5;
	snakearr[2].y = 5;
	snakearr[2].st = 0;
	snakeoffset = 0;
	score = 0;
	snakedir = 0;
	randpos(&applex, &appley);
	randpos(&bonusx, &bonusy);
	bonust = -1;
}

void bonuseffect(bonus)
int bonus;
{
	int a,i;
	switch(bonus) {
	case 0:
		a = snakelen;
		snakelen = max(snakelen-10, 3);
		for(i=snakelen; i<=a; ++i)
			drawblock(snakearr[(snakeoffset+i)%100].x,
			          snakearr[(snakeoffset+i)%100].y,
			          0);
		break;
	case 1:
		score = score * 11 / 10;
		break;
	}
}

void drawscreen(idle)
int idle;
{
	unsigned int i, rot;
	int dx,dy;
	int dx1,dy1;
	int cw;

	for(i = 0; i< snakelen; ++i) {
		dx1 = (snakearr[(i+snakeoffset)%100].x - snakearr[(i+snakeoffset+99)%100].x);
		dy1 = (snakearr[(i+snakeoffset)%100].y - snakearr[(i+snakeoffset+99)%100].y);
		dx = (snakearr[(i+snakeoffset+1)%100].x - snakearr[(i+snakeoffset+99)%100].x);
		dy = (snakearr[(i+snakeoffset+1)%100].y - snakearr[(i+snakeoffset+99)%100].y);

		if(i>0 && i< snakelen-1) {
			/*
			 * We only care about the first position
			 * after the head, all others are already
			 * drawn. Fix this to remove this crappy loop
			 */
			if (i != 1)
				continue;

			if(dx&1)  { /* giro */
				cw = (dx1 > 0 && dy > 0) || (dy1 > 0 && dx < 0) || (dx1 < 0 && dy < 0) || (dy1 < 0 && dx > 0);
				rot = 2*(dx<0) + (dx == dy);
				drawtile(snakearr[(i+snakeoffset)%100].x, snakearr[(i+snakeoffset)%100].y, snake_codo, cw?(2+rot)%4:rot);
			} else {
				/* dx = 2 -> 0
				   dx = -2 -> 2
				   dy = 2 -> 3
				   dy = -2 -> 1 */
				if(dx) rot = (2-dx)/2; else rot = 1 + (dy+2)/2;

				if(snakearr[(i+snakeoffset)%100].st == 1)
					drawtile(snakearr[(i+snakeoffset)%100].x, snakearr[(i+snakeoffset)%100].y, snake_ate, rot);
				else
					drawtile(snakearr[(i+snakeoffset)%100].x, snakearr[(i+snakeoffset)%100].y, snake_st, rot);
			}
		} else if(i==0){ /* head */
			drawtile(snakearr[(i+snakeoffset)%100].x, snakearr[(i+snakeoffset)%100].y, snake_head, snakedir);
		} else { /* tail */
			drawtile(snakearr[(i+snakeoffset)%100].x, snakearr[(i+snakeoffset)%100].y, snake_tail, dx1?1+dx1:2+dy1 );
		}
	}

	if(bonust != -1)
		drawtile(bonusx, bonusy, bonusg[bonust], 0);

	drawtile(applex, appley, apple, 0);
	drawframe(idle);
}

void lost()
{
	int i,j;
	for(i = 5; i < TILE_W - 5; ++i)
		for(j = 5; j < TILE_H - 5; ++j)
			drawblock(i, j, 0);

	for(i = 4; i < TILE_W - 4; ++i) {
		drawblock(i, 4, 1);
		drawblock(i, TILE_H - 5, 1);
	}

	for(j = 4; j < TILE_H - 4; ++j) {
		drawblock(4, j, 1);
		drawblock(TILE_W - 5, j, 1);
	}

	printstr("PERDISTE!", xy(16, 8), 4);
	printstr("TU PUNTAJE: ", xy(10, 12), 0xf);
	setcur(xy(22, 12)); printlong(score, 0x0f, 0);

	printstr("APRIETA ESPACIO", xy(12, 15), 0xf);
	printstr("PARA CONTINUAR",  xy(12, 16), 0xf);

	while(readkey() != ' ')
		relax();
}

int freepos(x,y)
int x;
int y;
{
	int i;
	for(i=0; i<snakelen; ++i)
		if(snakearr[(i+snakeoffset)%100].x == x &&
		   snakearr[(i+snakeoffset)%100].y == y)
			return 0;

	return 1;
}

int movesnake()
{
	int newoff;

	newoff = (snakeoffset+99)%100;
	snakearr[newoff] = snakearr[snakeoffset];
	snakearr[newoff].st = 0;

	switch(snakedir) {
	case 0: snakearr[newoff].x++; break;
	case 1: snakearr[newoff].y++; break;
	case 2: snakearr[newoff].x--; break;
	case 3: snakearr[newoff].y--; break;
	}
	
	if(snakearr[newoff].x == (-1) ||
	   snakearr[newoff].x == TILE_W ||
	   snakearr[newoff].y == 0 ||
	   snakearr[newoff].y == TILE_H ||
	   !freepos(snakearr[newoff].x,snakearr[newoff].y)) {
		return 0;
	} else {
		snakeoffset = newoff;
		if(snakearr[newoff].x == applex && snakearr[newoff].y == appley) {
			snakelen++;
			snakearr[newoff].st = 1;
			randpos(&applex, &appley);
			score += 10;
		} else if(bonust != -1 && snakearr[newoff].x == bonusx && snakearr[newoff].y == bonusy) {
			bonuseffect(bonust);
			randpos(&bonusx, &bonusy);
			bonust = -1;
		}

		drawblock(snakearr[(snakeoffset+snakelen)%100].x,
			  snakearr[(snakeoffset+snakelen)%100].y,
			  0);
		
	}
	
	return 1;
}

int randbonus()
{
	int x = myrand() % 100;

	if (x < 5)
		return 0; /* Shorten */
	else
		return 1; /* 10% score */
}

int manh(x1, y1, x2, y2)
int x1, y1, x2, y2;
{
	return myabs(x1 - x2) + myabs(y1 - y2);
}

void apply_dir(x1, y1, dir, x2, y2)
int x1, y1, dir;
int *x2;
int *y2;
{
	*x2 = x1;
	*y2 = y1;
	switch(dir) {
	case EAST:	(*x2)++; break;
	case SOUTH:	(*y2)++; break;
	case WEST:	(*x2)--; break;
	case NORTH:	(*y2)--; break;
	default: error();
	}

}

int idle_search() {
	int cost[TILE_W][TILE_H];
	int dir[TILE_W][TILE_H];
	int i, j;
	int d = 999;
	int ret = -1;

	int tx, ty; /* target */
	int cx, cy; /* current pos */

	if (bonust != -1) {
		tx = bonusx;
		ty = bonusy;
	} else {
		tx = applex;
		ty = appley;
	}

	cx = snakearr[snakeoffset % 100].x;
	cy = snakearr[snakeoffset % 100].y;

	for (i = 0; i < 4; i++) {
		int zx, zy;
		apply_dir(cx, cy, i, &zx, &zy);

		if (manh(cx, cy, zx, zy) != 1)
			error();

		if (!freepos(zx, zy))
			continue;

		if (zx < 0 || zx >= TILE_W ||
			zy < 1 || zy >= TILE_H)
			continue;

		if (manh(zx, zy, tx, ty) < d) {
			d = manh(zx, zy, tx, ty);
			ret = i;
		}
	}

	/* Die */
	if (ret == -1)
		ret = 0;

	return ret;
}

int lost_idle() {
	int clock;
	int dir;

	while (readkey() != ' ') {
		if(myrand() % 35 == 0 && bonust == -1)
			bonust = randbonus();

		dir = idle_search();
		snakedir = dir;

		if (!movesnake())
			return 1;

		drawscreen(1);
		clock = gettime();
		while(gettime() <= clock + wait)
			relax();
	}

	return 0;
}

void play_loop() {
	int k,prevk,clock;
	int dir;
	int i,j;

	while (1) {
		clock = gettime();
		if(myrand() % 35 == 0 && bonust == -1)
			bonust = randbonus();

		/*
		 * Flush all key reads and use only
		 * the latest one
		 */
		while ((k = readkey()) != -1)
			prevk = k;

		switch (prevk) {
			case 'w': dir=3; break;
			case 'a': dir=2; break;
			case 's': dir=1; break;
			case 'd': dir=0; break;
			case '-': wait++; dir=-1; break;
			case '+': wait--; if (wait<0) wait=0; dir=-1; break;
			default:  dir=-1; break;
		}

		/* Prevent 180° turn */
		if(dir != -1 && ((dir-snakedir)&1))
			snakedir = dir;

		if (movesnake() == 0) {
			/* Lost */
			return;
		} else {
			drawscreen(0);
			while(gettime() <= clock + wait)
				relax();
		}
	}
}

void mainloop()
{
	selftest();
	setvideo();

	while (1) {
		startup();
		resetvideo();
		playing = 1;
		play_loop();

		lost();

		playing = 0;
		while (1) {
			startup();
			resetvideo();
			if (!lost_idle())
				break;
		}
	}
}

