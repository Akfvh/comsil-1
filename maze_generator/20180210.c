#include "20180210.h"


/*---- main starts ----*/
int main(int argc, char* argv[]) {
	int i, j;
	int width, height;
	FILE* fp;
	//scanf("%d", &width);
	//scanf("%d", &height);

	/* input */
	//WIDTH = width;
	//HEIGHT = height;  i will have input with argc/argv
	if(argc != 3){
		printf("usage: ./maze [WIDTH] [HEIGHT]\n");
		exit(1);
	}
		   

	WIDTH = atoi(argv[1]);
	HEIGHT = atoi(argv[2]);

	/* dynamic allocation */
	cell = (Cell**)malloc(sizeof(Cell*) * HEIGHT);
	for (i = 0; i < HEIGHT; i++)
		cell[i] = (Cell*)malloc(sizeof(Cell) * WIDTH);

	srand((unsigned int)time(NULL)); //randomize

	/* open file */
	if ((fp = fopen("maze.maz", "wt")) == NULL)
		perror("file opening error");

	/* print first row(walls)*/
	fprintf(fp, "+");
	for (i = 0; i < WIDTH; i++) 
		fprintf(fp, "-+");	//top walls
	fprintf(fp, "\n");

	/* first row initialization: set all walls */
	for (i = 0; i < WIDTH; i++) {
		cell[0][i].group = ++cIndex;
		cell[0][i].right_wall = 1;
		cell[0][i].down_wall = 1;
	}

	/* first carve */
	rand_delete_right(cell[0]); 
	rand_delete_down(cell[0]); 

	/* print first row */
	print_right_wall(cell[0], fp);
	print_down_wall(cell[0], fp);
	/* ----- done with the first row ----*/

	/* ---- second ~ second last rows ----*/
	for (i = 1; i < HEIGHT; i++) {

		/* initialization for not grouped cells */
		for (j = 0; j < WIDTH; j++) {
			if (cell[i - 1][j].down_wall == 1)	//if created by down_carve
				cell[i][j].group = ++cIndex;	//inherit group
			else
				cell[i][j].group = cell[i - 1][j].group;	//else, new unique id

			cell[i][j].right_wall = 1;	//set all walls
			cell[i][j].down_wall = 1;
		}

		/* carve */
		if (i == HEIGHT - 1) break;  //last row carving is special case
		rand_delete_right(cell[i]);
		rand_delete_down(cell[i]);

		/* print */
		print_right_wall(cell[i], fp);
		print_down_wall(cell[i], fp);
		
	}

	/* last row */
	rand_delete_right2(cell[i]);
	print_right_wall(cell[i], fp);
	print_down_wall(cell[i], fp);
	
	/*---- routine done, dealloc memories and close fp ----*/

	for (i = 0; i < HEIGHT; i++) 
		free(cell[i]);
	free(cell);
	fclose(fp);

	return 0;
}
/* ----- end of main ----- */


void rand_delete_right(Cell* cell) {
	int i, j, tmp;

	for (i = 0; i < WIDTH - 1; i++) {	//most right wall is fixed

		/* meet different group */
		if (cell[i].group != cell[i + 1].group) {
			cell[i].right_wall = rand() % 2; // 50% carve

			/* if wall deleted */
			if (cell[i].right_wall == 0) {
				tmp = cell[i + 1].group;	// tmpstore group of cell to be updated

				//merge rule: merge to cell[i] + update all cells in same group being updated
				for (j = 0; j < WIDTH; j++)
					if (cell[j].group == tmp) 
						cell[j].group = cell[i].group;
			}
		}
	}
	return;
}

void rand_delete_right2(Cell* cell) {	// last row carving routine
	int i, j, tmp;

	for (i = 0; i < WIDTH - 1; i++) {
		if (cell[i].group != cell[i + 1].group) {	// meet different group
			cell[i].right_wall = 0;					// 100% carve
			tmp = cell[i + 1].group;
			for (j = i+1; j < WIDTH; j++)
				if (cell[j].group == tmp)
					cell[j].group = cell[i].group;
		}
	}
	return;
}


void rand_delete_down(Cell* cell) {
	int i;
	int m_flag = 0;	//merged_flag
	int counter = 1;	//counter for same group

	for (i = 0; i < WIDTH; i++) {

		/* last cell */
		if (i == WIDTH - 1) {
			if (counter == 1 || m_flag == 0) //diff group w/ former || smae group but not yet carved in former
				cell[i - (rand() % counter)].down_wall = 0; //100% carve one member cell
			else
				cell[i].down_wall = rand() % 2;   //50% carve self
			break;
		}

		/* not last cell */
		cell[i].down_wall = rand() % 2;   //50% carve
		
		if (cell[i].down_wall == 0)  //if carved
			m_flag = 1;             //set merged_flag

		if (cell[i].group != cell[i + 1].group) {   //diff group met
			if (m_flag == 0)    //not yet carved
				cell[i - (rand() % counter)].down_wall = 0; //100% carve one member down_wall
		
			counter = 1;    // reset counter  
			m_flag = 0;		// reset merged_flag
			continue;
		}
		counter++;
	}
	return;
}

void print_right_wall(Cell* cell, FILE* fp) {
	int i;
	fprintf(fp, "|"); //fixed first left wall
	for (i = 0; i < WIDTH; i++) {
		if (cell[i].right_wall) fprintf(fp, " |");
		else fprintf(fp, "  ");
	}
	fprintf(fp, "\n");

	return;
}

void print_down_wall(Cell* cell, FILE* fp) {
	int i;
	fprintf(fp, "+");    //fixed first left corner
	
	for (i = 0; i < WIDTH; i++) {
		if (cell[i].down_wall) fprintf(fp, "-+");
		else fprintf(fp, " +");
	}
	fprintf(fp, "\n");
	
	return;
}

//to do: minimalize times of write to file? -> sprintf to buf and write only once
//		 set carve_bias? -> n%100, if(n < bias_in_percent) carve
