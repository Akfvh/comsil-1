#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAXLINE 10000

typedef struct Cell{
	int group;
	int down_wall;
	int right_wall;
}Cell;
Cell** cell;

int cIndex=0;
int WIDTH, HEIGHT;
char buf[MAXLINE]={0};

void rand_delete_right(Cell*);
void rand_delete_right2(Cell* cell);
void rand_delete_down(Cell*);
void print_right_wall(Cell*, FILE*);
void print_down_wall(Cell*, FILE*);
