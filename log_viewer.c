#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
	char buf[365];
	int fd, n;
	fd = open("pentagolog.txt", O_RDONLY);
	if (fd == -1) {
		perror("Open pentagolog.txt");
		exit(1);
	}

	while ((n = read(fd, buf, sizeof(buf))) > 0) {
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
	
	
