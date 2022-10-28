#include <dirent.h> 
#include <stdio.h> 
#include <string.h>

int readDir(char *dir_name) {
	DIR *d;
	struct dirent *dir_struct;
	d = opendir(dir_name);

	if (d) {
    		while ((dir_struct = readdir(d)) != NULL) {
      			printf("%s/%s\n", dir_name, dir_struct -> d_name);
    		}
	}
	else {
		printf("Ошибка чтения");
		return 1;
	}

	if (closedir(d)) {
		printf("Ошибка закрытия");
		return 1;
	}

  	return 0;
}


int main(int argc, char *argv[]) {
  	if (readDir(".")) {
		return 1;
	  }
	
	return readDir(argv[1]);
}

