#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>


char *BYTES;
pid_t *PROCESSES;
int MAX_PROCESSES;


int find_bytes_in_file(char *filename) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        printf("%s", strerror(errno));
        return -1;
    }

    int i = 0;
    int entries_count = 0;
    ssize_t total_bytes = 0;
    size_t sequence_len = strlen(BYTES);
    char c = 0;

    ssize_t bytes_read;
 
    while ((bytes_read = read(fd, &c, sizeof(char))) > 0) {
        total_bytes++;
        if (BYTES[i] == c) {
		i++;
	} else {
		total_bytes = total_bytes - i + 1;
                lseek(fd, total_bytes, SEEK_SET);
		i = 0;
	};
        if (sequence_len == i) {
            entries_count++;
            i = 0;
        }
    }

    if (bytes_read == -1) {
        printf("%s", strerror(errno));
        return -1;
    }

    if (close(fd) == -1) {
        printf("%s", strerror(errno));
        return -1;
    }

    printf("pid: %d  filename: %s  total views: %ld  found: %d\n", getpid(), filename, total_bytes, entries_count);
    
    exit(0);
}


int runProcess(char *filename, pid_t ppid, int process_num) {
	pid_t pid = fork();
	if (getpid() == ppid) {
		PROCESSES[process_num] = pid;
	} else {
		printf("number %d    ", process_num);
		find_bytes_in_file(filename);
	}
	return 0;
}

int readDir(char *dir_path) {
        DIR *dir;
        struct dirent *dir_struct;
        dir = opendir(dir_path);
	int process_num = -1;
        while ((dir_struct = readdir(dir)) != NULL) {
                if (dir_struct -> d_type == DT_DIR) {
                        continue;
		} else {
			char location[500];
			if (!strcmp(&dir_path[strlen(dir_path) - 1], "/")) {
				dir_path[strlen(dir_path) - 1] = '\0';
			}
			if (process_num == MAX_PROCESSES) {
				while(1) {
                                	int result = waitpid(PROCESSES[process_num], 0, WNOHANG);     
					if (result == PROCESSES[process_num] || result == -1) {
						break;
					} else {
					
                                                if (process_num < MAX_PROCESSES) {
                                                        process_num++;
                                                } else process_num = 0;
					}
				}
			} else process_num++;

                        sprintf(location, "%s/%s", dir_path, dir_struct -> d_name);
			runProcess(location, getpid(), process_num);
			}
		
	}
}

int count_files(char *dir_path) {
	DIR *dir;
	struct dirent *dir_struct;
	dir = opendir(dir_path);
	int count = 0;

	while ((dir_struct = readdir(dir)) != NULL) {
		if (dir_struct -> d_type != DT_DIR) {
			count++; 
		}
	}
	closedir(dir);
	return count;
}


int main (int argc, char *argv[]) {
    MAX_PROCESSES = atoi(argv[3]) - 1;
    if (atoi(argv[3]) < 1) {
        printf("Processes number must be bigger than 1.");
        return 1;
    }

    PROCESSES = calloc(sizeof(pid_t), atoi(argv[3]));

    BYTES = argv[1];
    readDir(argv[2]);
    
    while ((wait(0) > 0));
    return 0;
}

