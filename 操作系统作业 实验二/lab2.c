#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(){
	int res;
	res=execl("/usr/bin/vi","vi",NULL);
	pid_t pid;
	pid=fork();
	if(pid<0){
	perror("fork");
	}
	if(0==pid){
		while(1){
		printf("child process");
		if(res==-1){
			perror("execl");
		}
		sleep(1);
		}	
	}
	else if (pid>0){
		while(1){
		printf("parent process");
		sleep(1);
		}
	}
	return 0;
}