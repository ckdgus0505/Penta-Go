#include <arpa/inet.h>
#include <netdb.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>

#define PORTNUM 5050

void handler(int signo) {
	printf("lose\n");
	exit(1);
}

char arr[6][6];	// 펜타고 보드 배열

int is_end;					// 게임이 끝난것을 확인하는 변수,
										// 0이면 게임이 끝나지 않은 상태,
										// 1이면 흑돌 win, 2이면, 백돌 win 
char quadrant;			// 회전시킬 사분면을 저장하는 변수
char x, y, c;				// x,y는 보드의 좌표축, c는 회전시킬 사분면의 회전 방향,
										// c 는 clock wise 의 줄임말로, y or Y 가 입력되면 시계방향 회전

void init_board();	// 보드를 깨끗한 상태로 초기화 하는 함수
void print_board(); // 현재 보드의 상태를 출력해주는 함수
void send_board(int ns);
void fix_board(int ns); // 현재 보드의 원하는 위치에 돌을 놓는 함수
void rotate_board(int ns); // 현재 보드에 원하는 사분면에 원하는 방향으로 회전시키는 함수
int check_pentago(); // 게임이 끝났는지 확인하는 함수
int is_finish(int ns);

int main(void) {
	struct sockaddr_in sin, cli;
	int sd, ns, clientlen = sizeof(cli);

	if((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

	memset((char*)&sin, '\0', sizeof(int));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(PORTNUM);
	sin.sin_addr.s_addr = inet_addr("127.0.0.1");

	system("clear");
	printf("도전자를 기다리는중...\n");
	if(bind(sd, (struct sockaddr *)&sin, sizeof(sin))) {
		perror("bind");
		exit(1);
	}

	if(listen(sd, 5)) {
		perror("listen");
		exit(1);
	}

	if ((ns = accept(sd, (struct sockaddr *)&cli, &clientlen)) == -1) {
		perror("accept");
		exit(1);
	}

		system("clear");
		printf("게임이 1초내로 시작됩니다.\n");
		sleep(1);
		signal(SIGALRM, handler);
		int count = 0;
		init_board(); // 게임을 시작하기 전 보드의 상태를 초기화 한다.


	while(is_end == 0) {
		system("clear");
		print_board();
		char type[2];
		if (recv(ns, type, sizeof(type), 0) == -1) { // 필요 함수를 받는다.
			perror("recv");
			exit(1);
		}
		printf("fnum get\n");
		if( strcmp(type, "1") == 0) { // send_board를 실행
			printf("do send_board()\n");
			send_board(ns);
		} else if(strcmp(type, "2") == 0) { // fix_board를 실행
			printf("do fix_board()\n");
			fix_board(ns);
		} else if (strcmp(type, "3") == 0) { // rotate_board를 실행
			printf("do rotate_board()\n");
			rotate_board(ns); // 현재 보드에 원하는 사분면에 원하는 방향으로 회전시키는 함수
		} else if (strcmp(type, "4") == 0) { // rotate_board를 실행
			printf("is_finish()\n");
			is_end = is_finish(ns); // 현재 보드에 원하는 사분면에 원하는 방향으로 회전시키는 함수
		}
	}
	close(ns);
	close(sd);

	return 0;
}

// 보드(판)을 ' '로 초기화 해주는 함수
void init_board() {
	for (int i = 0; i < 6; i++ )
		for(int j = 0; j < 6; j++ )
				arr[i][j] = ' ';
}

// 보드의 현재 상태를 출력해주는 함수
// 가로, 세로축에 A~F, 1~6 을 추가로 출력해준다.


void print_board() {
	printf(" │ A │ B │ C │ D │ E │ F │\n");
	printf("─┼───┼───┼───┼───┼───┼───┼\n");
	int i=0, j=0;
	for(int l = 0; l < 6; l++) {
		printf("%d│", l+1);
		for (int m = 0; m < 6; m++) {
			printf(" %c │", arr[l][m]);
		}
		printf("\n─┼───┼───┼───┼───┼───┼───┼\n");
	}
}

void send_board(int ns) {
	char buf[365];
	memset(buf, 0, sizeof(buf));
	int i, j;
	print_board();
	for(i = 0; i < 14; i++) {
		for(j = 0; j < 26; j++) {
			if ( i == 0 ) {
				if ( j % 2 == 0) buf[26*i+j] = ' '; 
				else if ( j % 4 == 1) buf[26*i+j] = '|';
				else if ( j % 4 == 3) buf[26*i+j] = j/4 + 'A';
			}
			else if (i % 2 == 1) {
				if ( j % 4 == 1) buf[26*i+j] = '+';
				else buf[26*i+j] = '-';
			}
			else {
				if (j==0) buf[26*i+j] = i/2+'0';
				else if ( j % 4 == 1) buf[26*i+j] = '|';
				else if ( j % 2 == 0) buf[26*i+j] = ' ';
				else buf[26*i+j] = arr[i/2-1][j/4]; 
			}
		}
	}
	buf[364] = '\0';
	if(send(ns, buf, strlen(buf) + 1, 0) == -1) {
		perror("send");
		exit(1);
	}
}

// 보드에 돌을 놓는 함수
// row, col 에다가 dol을 놓는다.
void fix_board(int ns) {
	char rowcol[4];
	int row, col;
	if(send(ns, "OK", strlen("OK") + 1, 0) == -1) { // 의미 없음
		perror("send");
		exit(1);
	}
	printf("좌표 send\n");
	
	if (recv(ns, rowcol, sizeof(rowcol), 0) == -1) {
		perror("recv");
		exit(1);
	}

	printf("좌표 receive  : %c %c %s\n", rowcol[0], rowcol[1], rowcol);
	sleep(1);

	row = rowcol[1] - '0' - 1;
	col = rowcol[0] - 'A';

	printf("%d %d\n", row, col);
	if (arr[row][col] == ' ') {
		arr[row][col] = rowcol[2];
		if(send(ns, "0", strlen("0") + 1, 0) == -1) {
			perror("send");
			exit(1);
		}
	} else {
		if(send(ns, "-1", strlen("-1") + 1, 0) == -1) {
			perror("send");
			exit(1);
		}
	}
	printf("end..\n");
//	print_board();
}

// 보드의 한 사분면을 회전하는 함수, is_clock_wise 가 y이거나 Y이면 시계방향 회전이다.
//  화면 출력 기준으로 

//   1 2 
//   3 4

//	사분면이다.
void rotate_board(int ns) {
	int row, col;
	char qc[3];
	if(send(ns, "1", strlen("1") + 1, 0) == -1) { // 의미 없음
		perror("send");
		exit(1);
	}

	if (recv(ns, qc, sizeof(qc), 0) == -1) {
		perror("recv");
		exit(1);
	}
	printf("qc : %s\n", qc);
	if (qc[0] == '1') {
		row = 0;
		col = 0;
	} else if (qc[0] == '2') {
		row = 0;
		col = 3;
	} else if (qc[0] == '3') {
		row = 3;
		col = 0;
	} else if (qc[0] == '4') {
		row = 3;
		col = 3;
	}	
	for (int i = 0; i < qc[1]-'0'; i++) {
		char tmp = arr[0+row][0+col];
		arr[0+row][0+col] = arr[2+row][0+col];
		arr[2+row][0+col] = arr[2+row][2+col];
		arr[2+row][2+col] = arr[0+row][2+col];
		arr[0+row][2+col] = tmp;
		tmp = arr[0+row][1+col];
		arr[0+row][1+col] = arr[1+row][0+col];
		arr[1+row][0+col] = arr[2+row][1+col];
		arr[2+row][1+col] = arr[1+row][2+col];
		arr[1+row][2+col] = tmp;
	}

	if(send(ns, "1", strlen("1") + 1, 0) == -1) { // 의미 없음
		perror("send");
		exit(1);
	}

}


//게임이 끝났는지 확인하는 함수
//5개의 돌이 이어졌는지 체크한다.
int is_finish(int ns) {
	char str[2];
	int ret = check_pentago();
	str[0] = ret + '0';
	str[1] = '\0';
	if(send(ns, str, strlen(str) + 1, 0) == -1) {
		perror("send");
	}
	printf(" ret : %d\n", ret);
	if( ret == 1 ) {
		return 1; // 게임 끝
	}
	else return 0; // 게임 계속 진
}


int check_pentago() {
	int count = 0;
	int notNull = 0;
	//가로
	for(int i = 0 ; i < 6 ; i++) {
		for(int j = 1 ; j < 5 ; j++) {
			if(arr[i][j] == ' ') {
				notNull = 1;
				break;
			}
		}
		if((notNull != 1 && arr[i][0] != ' ') || (notNull != 1 && arr[i][5] != ' ')) {
			for(int j = 0 ; j < 6 ; j += 5) {
				for(int k = 1 ; k < 5 ; k++) {
					if(arr[i][j] == arr[i][k]) {
						count++;
					}
				}
				if(count == 4) {
					return 1;
				}
				else {
					count = 0 ;
				}
			}
		}
		notNull = 0;
	}
	return 0;
} /*
	//세로

	count = 0;
	for(int i = 0 ; i < 6 ; i++) {
		for(int j = 1 ; j < 5 ; j++) {
			if(arr[j][i] == ' ') {
				break;
			}
		}
		if(arr[0][i] != ' ' || arr[5][i] != ' ') {
			for(int j = 0 ; j < 6 ; j += 5 ) {
				for(int k = 1 ; k < 5 ; k++) {
					if(arr[j][i] == arr[k][i]) {
						count++;
					}
				}
				if(count == 4) {
					return 1;
				}
				else {
					count = 0;
				}
			}
		}
	}
	//왼쪽 위 오른쪽 아래 1
	count = 0;
	{
		for(int i = 1 ; i < 5 ; i++) {
			if(arr[i][i] == " ") {
				break;
			}
		}
		if(arr[0][0] != ' ' || arr[5][5] != ' ') {
			for(int i = 0 ; i < 6 ; i += 5) {
				for(int j = 1 ; j < 5 ; j++) {
					if(arr[i][i] == arr[j][j]) {
						count++;
					}
				}
				if(count == 4) {
					return 1;
				}
				else {
					count = 0 ;
				}
			}
		}
	}
	//왼쪽 아래 오른쪽 위 2
	count = 0;
	{
		for(int i = 1, j = 4 ; i < 5 ; i++, j--) {
			if(arr[i][j] == " ") {
				break;
			}
		}
		if(arr[0][5] != ' ' || arr[5][0] != ' ') {
			for(int i = 0 , j = 5 ; i < 6 ; i += 5, j -= 5) {
				for(int k = 1, l = 4 ; k < 5 ; k++, l--) {
					if(arr[i][j] == arr[k][l]) {
						count++;
					}
				}
				if(count == 4) {
					return 1;
				}
				else {
					count = 0;
				}
			}
		}
	}
	//왼쪽 위 오른쪽 아래 4개 위 3
	count = 0;
	if(arr[0][1] != ' ') {
		for(int i = 1 ; i < 5 ; i++) {
			if(arr[0][1] == arr[i][i+1]) {
				count++;
			}
		}
		if(count == 4) {
			return 1;
		}
		else {
			count = 0 ;
		}
	}
	//왼쪽 위 오른쪽 아래 4개 아래 4
	count = 0;
	if(arr[1][0] != ' ') {
		for(int i = 1 ; i < 5 ; i++) {
			if(arr[1][0] == arr[i+1][i]) {
				count++;
			}
		}
		if(count == 4) {
			return 1;
		}
		else {
			count = 0;
		}
	}
	//왼쪽 아래 오른쪽 위 4개 위 5
	count = 0;
	if(arr[0][4] != ' ') {
		for(int i = 1, j = 3; i < 5 ; i++, j--) {
			if(arr[0][4] == arr[i][j]) {
				count++;
			}
		}
		if(count == 4) {
			return 1;
		}
		else {
			count = 0;
		}
	}
	//왼쪽 아래 오른쪽 위 4개 아래 6
	count = 0;
	if(arr[1][5] != ' ') {
		for(int i = 2, j = 4 ; i < 6 ; i++, j--) {
			if(arr[1][5] == arr[i][j]) {
				count++;
			}
		}
		if(count == 4) {
			return 1;
		}
		else {
			count = 0;
		}
	}
	return 0;
}*/
