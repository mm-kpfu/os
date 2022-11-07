#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>


typedef struct {
	char location[500];
	char copy_to[500];
	unsigned int mode;
	int size;
} FileInfo;

int size_compare(const void *a, const void *b) {
	FileInfo *file1 = (FileInfo *)a;
	FileInfo *file2 = (FileInfo *)b;
	return (file1 -> size - file2 -> size);
}

int name_compare(const void *a, const void *b) {
	FileInfo *file1 = (FileInfo *)a;
        FileInfo *file2 = (FileInfo *)b;
	return strcmp(file1 -> location, file2 -> location);
}

int getFileInfos(FileInfo *paths, char *dir_path, char *copy_dir_path, int last_index) {
        DIR *dir;
        struct dirent *dir_struct;
        dir = opendir(dir_path);

        while ((dir_struct = readdir(dir)) != NULL) {
                char location[500];
		char copy_to[500];
                sprintf(location, "%s/%s", dir_path, dir_struct -> d_name);
		sprintf(copy_to, "%s/%s", copy_dir_path, dir_struct -> d_name);
		struct stat st;
		stat(location, &st);
                if (!strcmp(".", dir_struct -> d_name) || !strcmp("..", dir_struct -> d_name)) {
                        continue;
                } else if (dir_struct -> d_type == DT_DIR && strcmp(copy_dir_path, location)) {
			mkdir(copy_to, st.st_mode);
                        last_index = getFileInfos(paths, location, copy_to, last_index);
                } else {
			sprintf(paths[last_index].location, "%s", location);
			sprintf(paths[last_index].copy_to, "%s", copy_to);	
			paths[last_index].size = st.st_size;
			paths[last_index].mode = st.st_mode;
                        last_index++;                   
                }
        }

	if (closedir(dir)) {
		printf("Ошибка закрытия директории %s", dir_path);
	}
	return last_index;
}

int count_files(char *dir_path, char *copy_dir_path) {
	DIR *dir;
	struct dirent *dir_struct;
	dir = opendir(dir_path);
	int count = 0;

	while ((dir_struct = readdir(dir)) != NULL) {
		char location[500];
		sprintf(location, "%s/%s", dir_path, dir_struct -> d_name);
		if (!strcmp(".", dir_struct -> d_name) || !strcmp("..", dir_struct -> d_name)) {
			continue;
		} else if (dir_struct -> d_type == DT_DIR && strcmp(copy_dir_path, location)) {
			count += count_files(location, copy_dir_path); 
		} else {
			count++;
		}
	}
	closedir(dir);
	return count;
}

void copy(FileInfo *paths, int count) {
	for (int i = 0; i < count; i++) {
		FILE *copy_file;
		FILE *location_file;
		copy_file = fopen(paths[i].copy_to, "w");
		location_file = fopen(paths[i].copy_to, "r");
		if (location_file != NULL || copy_file != NULL) {
			while (!feof(location_file)) {
				char sym = fgetc(location_file);
				fputc(sym, copy_file);
			}
			printf("Путь до файла %s Размер файла %d\n", paths[i].copy_to, paths[i].size);
		} else {
			printf("Не удалось скопировать файл %s в %s\n", paths[i].location, paths[i].copy_to);
			continue;
		}

		fclose(copy_file);
		fclose(location_file);
	}
}

int main(int argc, char *argv[]) {
	int count;
	DIR *location_dir = opendir(argv[1]);
	DIR *copy_dir = opendir(argv[3]);
	if (!location_dir) {
		if (mkdir(argv[1], 0777)) {
			printf("Одной из директорий в пути копируемой директории не существует или копируемая директория %s не может быть создана/открыта\n", argv[1]);
			return 1;
		}
	} else closedir(location_dir);

	if (!copy_dir) {
		if (mkdir(argv[3], 0777)) {
                        printf("Одной из директорий в пути директории назначения не существует или директория назначения %s не может быть создана/открыта\n", argv[3]);
			return 1;
		}
	} else closedir(copy_dir);

	if (atoi(argv[2]) == 1) {
		count = count_files(argv[1], argv[3]);
		FileInfo paths[count];
		getFileInfos(paths, argv[1], argv[3], 0);
		qsort(paths, count, sizeof(FileInfo), size_compare);
		copy(paths, count);
	} else if (atoi(argv[2]) == 2) {
                count = count_files(argv[1], argv[3]);
                FileInfo paths[count];
                getFileInfos(paths, argv[1], argv[3], 0);
                qsort(paths, count, sizeof(FileInfo), name_compare);
		copy(paths, count);
	} else {
		printf("Неверная сортировка. Вы можете выбрать:\n1 - Сортировка по размеру\n2 - Сортировка по названию\n");
		return 1;
	}
}

