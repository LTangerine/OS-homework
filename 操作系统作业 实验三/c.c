#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>


char buf[10];
sem_t *empty=NULL,*full=NULL; 

void *producer(void *arg) {
	int i;
	for (i = 0; i < 10; ) {
		sem_wait(empty);
		scanf("%c",&buf[i]);
		i=(i+1)%10;
		sem_post(full);
	}
	return NULL;
}

void *customer(void *arg) {
	int i;
	for (i = 0; i < 10; ) {
		sem_wait(full);
		printf("%c\n",buf[i]);
		i=(i+1)%10;
		sem_post(empty);
		sleep(1);
	}
	return NULL;
}

int main(int argc, char *argv[])
{
	empty=sem_open("mySemName1",O_CREAT,0666,10);
	full=sem_open("mySemName2",O_CREAT,0666,0);
	pthread_t p1, p2;
	pthread_create(&p1, NULL, producer, NULL);
	pthread_create(&p2, NULL, customer, NULL);
	pthread_join(p1, NULL);
	pthread_join(p2, NULL);
	sem_close(empty);
	sem_close(full);
	sem_unlink("mySemName1");
	sem_unlink("mySemName2");
	return 0;
}	
