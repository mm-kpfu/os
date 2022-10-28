#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

FILE *from_file;
FILE *to_file;

int closeFiles(int code) {
	for (int i=0; i<3; i++) {
		if (!fclose(from_file) && !fclose(to_file)) {
			return code;
		}
	}
	printf("\nОшибка закрытия файла\n");
	return 1;
}

int main(int argc, char *argv[]) {
	from_file = fopen(argv[1], "r");
	to_file = fopen(argv[2], "w");
	if (ferror(from_file) || ferror(to_file)) {
		printf("Ошибка открытия файла");
		return closeFiles(1);
	}

	while (1) {
		char sym = fgetc(from_file);
                if (feof(from_file)) {
                        break;
		}
                fputc(sym, to_file);

		if (ferror(from_file) || ferror(to_file)) {
			printf("Ошибка записи");
			return closeFiles(1);
		}
	}

	struct stat st;
	stat(argv[1], &st);
	chmod(argv[2], st.st_mode);

	return closeFiles(0);
}

