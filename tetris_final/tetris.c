#include "tetris.h"

static struct sigaction act, oact;
time_t T_start, T_end;
//clock_t  DS_start, DS_end;
long long space=0;
int space_index=0;
double DS_duration=0;
struct timeval DS_start, DS_end;

int main(){
	int exit=0;

	initscr();
	noecho();
	keypad(stdscr, TRUE);	

	srand((unsigned int)time(NULL));

	createRankList();

	while(!exit){
		clear();
		switch(menu()){
		case MENU_PLAY: play(); break;
		case MENU_RANK: rank(); break;
		case MENU_REC_PLAY: recommendedPlay(); break;
		case MENU_EXIT: exit=1; break;
		default: break;
		}
	}

	endwin();
	system("clear");
	return 0;
}

void InitTetris(){
	int i,j;

	for(j=0;j<HEIGHT;j++)
		for(i=0;i<WIDTH;i++)
			field[j][i]=0;

	nextBlock[0]=rand()%7;
	nextBlock[1]=rand()%7;
	nextBlock[2]=rand()%7;

	blockRotate=0;
	blockY=-1;
	blockX=WIDTH/2-2;
	score=0;	
	gameOver=0;
	timed_out=0;

	DrawOutline();
	DrawField();
	//DrawBlock(blockY,blockX,nextBlock[0],blockRotate,' ');

	recRoot = (RecNode*)malloc(sizeof(RecNode));

	recRoot->score = 0;
	recRoot->lv = 0;
	recRoot->flag = 0;

	modified_recommend(recRoot, field);

	DrawBlockWithFeatures(blockY, blockX, nextBlock[0], blockRotate);
	
	//free(recRoot);
	DrawNextBlock(nextBlock);
	PrintScore(score);
	free(recRoot);
}

void DrawOutline(){	
	/* 블럭이 떨어지는 공간의 태두리를 그린다.*/
	DrawBox(0,0,HEIGHT,WIDTH);

	/* next block을 보여주는 공간의 태두리를 그린다.*/
	move(2,WIDTH+10);
	printw("NEXT BLOCK");
	DrawBox(3,WIDTH+10,4,8);

	//for additional next block
	move(8, WIDTH+10);
	DrawBox(9, WIDTH+10, 4, 8);

	/* score를 보여주는 공간의 태두리를 그린다.*/
	move(15,WIDTH+10);
	printw("SCORE");
	DrawBox(16,WIDTH+10,1,8);
}

int GetCommand(){
	int command;
	command = wgetch(stdscr);
	switch(command){
	case KEY_UP:
		break;
	case KEY_DOWN:
		break;
	case KEY_LEFT:
		break;
	case KEY_RIGHT:
		break;
	case ' ':	/* space key*/
		/*fall block*/
		break;
	case 'q':
	case 'Q':
		command = QUIT;
		break;
	default:
		command = NOTHING;
		break;
	}
	return command;
}

int ProcessCommand(int command){
	int ret=1;
	int drawFlag=0;
	switch(command){
	case QUIT:
		ret = QUIT;
		break;
	case KEY_UP:
		if((drawFlag = CheckToMove(field,nextBlock[0],(blockRotate+1)%4,blockY,blockX)))
			blockRotate=(blockRotate+1)%4;
		break;
	case KEY_DOWN:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)))
			blockY++;
		break;
	case KEY_RIGHT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX+1)))
			blockX++;
		break;
	case KEY_LEFT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX-1)))
			blockX--;
		break;
	default:
		break;
	}
	if(drawFlag) DrawChange(field,command,nextBlock[0],blockRotate,blockY,blockX);
	return ret;	
}

