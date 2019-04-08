#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>

pthread_mutex_t mutex;
sem_t *a=NULL,*b=NULL,*c=NULL,*d=NULL;
   
int main(){
    pid_t p1,p2,p3,p4;
    pthread_mutex_init(&mutex,NULL);
    a = sem_open("P1P2", O_CREAT, 0666, 0);
    b = sem_open("P1P3", O_CREAT, 0666, 0);
    c = sem_open("P2P4", O_CREAT, 0666, 0);
    d = sem_open("P3P4", O_CREAT, 0666, 0);
    while((p2=fork())==-1);//创建进程P2
    if(p2==0)
    {
        sem_wait(a);
        pthread_mutex_lock(&mutex);//p2p3互斥
        printf("I am the process P2\n");
        sem_post(c);
        pthread_mutex_unlock(&mutex);
    }
    if(p2>0)
    {
        while((p3=fork())==-1);
        if(p3==0)
        {
            //sleep(1);
            sem_wait(b);
            pthread_mutex_lock(&mutex);//p2p3互斥
            printf("I am the process P3\n");
            sem_post(d);
            pthread_mutex_unlock(&mutex);
        }
        if(p3>0)
        {
            printf("I am the process P1\n");
            sem_post(a);
            sem_post(b);
            while((p4=fork())==-1);
            if(p4==0)
            {
                sem_wait(c);
                sem_wait(d);
                printf("I am the process P4\n");
                sem_post(c);
                sem_post(d);
            }
        }

    }
    sleep(1);
    sem_close(a);
    sem_close(b);
    sem_close(c);
    sem_close(d);
    sem_unlink("P1P2");
    sem_unlink("P1P3");
    sem_unlink("P2P4");
    sem_unlink("P3P4");
    return 0;
}