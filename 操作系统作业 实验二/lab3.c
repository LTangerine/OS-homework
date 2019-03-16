#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(){
	int p1,p2,p3,p4,p5;
	while((p1=fork())==-1);
	if(p1==0){
		printf("p2 is p1's child peocess pid: %d,p1 is parent process pid: %d\n",getpid(),getppid());
		while((p2=fork())==-1);
		if(p2==0){//4				
			printf("p4 is p2's child peocess pid: %d,p2 is parent process pid: %d\n",getpid(),getppid());
		}				
		else{
			while((p5=fork())==-1);
			if(p5>0)sleep(1);
	                               else if(p5==0){					
				printf("p5 is p2's child peocess pid: %d,p2 is parent process pid: %d\n",getpid(),getppid());
			}
		}
	}

	else{
	   printf("p1 is parent peocess pid: %d\n",getpid());		
	   while((p3=fork())==-1);
	   if(p3>0)sleep(1);
	   else if(p3==0){
	  	printf("p3 is p1's child peocess pid: %d,p1 is parent process pid: %d\n",getpid(),getppid());		
	  }
	}
}

