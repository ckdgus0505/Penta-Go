#include <stdio.h>
#include <unistd.h>
char arr[4][3][3];	// 펜타고 보드 배열
int is_end;					// 게임이 끝난것을 확인하는 변수,
										// 0이면 게임이 끝나지 않은 상태,
										// 1이면 흑돌 win, 2이면, 백돌 win 
char quadrant;			// 회전시킬 사분면을 저장하는 변수
char x, y, c;				// x,y는 보드의 좌표축, c는 회전시킬 사분면의 회전 방향,
										// c 는 clock wise 의 줄임말로, y or Y 가 입력되면 시계방향 회전
void init_board();	// 보드를 깨끗한 상태로 초기화 하는 함수
void print_board(); // 현재 보드의 상태를 출력해주는 함수
int fix_board(int row, int col, char dol); // 현재 보드의 원하는 위치에 돌을 놓는 함수
void rotate_board(int quad, char is_clock_wise); // 현재 보드에 원하는 사분면에 원하는 방향으로 회전시키는 함수
int check_pentago(); // 게임이 끝났는지 확인하는 함수


int main(void) {
	system("clear"); // unix 시스템에서 cli 화면 clear을 하기 위함.
	printf("첫 돌은 흑돌입니다.\n");
	int count = 0;	// 돌 놓은 횟수를 저장, 현재 플레이어를 확인하기 위함
	init_board(); 	// 게임을 시작하기 전 보드를 초기화 한다.
	sleep(1);				// 플레이어에게 '첫 돌은 흑돌입니다' 라는 문구를 보여주기 위함

	while (is_end == 0) {	// is_end 변수는 게임이 진행중일때는 0이다. 


		system("clear");

		print_board();	// 현재 보드의 상태를 출력해준다.

		printf("좌표 (ex, A1) : ");
		scanf(" %c %c", &x, &y); // 플레이어가 원하는 좌표를 입력받는다.
		if (count % 2 == 0) {
			while (fix_board(x-'A', y-'0' - 1, 'O')!=0) {
				printf("잘못두셨습니다. 다시 두세요 :");
				scanf(" %c %c", &x, &y); // 플레이어가 원하는 좌표를 입력받는다.
			}
		} else {
			while (fix_board(x-'A', y-'0' - 1, 'X')!=0) {
				printf("잘못두셨습니다. 다시 두세요 :");
				scanf(" %c %c", &x, &y); // 플레이어가 원하는 좌표를 입력받는다.
			}
		}
		system("clear");
		print_board();		// 돌 놓은곳을 반영하여 출력해준다.


		printf("회전할 사분면\n");
		scanf(" %c", &quadrant);
		printf("시계방향?\n");
		scanf(" %c", &c);

		rotate_board(quadrant - '0' - 1, c); // 판을 회전한다., c가 y나 Y이면 시계방향 , 아니면 반시계방향
		system("clear");
		print_board();		// 회전을 반영하여  반영하여 출력해준다.
		printf("\n 플레이어 체인지!\n");
		sleep(1);

		// is_end = check_pentago();
		count ++;
	}
	return 0;
}
// 보드(판)을 ' '로 초기화 해주는 함수
void init_board() {
	for (int i = 0; i < 4; i++ )
		for(int j = 0; j < 3; j++ )
			for(int k = 0; k < 3; k++ )
				arr[i][j][k] = ' ';
}

// 보드의 현재 상태를 출력해주는 함수
// 가로, 세로축에 A~F, 1~6 을 추가로 출력해준다.


void print_board() {
	printf("  A B C D E F \n");
	int i=0, j=0, k=0;
	for(int l = 0; l < 6; l++) {
		printf("%d ", l+1);
		for (int m = 0; m < 6; m++) {
			if (l < 3) {
				if(m < 3) i = 0;
				else i = 1;
			}
			else {
				if(m < 3) i = 2;
				else i = 3;
			}
			
			j = l % 3;
			k = m % 3;
			printf("%c ", arr[i][j][k]);
			
		}
		printf("\n");
	}
}

// 보드에 돌을 놓는 함수
// row, col 에다가 dol을 놓는다.
int fix_board(int row, int col, char dol) {
	int i, x, y;
	if ( col < 3 ) {
		if( row < 3) i = 0;
		else i = 1;
	}
	else {
		if ( row < 3) i = 2;
		else i = 3;
	}
	x = col % 3;
	y = row % 3;
	if (arr[i][x][y] == ' ') {
		arr[i][x][y] = dol;
		return 0;
	}
	else return -1;
}

// 보드의 한 사분면을 회전하는 함수, is_clock_wise 가 y이거나 Y이면 시계방향 회전이다.
//  화면 출력 기준으로 

//   1 2 
//   3 4

//	사분면이다.
void rotate_board(int quad, char is_clock_wise) {
	int rotate_n = 3;
	if ( (is_clock_wise == 'y') || (is_clock_wise == 'Y')) rotate_n = 1;

	for(int i = 0; i < rotate_n; i++) {
		int temp = arr[quad][0][0];
		arr[quad][0][0] = arr[quad][2][0];
		arr[quad][2][0] = arr[quad][2][2];
		arr[quad][2][2] = arr[quad][0][2];
		arr[quad][0][2] = temp;
		temp = arr[quad][0][1];
		arr[quad][0][1] = arr[quad][1][0];
		arr[quad][1][0] = arr[quad][2][1];
		arr[quad][2][1] = arr[quad][1][2];
		arr[quad][1][2] = temp;
	
	}
}
// int check_pentago();