void DrawField(){
	int i,j;
	for(j=0;j<HEIGHT;j++){
		move(j+1,1);
		for(i=0;i<WIDTH;i++){
			if(field[j][i]==1){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(".");
		}
	}
}


void PrintScore(int score){
	move(17,WIDTH+11);
	printw("%8d",score);
}

void DrawNextBlock(int *nextBlock){
	int i, j;
	for( i = 0; i < 4; i++ ){
		move(4+i,WIDTH+13);
		for( j = 0; j < 4; j++ ){
			if( block[nextBlock[1]][0][i][j] == 1 ){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
	}

	for(i=0; i<4; i++){
		move(10+i, WIDTH+13);
		for(j=0; j<4; j++){
			if(block[nextBlock[2]][0][i][j] == 1){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
	}
}

void DrawBlock(int y, int x, int blockID,int blockRotate,char tile){
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++){
			if(block[blockID][blockRotate][i][j]==1 && i+y>=0){
				move(i+y+1,j+x+1);
				attron(A_REVERSE);
				printw("%c",tile);
				attroff(A_REVERSE);
			}
		}

	move(HEIGHT,WIDTH+10);
}


void DrawBox(int y,int x, int height, int width){
	int i,j;
	move(y,x);
	addch(ACS_ULCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_URCORNER);
	for(j=0;j<height;j++){
		move(y+j+1,x);
		addch(ACS_VLINE);
		move(y+j+1,x+width+1);
		addch(ACS_VLINE);
	}
	move(y+j+1,x);
	addch(ACS_LLCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_LRCORNER);
}

void play(){
	int command;
	clear();
	act.sa_handler = BlockDown;
	sigaction(SIGALRM,&act,&oact);
	InitTetris();
	do{
		if(timed_out==0){
			alarm(1);
			timed_out=1;
		}

		command = GetCommand();
		if(ProcessCommand(command)==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();

			return;
		}
	}while(!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	while(getch()==KEY_DOWN);	/* edited: holding key_down causing no stop */
	newRank(score);
}

char menu(){
	printw("1. play\n");
	printw("2. rank\n");
	printw("3. recommended play\n");
	printw("4. exit\n");
	return wgetch(stdscr);
}

/////////////////////////첫주차 실습에서 구현해야 할 함수/////////////////////////

int CheckToMove(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	int i, j, tmpX, tmpY;
	for(i=0; i<4; i++){
		for(j=0; j<4; j++){	//size of block space
			if(block[currentBlock][blockRotate][i][j]){	//for existing part of the block
				tmpX = blockX + j;			//make temporary block in game
				tmpY = blockY + i;
			

			if(f[tmpY][tmpX] == 1 || (tmpX >= WIDTH) || (tmpX < 0) || (tmpY >= HEIGHT)|| (tmpY < 0)) 
				return 0; //out of range condition
			}
		}
	}
	return 1;
}

void DrawChange(char f[HEIGHT][WIDTH],int command,int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	int tmp_y = blockY;
	//while(CheckToMove(field, nextBlock[0], blockRotate, tmp_y+1, blockX)) tmp_y++;

	//1. 이전 블록 정보를 찾는다. ProcessCommand의 switch문을 참조할 것
	//2. 이전 블록 정보를 지운다. DrawBlock함수 참조할 것.
	switch(command){	//find the previous original block with switch(command)
		case KEY_UP:
			while(CheckToMove(f, currentBlock, (blockRotate+3)%4, tmp_y+1, blockX)) tmp_y++;
			deleteOriginal(currentBlock, (blockRotate+3)%4, blockY, blockX);
			deleteOriginal(currentBlock, (blockRotate+3)%4, tmp_y, blockX);
			break;
		case KEY_DOWN:
			while(CheckToMove(f, currentBlock, blockRotate, tmp_y, blockX)) tmp_y++;
			deleteOriginal(currentBlock, blockRotate, blockY-1, blockX);
			deleteOriginal(currentBlock, blockRotate, tmp_y-1, blockX);
			break;
		case KEY_LEFT:
			while(CheckToMove(f, currentBlock, blockRotate, tmp_y+1, blockX+1)) tmp_y++;
			deleteOriginal(currentBlock, blockRotate, blockY, blockX+1);
			deleteOriginal(currentBlock, blockRotate, tmp_y, blockX+1);
			break;
		case KEY_RIGHT:
			while(CheckToMove(f, currentBlock, blockRotate, tmp_y+1, blockX-1)) tmp_y++;
			deleteOriginal(currentBlock, blockRotate, blockY, blockX-1);
			deleteOriginal(currentBlock, blockRotate, tmp_y, blockX-1);
			break;
		default: break;
	}
	//3. 새로운 블록 정보를 그린다. 
	//DrawBlock(blockY, blockX, currentBlock, blockRotate, ' ');	//draw a new block
	DrawBlockWithFeatures(blockY, blockX, currentBlock, blockRotate);
	move(HEIGHT+10, WIDTH+10);					//and move the cursor out of the field.
}

void deleteOriginal(int currentBlock, int blockRotate, int blockY, int blockX){	//deletes the original block before changing its position
	int i, j;
	for(i=0; i<4; i++){
		for(j=0; j<4; j++){
			if(block[currentBlock][blockRotate][i][j]==1){	//on parameters of the original block
				move(i+blockY+1, j+blockX+1);			//move cursor
				printw(".");				//change the color-reversed ' ' with "."
			}
		}
	}
	
	return;
	
}

void BlockDown(int sig){
	// user code
	int extra_score;
	if(CheckToMove(field, nextBlock[0], blockRotate, blockY+1, blockX)){	//check if it can move down 1
		blockY++;	//yes -> move down
		DrawChange(field, KEY_DOWN, nextBlock[0], blockRotate, blockY, blockX);	//draw change
	}else{
		if(blockY == -1) gameOver = 1;	//when there is no room for new block, set gameOver

		extra_score = AddBlockToField(field, nextBlock[0],blockRotate, blockY, blockX);	//when block reached ground, add block to field

		score += DeleteLine(field);	//check for full lines
		score += extra_score;

		nextBlock[0] = nextBlock[1];	//current block = next block
		nextBlock[1] = nextBlock[2];	//random shape id for next block
		nextBlock[2] = rand()%7;	

		recRoot = (RecNode*)malloc(sizeof(RecNode));
		
		recRoot->score = 0;
		recRoot->lv = 0;
		
		recRoot->flag = 0;
		recRoot->tmpX = 0;
		recRoot->tmpY = 0;
		recRoot->tmpR = 0;
		
		gettimeofday(&DS_start, NULL);
		modified_recommend(recRoot, field);
		gettimeofday(&DS_end, NULL);
		DS_duration += (DS_end.tv_sec - DS_start.tv_sec) + ((DS_end.tv_usec - DS_start.tv_usec)/1000000.0);

		free(recRoot);
		blockRotate = 0;		//initialization
		blockY = -1;
		blockX = WIDTH/2 - 2;

		DrawNextBlock(nextBlock);		//draw next
		PrintScore(score);
		DrawField();
	}	
	//강의자료 p26-27의 플로우차트를 참고한다.
	timed_out = 0;				//for auto block drop
}

int AddBlockToField(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){	
	// user code
	//Block이 추가된 영역의 필드값을 바꾼다.

	int touched=0;
	int i, j;
	
	/* block status: cannot move down, either bc stacked blocks or touching the bottom */

	for(i=0; i<4; i++){
		for(j=0; j<4; j++){
			if(block[currentBlock][blockRotate][i][j]){	//testing for bits of the big block

				/*part of block shape is set*/

				//if(blockY+i == HEIGHT-1) touched++;	//if the part set is located at the bottom of the field, touch++

				/* edited: misunderstood the statement */

				if((blockY+i == HEIGHT-1) || (f[blockY+i+1][blockX+j])) touched++;	//if the bottom side of the block is 1.touching the field. 2.touching the frame of the box, touched++
				f[blockY+i][blockX+j] = 1;		//the corresponding field is set 
			}
		}
	}
	return touched * 10;
}	

int DeleteLine(char f[HEIGHT][WIDTH]){
	// user code
	int i, j, k, count=0;
	for(i=0; i<HEIGHT; i++){
		for(j=0; j<WIDTH; j++){
			if(!f[i][j]) break;
		}

		if(j == WIDTH){ 	// if i is filled line
			count++;
			for(k=i; k>0; k--){	//starting from filled line, replace with upper lines.
				for(j=0; j<WIDTH; j++){
					f[k][j] = f[k-1][j];
				}
			}
		}
	}

	return count * count * 100;	//return score
	//1. 필드를 탐색하여, 꽉 찬 구간이 있는지 탐색한다.
	//2. 꽉 찬 구간이 있으면 해당 구간을 지운다. 즉, 해당 구간으로 필드값을 한칸씩 내린다.
}

///////////////////////////////////////////////////////////////////////////

void DrawShadow(int y, int x, int blockID,int blockRotate){
	// user code
	int y_limit = y;
	while(CheckToMove(field, nextBlock[0], blockRotate, y_limit+1, x)) y_limit++;
	DrawBlock(y_limit, x, blockID, blockRotate, '/');
	return;
}

void DrawBlockWithFeatures(int y, int x, int blockID, int blockRotate){
	DrawRecommend(recommendY, recommendX, blockID, recommendR);
	DrawBlock(y, x, blockID, blockRotate, ' ');
	DrawShadow(y, x, blockID, blockRotate);
	return;
}

void createRankList(){
	// 목적: Input파일인 "rank.txt"에서 랭킹 정보를 읽어들임, 읽어들인 정보로 랭킹 목록 생성
	// 1. "rank.txt"열기
	// 2. 파일에서 랭킹정보 읽어오기
	// 3. LinkedList로 저장
	// 4. 파일 닫기
	FILE *fp;
	int total, tmpScore;
	char tmpName[NAMELEN];
	Node* new;
	Node* prev;	

	//1. 파일 열기
	fp = fopen("rank.txt", "r");
	if(!fp) return;
	
	//printw("open file complete\n");

	// 2. 정보읽어오기
	fscanf(fp, "%d", &total);
	g_total = total;
	//printw("total %d elements\n", total);

	while(feof(fp)==0){
		fscanf(fp, "%s %d", tmpName, &tmpScore);
		
		//printw("%s %d\n", tmpName, tmpScore);
		//create new node with given info
		new = (Node*)malloc(sizeof(Node));
		strcpy(new->name, tmpName);
		new->score = tmpScore;
		new->link = NULL;
		//printw("%s, %d\n", new->name, new->score);

		prev = head;
		//add the new node to the list.
		if(head == NULL) {
			head = new;
			continue;
		}
		while(prev->link)
			prev = prev->link;
		prev->link = new->link;
		prev->link = new;
		
	}
		//insertion done
			
	/* int fscanf(FILE* stream, const char* format, ...);
	stream:데이터를 읽어올 스트림의 FILE 객체를 가리키는 파일포인터
	format: 형식지정자 등등
	변수의 주소: 포인터
	return: 성공할 경우, fscanf 함수는 읽어들인 데이터의 수를 리턴, 실패하면 EOF리턴 */
	// EOF(End Of File): 실제로 이 값은 -1을 나타냄, EOF가 나타날때까지 입력받아오는 if문
	// 4. 파일닫기
	fclose(fp);
}

void rank(){
	//목적: rank 메뉴를 출력하고 점수 순으로 X부터~Y까지 출력함
	//1. 문자열 초기화
	int X=1, Y=g_total, ch, count=1;
	clear();

	Node* walker = head;
	//2. printw()로 3개의 메뉴출력
	printw("1. list rank from X to Y\n");
	printw("2. list ranks by a specific name\n");
	printw("3. delete a specific rank\n");

	//3. wgetch()를 사용하여 변수 ch에 입력받은 메뉴번호 저장
	ch = wgetch(stdscr);
	//4. 각 메뉴에 따라 입력받을 값을 변수에 저장
	//4-1. 메뉴1: X, Y를 입력받고 적절한 input인지 확인 후(X<=Y), X와 Y사이의 rank 출력
	if (ch == '1') {
		//Node* walker = head;
		echo();

		printw("X: ");
		scanw("%d", &X);
	
		printw("Y: ");
		//Y = getch();	//cannot handle digit>1 input
		scanw("%d", &Y);
		if(Y > g_total) Y = g_total;

		noecho();
		//printw("\n%d %d\n", X, Y);
		printw("\n     name     \|    score  \n");
		printw("---------------------------\n");
		while(walker){
			if(X>Y || X>g_total) {
				printw("search failure: no rank in the list\n");
				break;	
			}
			if((count >= X) && (count <= Y)) 
				printw("%-13s \| %-10d\n", walker->name, walker->score);
			walker = walker->link;
			count++;
		}
		
	}

	//4-2. 메뉴2: 문자열을 받아 저장된 이름과 비교하고 이름에 해당하는 리스트를 출력
	else if ( ch == '2') {
		char str[NAMELEN+1];
		int check = 0;
		//Node* walker = head;
		
		echo();

		printw("Name to find: ");
		getstr(str);
		
		noecho();

		printw("\n     name     \|    score  \n");
		printw("---------------------------\n");
		
		while(walker){
			if(strcmp(walker->name, str) == SAME){
				check = 1;
				printw("%-13s \| %-10d\n", walker->name, walker->score);
			}
			walker = walker->link;
		}

		if(!check) printw("search failure: no infomation in the list\n");
	}

	//4-3. 메뉴3: rank번호를 입력받아 리스트에서 삭제
	else if ( ch == '3') {
		int num = 0;
		int count = 1;
		Node* prev = head;
	
		echo();
		
		printw("rank to delete: \n");	
		scanw("%d", &num);

		noecho();
		
		while(walker){

			if((num<=0) || (num > g_total)){		//if rank to delete > total rank number
				printw("search failure: no rank in the list\n");
				break;
			}			

			if(num == count){	//if the corresponding rank exists
				if(count == 1){		//if it is 1, need to move 'head' to the next node.
					walker = walker->link;
					head = walker;
					prev->link = NULL;
					printw("deleting... %dth rank: %s with the score of %d\n", num, prev->name, prev->score);
					free(prev);
					printw("result: rank deleted\n");
					g_total--;
					writeRankFile();
					break;
				}else{			//else, no need to move 'head'.
					prev->link = walker->link;
					walker->link = NULL;
					printw("deleting... %dth rank: %s with the score of %d\n", num, walker->name, prev->score);
					free(walker);
					printw("result: rank deleted\n");
					g_total--;
					writeRankFile();
					break;
				}

			}
			prev = walker;	
			walker = walker->link;	
			count++;	
		}
		
	}	

	getch();

}

void writeRankFile(){
	// 목적: 추가된 랭킹 정보가 있으면 새로운 정보를 "rank.txt"에 쓰고 없으면 종료
	//1. "rank.txt" 연다
	FILE *fp = fopen("rank.txt", "w");
	int i;
	Node* walker = head;
	//2. 랭킹 정보들의 수를 "rank.txt"에 기록
	fprintf(fp, "%d\n", g_total);
	//3. 탐색할 노드가 더 있는지 체크하고 있으면 다음 노드로 이동, 없으면 종료
	for(i=0; i<g_total; i++){
		fprintf(fp, "%s %d\n", walker->name, walker->score);
		walker = walker->link;
	}

	fclose(fp);
	//if ( sn == score_number) return;
	//else {


	//}
	//for ( i= 1; i < score_number+1 ; i++) {
	//	free(a.rank_name[i]);
	//}
	//free(a.rank_name);
	//free(a.rank_score);
}

void newRank(int score){
	// 목적: GameOver시 호출되어 사용자 이름을 입력받고 score와 함께 리스트의 적절한 위치에 저장
	char str[NAMELEN+1];
	Node* new = (Node*)malloc(sizeof(Node));
	Node* prev;
	Node* curr;
	
	
	clear();
	echo();
	//1. 사용자 이름을 입력받음
	printw("your name:");

	//scanw("%s", str);
	scanw("%[^\n]", str);	/* edited: for input with space */

	noecho();
	
	//2. 새로운 노드를 생성해 이름과 점수를 저장, score_number가
	new->score = score;
	strcpy(new->name, str);
	new->link = NULL;

	curr = prev = head;

	if(!prev) head = new;
	else if(prev->score < score){
		new->link = head;
		head = new;
	}else{
		curr = curr->link;
		while(curr){	//prev shoulc be pointing location just befoer insertion.
			if(curr->score < score) break;

			prev = curr;
			curr = curr->link;
		}
		new->link = prev->link;
		prev->link = new;
	}
	g_total++;

	writeRankFile();	
}

void DrawRecommend(int y, int x, int blockID,int blockRotate){
	// user code
	DrawBlock(y, x, blockID, blockRotate, 'R');
}

int modified_recommend(RecNode *root, char parent_field[HEIGHT][WIDTH]){
	int max=0; // 미리 보이는 블럭의 추천 배치까지 고려했을 때 얻을 수 있는 최대 점수
	int i, j, h, x, y, flag_index=0, c_index=0;
	int tmpScore=-1, min;
	int max_rotation=4;
	RecNode* new;
	
	space += sizeof(root);

	if(nextBlock[root->lv] == 0 || nextBlock[root->lv] >=5) max_rotation = 2;
	else if(nextBlock[root->lv] == 4) max_rotation = 1;

	for(i=0; i<max_rotation; i++){			//i: rotation
		for(j=-2; j<WIDTH; j++){			//j: x coordinate
			
			//create node
			new = (RecNode*)malloc(sizeof(RecNode));

			//sotre info - field
			for(h=0; h<HEIGHT; h++)
				for(x=0; x<WIDTH; x++){
					new->f[h][x] = parent_field[h][x];
				}
	
			//store info - lv, accumulated score
			new->lv = root->lv + 1;
			new->score = root->score;

			//determine y
			y=-1;
			while(CheckToMove(new->f, nextBlock[root->lv], i, y+1, j)) y++;	
			if(y==-1) continue;	//for segfault

			//score calculation
			new->score += AddBlockToField(new->f, nextBlock[root->lv], i, y, j);	//segfault?
			new->score += DeleteLine(new->f);					//delete line
			new->score += y * y;
				
			//store info = x, y, z
			new->tmpX = j;
			new->tmpY = y;
			new->tmpR = i;
			new->flag = 0;

			root->c[c_index++] = new;
			/* ------- info store done ------- */
		}
	}
	root->c[c_index] = NULL;
	
	/* all child nodes created */


	/* check throught children, set flag */
	for(i=0; i < min_num_to_find; i++){
		min = 99999;
		flag_index = -1;
		for(j=0; root->c[j]; j++){	//inner loop: set one min_flag per iteration
			if(min > root->c[j]->score){
				if(root->c[j]->flag) continue;	//if flagged in former outer loop, skip
				if(flag_index != -1) root->c[flag_index]->flag = 0;	//if find inner min >=2 times, unset former min flags.

				root->c[j]->flag = 1;		//set new flag
				flag_index = j;			//store flag index

				min = root->c[j]->score;		//update min
			}
		}
	}
	 /* ---- flagging done, 'min_num_to_find' flags set ---- */


	/* recursion: except flagged children */
	for(i=0; root->c[i]; i++){
		if(root->c[i]->flag){
			free(root->c[i]);
			continue;
		}

		if(root->c[i]->lv < VISIBLE_BLOCKS){
			tmpScore = modified_recommend(root->c[i], root->c[i]->f);
		}else tmpScore = root->c[i]->score;
		
		if(tmpScore > max){
			max = tmpScore;
			if(root->lv == 0){
				recommendX = root->c[i]->tmpX;
				recommendY = root->c[i]->tmpY;
				recommendR = root->c[i]->tmpR;
			}
		}
		
		free(root->c[i]);
	}


	return max;
}
	
void recommendedPlay(){
	int command;
	double T_duration;

	T_start = time(NULL);	

	clear();
	act.sa_handler = BlockDown;
	sigaction(SIGALRM, &act, &oact);
	InitTetris();
	do{
		if(timed_out == 0){
			alarm(1);
			timed_out = 1;
		}

		command = GetCommand();	//to input 'QUIT'

		if(ProcessCommand(command) == QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1, WIDTH/2-5, 1, 10);
			move(HEIGHT/2, WIDTH/2-4);
			printw("Good-Bye!!");
			refresh();
			getch();

			return;
		}

		/* else, recommended blocks */

		ProcessCommand(KEY_DOWN); 

		while(recommendR != blockRotate) {
			ProcessCommand(KEY_UP);
				
		}
		while(blockX > recommendX){
			ProcessCommand(KEY_LEFT);
			
		}
		while(blockX < recommendX){
			ProcessCommand(KEY_RIGHT);
		
		}
		while(blockY < recommendY) ProcessCommand(KEY_DOWN);	
		//while(CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)) ProcessCommand(KEY_DOWN);
	
	T_end = time(NULL);
	T_duration = (double)difftime(T_end, T_start);

	move(2, WIDTH+28);
	printw("*** evaluation ***");
		
	move(5, WIDTH+24);
	printw("time		: %8.2f s", T_duration);

	move(8, WIDTH+24);
	printw("duration	: %8.2lf s", DS_duration);
	
	move(9, WIDTH+24);
	printw("t_efficiency	: %8.2lf /s", score/DS_duration);

	move(12, WIDTH+24);
	printw("space used	: %8.2f kb", (double)space / 1024);
	
	move(13, WIDTH+24);
	printw("s_efficiency	: %8.2f kb/s", score / ((double)space / 1024));


	}while(!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1, WIDTH/2-5, 1, 10);
	move(HEIGHT/2, WIDTH/2-4);
	printw("GameOver!!");
	refresh();

	getch();	//game ended, pause until any input.

	newRank(score);
	// user code
}


/*
int evalSize(RecNode* root){
	int i=0, tmpscore=0, evalsize=0;

	while(root->c[i]){
		if(root->lv < VISIBLE_BLOCKS) evalsize += evalSize(root->c[i]);
		else evalsize += sizeof(RecNode);
		i++;
	}

	return evalsize;
}
*/






