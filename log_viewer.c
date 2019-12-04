#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
// 프로그램 인자로 주어진 로그 파일명을
// 출력해주는 프로그램
int main(int argc, char* argv[]) {
	char buf[365];
	int fd, n;
	int cnt = 0;
	fd = open(argv[1], O_RDONLY);
	if (fd == -1) {
		perror(argv[1]);
		exit(1);
	}

	while ((n = read(fd, buf, sizeof(buf))) > 0) {
		printf("%d\n", ++cnt);
		int i, j;
		for(i = 0; i < 14; i++) {
			for(j = 0; j < 26; j++) {
				printf("%c", buf[26*i+j]);
			}
		printf("\n");
		}
	printf("\n\n");
	}

	close(fd);
	return 0;
}
	
	
