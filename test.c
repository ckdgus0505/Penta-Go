#include <stdio.h>
#include <unistd.h>
char arr[4][3][3];
int is_end;
char quadrant;
char x, y, c;

void print_board();
void fix_board(int row, int col, char dol);
void rotate_board(int quad, char is_clock_wise);
int check_pentago();
int main(void) {
	system("clear");
	printf("첫 돌은 흑돌입니다.\n");
	int count = 0;
	for (int i = 0; i < 4; i++ )
		for(int j = 0; j < 3; j++ )
			for(int k = 0; k < 3; k++ )
				arr[i][j][k] = ' ';
	sleep(1);

	while (is_end == 0) {
		system("clear");

		print_board();
		printf("좌표 (ex, A1) : ");
		scanf(" %c %c", &x, &y);
		if (count % 2 == 0) 
			fix_board(x-'A', y-'0' - 1, 'O');
		else
			fix_board(x-'A', y-'0' - 1, 'X');
		system("clear");
		print_board();
		printf("회전할 사분면\n");
		scanf(" %c", &quadrant);
		printf("시계방향?\n");
		scanf(" %c", &c);

		rotate_board(quadrant - '0' - 1, c);
		// is_end = check_pentago();
		count ++;
	}
	return 0;
}

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

void fix_board(int row, int col, char dol) {
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
	arr[i][x][y] = dol;
}

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
