#include <arpa/inet.h>
#include <netdb.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define PORTNUM 5050

char arr[4][3][3];	// 펜타고 보드 배열

int is_end;					// 게임이 끝난것을 확인하는 변수,
										// 0이면 게임이 끝나지 않은 상태,
										// 1이면 흑돌 win, 2이면, 백돌 win 
char quadrant;			// 회전시킬 사분면을 저장하는 변수
char x, y, c;				// x,y는 보드의 좌표축, c는 회전시킬 사분면의 회전 방향,
										// c 는 clock wise 의 줄임말로, y or Y 가 입력되면 시계방향 회전

void init_board();	// 보드를 깨끗한 상태로 초기화 하는 함수
void print_board(); // 현재 보드의 상태를 출력해주는 함수
void send_board(int ns);
int fix_board(int row, int col, char dol); // 현재 보드의 원하는 위치에 돌을 놓는 함수
void rotate_board(int quad, char is_clock_wise); // 현재 보드에 원하는 사분면에 원하는 방향으로 회전시키는 함수
int check_pentago(); // 게임이 끝났는지 확인하는 함수

int main(void) {
	struct sockaddr_in sin, cli;
	int sd, ns, clientlen = sizeof(cli);
	char buf[1024];

	if((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

	memset((char*)&sin, '\0', sizeof(int));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(PORTNUM);
	sin.sin_addr.s_addr = inet_addr("127.0.0.1");

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
		printf("플레이어 차례입니다.\n");
		int count = 0;
		init_board(); // 게임을 시작하기 전 보드의 상태를 초기화 한다.
		sleep(1);

	while(is_end == 0) {
		system("clear");
		print_board();
		send_board(ns);

		sprintf(buf, "좌표 (ex, A1) : ");
		if(send(ns, buf, strlen(buf) + 1, 0) == -1) {
			perror("send");
			exit(1);
		}


		if (recv(ns, buf, sizeof(buf), 0) == -1) {
			perror("recv");
			exit(1);
		}
		fix_board(buf[0], buf[1], 'O');

		print_board();
		send_board(ns);

	}

	close(ns);
	close(sd);

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
	printf(" │ A │ B │ C │ D │ E │ F │\n");
	printf("─┼───┼───┼───┼───┼───┼───┼\n");
	int i=0, j=0, k=0;
	for(int l = 0; l < 6; l++) {
		printf("%d│", l+1);
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
			printf(" %c │", arr[i][j][k]);
			
		}
		printf("\n─┼───┼───┼───┼───┼───┼───┼\n");
	}
}

void send_board(int ns) {
	char buf[365];
	memset(buf, 0, sizeof(buf));
	int cnt = 0;
	for(int i = 0; i < 14; i++) {
		for(int j = 0; j < 26; j++) {
			if ( i == 0 ) {
				if ( j % 2 == 0) buf[cnt++] = ' '; 
				if ( j % 4 == 1) buf[cnt++] = '|';
				if ( j % 4 == 3) buf[cnt++] = j/4 + 'A';
			}
			else if (i % 2 == 1) {
				if ( j % 4 == 1) buf[cnt++] = '+';
				else buf[cnt++] = '-';
			}
			else {
				if ( j % 4 == 1) buf[cnt++] = '|';
				else buf[cnt++] = ' '; 
			}
		}
	}
	buf[cnt] = '\0';
	for (int l = 0; l < 4; l++) {
		for(int m = 0; m < 3; m++) {
			for(int n = 0; n < 3; n++) {
				int tmp;
				if (l == 0) tmp = 6+m+n;//buf[6*m+n] = arr[l][m][n];
				else if (l == 1) tmp = 6*m+n+3; //buf[6*m+n + 3] = arr[l][m][n]; 
				else if (l == 2) tmp = 6*m+n+18;//buf[6*m+n + 18] = arr[l][m][n]; 
				else if (l == 3) tmp = 6*m+n+21;//buf[6*m+n + 21] = arr[l][m][n];
				int i = tmp %6;
				int j = tmp / 6;
				buf[(4*i+3)*26+2*j+2] = arr[l][m][n];

			}
		}
	}
	printf("%s", buf);
	if(send(ns, buf, strlen(buf) + 1, 0) == -1) {
		perror("send");
		exit(1);
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


//게임이 끝났는지 확인하는 함수
//5개의 돌이 이어졌는지 체크한다.

 int check_pentago();int check_pentago() {
	int count = 0;

	//가로
	for(int i = 0 ; i < 4 ; i += 2) {
		for(int j = 0 ; j < 3 ; j++) {
			if((arr[i][j][1] == 'O' && arr[i][j][2] == 'O') ||
					(arr[i][j][1] == 'X' && arr[i][j][2] == 'X')) {
				if(arr[i][j][0] == arr[i][j][1]) {
					if(arr[i][j][0] == arr[i][j][2]) {
						for(int k = 0 ; k < 2 ; k++){
							if(arr[i][j][0] == arr[i+1][j][k]){
								count++;
							}
						}
					}
				}
				if(count == 2) {
					return 1;
				}
				else {
					count = 0;
				}
				if(arr[i][j][1] == arr[i][j][2]) {
					for(int k = 0 ; k < 3 ; k++) {
						if(arr[i][j][1] == arr[i+1][j][k]) {
							count++;
						}
					}
				}
				if(count == 3){
					return 1;
				} 
				else {
					count = 0;
				}
			}	
			else {
				continue;
			}
		}
	}
	count = 0;	
	//세로
	for(int i = 0 ; i < 2 ; i++) {
		for(int j = 0 ; j < 3 ; j++) {
			if((arr[i][1][j] == 'O' && arr[i][2][j] == 'O') ||
					(arr[i][1][j] == 'X' && arr[i][2][j] == 'X')) {
				if(arr[i][1][j] == arr[i][0][j]) {
					if(arr[i][1][j] == arr[i][2][j]) {
						for(int k = 0 ; k < 2 ; k++) {
							if(arr[i][0][j] == arr[i+2][k][j]) {
								count++;
							}
						}
					}
				}
				if(count == 2) {
					if(arr[i][1][j] == 'O') {
						return 1;
					}
					else {
						return 2;
					}
				}
				else {
					count = 0;
				}
				if(arr[i][1][j] == arr[i][2][j]) {
					for(int k = 0 ; k < 3 ; k++) {
						if(arr[i][1][j] == arr[i+2][k][j]) {
							count++;
						}
					}
				}
				if(count == 3) {
					if(arr[i][1][j] == 'O') {
						return 1;
					}
					else {
						return 2;
					}
				}
				else {
					count = 0;
				}
			}
		}
	}
	count = 0;
	//왼쪽 위에서 오른쪽 아래 대각선
	if((arr[0][1][1] == 'O' && arr[0][2][2] == 'O') ||
			(arr[0][1][1] == 'X' && arr[0][2][2] == 'X')) {
		if(arr[0][1][1] == arr[0][2][2]) {
			if(arr[0][1][1] == arr[0][0][0]) {
				for(int i = 0 ; i < 2 ; i++) {
					if(arr[0][1][1] == arr[3][i][i]) {
						count++;
					}
				}
			}
			if(count == 2) {
			  if(arr[0][1][1] == 'O') {
    			return 1;
  			}
  			else {
    			return 2;
			  }
			}
			else {
				count = 0;
			}
			for(int i = 0 ; i < 3 ; i++) {
				if(arr[0][1][1] == arr[3][i][i]) {
					count++;
				}
			}
			if(count == 3) {
			  if(arr[0][1][1] == 'O') {
			    return 1;
			  }
			  else {
			    return 2;
			  }
			}
			else {
				count = 0;
			}
		}
	}
	count = 0;
	//오른쪽 위에서 왼쪽 아래 대각선
	if((arr[1][1][1] == 'O' && arr[1][2][0] == 'O') ||
			arr[1][1][1] == 'X' && arr[1][2][0] == 'X') {
		if(arr[1][1][1] == arr [1][2][0]) {
			if(arr[1][1][1] == arr[1][0][2]) {
				int temp = 2;
				for(int i = 0 ; i < 2 ; i++) {
					if(arr[1][1][1]==arr[2][i][temp--]) {
						count++;
					}
				}
				if(count == 2) {
					if(arr[1][1][1] == 'O') {
				    return 1;
				  }
				  else {
				    return 2;
				  }
				}
				else {
					count = 0;
				}
			}
			int temp = 2;
			for(int i = 0 ; i < 3 ; i++) {
				if(arr[1][1][1] == arr[2][i][temp--]) {
					count++;
				}
			}
			if(count == 3) {
			  if(arr[1][1][1] == 'O') {
			    return 1;
			  }
			  else {
			    return 2;
			  }
			}
			else {
				count = 0;
			}
		}
	}
	count = 0;
	if(arr[1][2][0] == 'O' || arr[1][2][0] == 'X') {
		for(int i = 0 ; i < 4 ; i += 3) {
			for(int j = 0 ; j < 2 ; j++) {
				for(int k = 1 ; k < 3 ; k++) {
					if(arr[1][2][0] == arr[i][j][k]) {
						count++;
					}
				}
			}
		}
    if(count == 4) {
      if(arr[1][2][0] == 'O') {
        return 1;
      }
      else {
        return 2;
      }
    }
    else {
      count = 0;
		}
	}
	count = 0;
	if(arr[2][0][2] == 'O' || arr[2][0][2] == 'X'){
		for(int i = 0 ; i < 4 ; i += 3) {
			for(int j = 1 ; j < 3 ; j++) {
				for(int k = 0 ; k < 2 ; k++) {
					if(arr[2][0][2] == arr[i][j][k]) {
						count++;
					}
				}
			}
		}
    if(count == 4) {
      if(arr[2][0][2] == 'O') {
        return 1;
      }
      else {
	      return 2;
      }
    }
    else {
      count = 0;
		}
	}
	count = 0;
	if(arr[0][2][2] == 'O' || arr[0][2][2] == 'X') {
	  for(int i = 1 ; i < 3 ; i ++) {
  	  for(int j = 0 ; j < 2 ; j++) {
   		  for(int k = 1 ; k >= 0 ; k--) {
					if(arr[0][2][2] == arr[i][j][k]) {
   	        count++;
   	     	}
   	   	}
	   	}
	  }
    if(count == 4) {
      if(arr[0][2][2] == 'O') {
        return 1;
      }
      else {
        return 2;
 	   }
    }
    else {
      count = 0;
    }
	}
	count = 0;
	if(arr[3][0][0] == 'O' || arr[3][0][0] == 'X'){
 	 for(int i = 1 ; i < 3 ; i ++) {
	    for(int j = 1 ; j < 3 ; j++) {
	      for(int k = 2 ; k >= 0 ; k--) {
	        if(arr[3][0][0] == arr[i][j][k]) {
	          count++;
	        }
	      }
	    }
	  }
    if(count == 4) {
      if(arr[3][0][0] == 'O') {
        return 1;
      }
      else {
        return 2;
      }
    }
    else {
      count = 0;
    }
	}

	return 0;
}

