#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>

volatile int ticketCount=1000;
sem_t *mySem = NULL;
void *SaleTicket(void *arg)
{
    int num,temp;
    num=atoi(arg)+1;//将字符串arg转换成一个整数并返回结果
    for(int i=1;i<num;i++)
    {
        sem_wait(mySem);
        temp=ticketCount;
        //放弃CPU，强制切换到另外一个进程
        pthread_yield();
        temp=temp-1;
        pthread_yield();
        ticketCount=temp;
        printf("售票线程：售出%d张票,当前票数为%d张\n",i,ticketCount); 
        sem_post(mySem);
    }
    return NULL;
}

void *RefundTicket(void *arg)
{
    int num,temp;
    num=atoi(arg)+1;
    for(int i=1;i<num;i++)
    {
        sem_wait(mySem);
        temp=ticketCount;
        pthread_yield();
        temp=temp+1;
        pthread_yield();
        ticketCount=temp;
        printf("退票线程：退票%d张，当前票数为%d张\n",i,ticketCount);  
        sem_post(mySem);
    }
    return NULL;
}
int main(int argc,char *argv[])
{
    if (argc != 3) {
    fprintf(stderr, "usage: threads <value>\n");
    exit(1);
    }
    mySem = sem_open("mySemName1", O_CREAT, 0666, 1);
    printf("余票初始值：%d\n",ticketCount);
    pthread_t p1,p2;
    pthread_create(&p1,NULL,SaleTicket,argv[1]);
    pthread_create(&p2,NULL,RefundTicket,argv[2]);
    pthread_join(p1,NULL);
    pthread_join(p2,NULL);  
    sem_close(mySem);
    sem_unlink("mySemName");
    printf("最终余票：%d\n",ticketCount);
    return 0;
}