#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>

FILE *file;

int closeFile() {
	for (int i=0; i<3; i++) {
		if (!fclose(file)) {
			return 0;
		}
	}
	printf("\nОшибка закрытия файла\n");
	exit(1);
}

void sigintHandler(int sig_num) {
        closeFile(); 
        exit(0);
}


int main(int argc, char *argv[]) {
	signal(SIGINT, sigintHandler);

    	if(argv[1]==NULL) {
        	printf("Имя файла не указано\n");
    		return 1;
	}

        file = fopen(argv[1],"w");
	if (file == NULL) {
		printf("Error: %d %s %s\n", errno, argv[1], strerror(errno));
		return 1;
	}

	signal(SIGINT, sigintHandler);

	while (1) {
		if (ferror(file)) {
			fclose(file);
			printf("Ошибка записи");
			return 1;
		}
		char str = fgetc(stdin);
		if (str == *"!end!") {
			break;
		}
		fputc(str, file);
        }

	closeFile();

	return 0;
}

