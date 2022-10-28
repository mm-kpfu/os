#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
	pid_t parent_pid = getpid();
	pid_t child_pids[2];

	for (int i = 0; i < 2; i++) {
		if (parent_pid == getpid()) {
			child_pids[i] = fork();
		}
	}

	time_t rawtime;
  	struct tm * timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	char formatted[100];
	strftime(formatted, 100, "%H:%M:%S", timeinfo);
	printf("My pid: %d\nParent pid: %d\nTime: %s\n\n", getpid(), getppid(), formatted);
	
	if (parent_pid == getpid()) {
		system("ps -x | grep first_process");
	}

	int status;

	for (int i; i < 2; i++) {
		waitpid(child_pids[i], &status, WUNTRACED);
	}
	return 0;
}
