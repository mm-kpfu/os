#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *file;

int closeFile() {
	for (int i=0; i<3; i++) {
		if (!fclose(file)) {
			exit(0);
		}
	}
	printf("\nОшибка закрытия файла\n");
	exit(1);
}


struct result {
	char part[32768];
	int is_end_of_file;
};


struct result readLines(int count) {
	int lines_readed = 0;
	struct result r;
	strcpy(r.part, "");


	if (count == 0) {
		count = -1;
	}

	while (count != lines_readed) {
		while (1) {
			char sym[1];
			sym[0] = (char) fgetc(file);

                        if (feof(file)) {
                                r.is_end_of_file = 1;
                                return r;
                        }

			strcat(r.part, sym);

			if (*"\n" == sym[0]) {
				break;
			}

			if (ferror(file)) {
				printf("Ошибка чтения");
				closeFile(file);
			}
		}
		lines_readed++;
	}

	return r;
}


int main(int argc, char *argv[]) {
	file = fopen(argv[1], "r");

	if (ferror(file)) {
		printf("Ошибка открытия файла");
		return 1; 
	}
	while (1) {
		struct result res = readLines(atoi(argv[2]));
		printf("%s", res.part);
		if (res.is_end_of_file) {
			printf("\nEnd of file!\n");
			break;
		}

		getc(stdin);
	}
	closeFile();
	return 0;
}
