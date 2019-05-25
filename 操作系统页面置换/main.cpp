#include<iostream>
using namespace std;
#include <stdio.h>
#include<ctime>
#include<algorithm>
#include <iomanip>
#define MAX 100
#define MAX1 20
#define randomNum(a,b) rand() % (b - a + 1) + a//产生[a,b]之间的数
#define random(x) rand()%x
//-----------------数据定义------------
int N;            //虚拟内存尺寸
int e;   //页面个数
int p;        //工作集起始页号
int m;     //工作集移动速率
double t;
double r;
clock_t start,finish;     //开始时间以及结束时间
double a1,a2,b1,b2,c1,c2,d1,d2,e1,e2;
int vpage=0;             //页面访问变量
int rpage;             //页面替换变量
int pageframe[MAX1];      // 分配的页框，存放物理块
int pagehistory[MAX1];    //记录页框中数据的访问历史,lru置换
int pagefuture[MAX1];     //记录页框中未访问数据，opt置换
int A[MAX];     //页面访问位，改进clock置换
int M[MAX];  //页面修改位，改进clock置换

struct PageInfo       //页面信息结构
{
   int List[MAX];  //页面序列
   int flag;         // 标志位，1表示无页面访问数据
   int dispage_num;    // 缺页次数
   int pf;     // 分配的物理块数
   int pn;     // 访问页面序列长度
}pageInfo;

void createRandom();      //随机生成访问序列
void opt();          //最佳置换算法
void fifo();          //先进先出算法
void lru();           //最近最久未使用算法
void Mclock();        //改进型Clock置换算法
int Mloop(int f);      //循环扫描
void displayInfo();   //显示当前状态及缺页情况
int sreachPage(int page);      //查找页面是否在内存
void compare();   //性能比较


//页面缓冲算法PBA
struct LNode
{
    int data;
    int flag;//访问位
    int modify;//修改位
    LNode* next;
};
struct Link
{
    int num;//当前链表上的结点数
    LNode* next;
};
int size=3;
int access[MAX]; //访问序列
int lost=0;//缺页数
int index=0;//当前页面
LNode* nodes;
Link idle;
Link modified;
bool isInNodes(int n);
void addToLink(int data,int type);
void emptyIdle();
void emptyModi();
void PBA(int n);
void PBAmian();


int main()
{
    int i,j;
    createRandom();        // 随机生成访问序列
    //opt
    printf("\n\n----------*****执行opt算法*****-----------\n");
    opt();
    //fifo
    printf("\n\n----------*****执行FIFO算法*****-----------\n");
    fifo();
    //lru
    printf("\n\n----------*****执行LRU算法*****----------\n");
    lru();
    //改进clock
    printf("修改位：\n");
    for(i=0;i<pageInfo.pn;i++){
        printf("%d  ",M[i]);
    }
    printf("\n\n----------*****执行改进型clock算法*****-----------\n");
    Mclock();
    //PBA
    printf("\n\n----------*****执行PBA算法*****-----------\n");
    PBAmian();
    compare();
    return 0;
}

void compare(){
	cout<<"名称    "<<"\t"<<"opt"<<"\t"<<"fifo"<<"\t"<<"lru"<<"\t"<<"Mclock"<<"\t"<<"PBA"<<endl;
	cout<<"缺页率  "<<"\t"<<setprecision(2)<<a1<<"%\t"<<b1<<"%\t"<<c1<<"%\t"<<d1<<"%\t"<<e1<<"%"<<endl;
	cout<<"时间开销"<<"\t"<<setprecision(4)<<a2<<"\t"<<b2<<"\t"<<c2<<"\t"<<d2<<"\t"<<e2<<endl;
}

// 随机生成访问序列
void createRandom(void )
{
    //初始化结构体pageInfo
    int i;
    int pf,pn;
	pageInfo.dispage_num=0;   // 缺页次数
	pageInfo.flag=1;        // 标志位，1表示无页面访问数据
    cout<<"请输入要分配的页框数：";
	cin>>pf;
	cout<<"请设置生成的页面访问序列的个数:";
	cin>>pn;
	pageInfo.pf=pf;
    pageInfo.pn=pn;
    for(i=0;i<MAX;i++){   // 清空页面序列
       pageInfo.List[i]=-1;
    }
    //输入参数
	cout<<"请设置虚拟内存的尺寸N:";
	cin>>N;
	cout<<"请设置起始位置P：";
	cin>>p;
	cout<<"请设置的工作集页数e：";
	cin>>e;
	cout<<"请设置工作集移动速率m：";
	cin>>m;
	cout<<"请设置一个范围在0和1之间的值t：";
	cin>>t;
	cout<<"---------------------------------" << endl;
    /*pageInfo.dispage_num=0;   // 缺页次数
	pageInfo.flag=1;        // 标志位，1表示无页面访问数据
	pageInfo.pf=3;
    pageInfo.pn=20;
    for(int i=0;i<MAX;i++){   // 清空页面序列
       pageInfo.List[i]=-1;
    }
    N=10;
    p=1;
    e=4;
    m=4;
    t=0.5;*/
	srand((int)time(0));  //给一个随机种子数，有这个数以后才可以产生随机数
	int j=0;
	for(int i=0;i<pageInfo.pn;i++) {
		if(j<m){//产生m个取值范围在p和p + e间的随机数，并记录到页面访问序列串中
			int x=p+e,y=N-1;
			pageInfo.List[i]=randomNum(p,min(x,y));
			access[i]=pageInfo.List[i];
			double sn=random(10)*0.1;
			if(sn>0.6){
				M[i]=1;//以写方式访问
			}
			else{
				M[i]=0;//以读方式访问
			}
			j++;
		}
		else{
			r=random(10)*0.1;
			j=0;
			if(r<t){
				p=random(N);
			}
			else{
				p=(p+1)%N;
			}
			i--;
		}
	}
}


//最佳置换算法
void opt()
{
    int i,Count,pstate,Min;
    int t[pageInfo.pf]={0};
    pageInfo.dispage_num=0;
    rpage=0;
    Count=0;
    for(i=0;i<pageInfo.pf;i++){ // 清除页框信息
        pageframe[i]=-1;
    }
    start=clock();
    pageInfo.flag=1;   //缺页标志，0为缺页，1为不缺页
    for(vpage=0;vpage<pageInfo.pn;vpage++){
        pstate=sreachPage(pageInfo.List[vpage]);  //查找页面是否在内存
        if(Count<pageInfo.pf){    // 开始物理块不计入缺页
            if(pstate==0){
			pageframe[rpage]=pageInfo.List[vpage];
			rpage=(rpage+1)%pageInfo.pf;
			Count++;
            }
        }
        else{       // 正常缺页置换
            if(pstate==0){//缺页置换
                for(i=0;i<pageInfo.pf;i++)
                    t[i]=0;
                int j=0,c=0,b=0;
                Min=0;
                for(i=vpage;i<pageInfo.pn;i++){
                    for(j=0;j<pageInfo.pf;j++){
                        if(pageframe[j]==pageInfo.List[i]){
                            pagefuture[j]=0;
                            for(c=0;c<j;c++)//j之前
                                pagefuture[c]++;
                            for(c=j+1;c<pageInfo.pf;c++)//j之后
                                pagefuture[c]++;
                            t[j]++;
                        }
                    }
                }
                for(i=0;i<pageInfo.pf;i++){
                    if(t[i]==0){
                        Min=i;
                        break;
                    }
                    else
                        b++;
                }
                if(b==pageInfo.pf){
                    for(i=1;i<pageInfo.pf;i++){
                        if(pagefuture[i]<pagefuture[Min])
                            Min=i;
                    }
                }
                rpage=Min;
                pageframe[rpage]=pageInfo.List[vpage];
                pageInfo.dispage_num++;  // 缺页次数加1
            }
        }
        displayInfo();   // 显示当前状态
    }
    finish=clock();
    a1=(float)(pageInfo.dispage_num)*100.0/pageInfo.pn;
    printf("缺页率%3.1f\n",a1);
    a2=double(finish - start)/CLOCKS_PER_SEC;
    cout <<"算法开销时间:"<<setprecision(4)<<a2<< endl;
}

//FIFO页面置换算法
void fifo(void)
{
    int i,Count,pstate;
    pageInfo.dispage_num=0;
    rpage=0;
    Count=0;
    for(i=0;i<pageInfo.pf;i++){ // 清除页框信息
        pageframe[i]=-1;
    }
    start=clock();
    pageInfo.flag=1;   //缺页标志，0为缺页，1为不缺页
    for(vpage=0;vpage<pageInfo.pn;vpage++){
        pstate=sreachPage(pageInfo.List[vpage]);  //查找页面是否在内存
        if(Count<pageInfo.pf){    // 开始物理块不计算缺页
            if(pstate==0){
			pageframe[rpage]=pageInfo.List[vpage];
			rpage=(rpage+1)%pageInfo.pf;
			Count++;
            }
        }
        else{       // 正常缺页置换
            if(pstate==0){
                pageframe[rpage]=pageInfo.List[vpage];
				rpage=(rpage+1)%pageInfo.pf;
				pageInfo.dispage_num++;     // 缺页次数加1
            }
        }
	    displayInfo();       // 显示当前状态
    }
    finish=clock();
    b1=(float)(pageInfo.dispage_num)*100.0/pageInfo.pn;
    printf("缺页率%3.1f\n",b1);
    b2=double(finish - start)/CLOCKS_PER_SEC;
    cout <<"算法开销时间:"<<setprecision(4)<<b2<< endl;
}

//LRU页面置换算法
void lru(void)
{
    int i,Count,pstate,max;
    pageInfo.dispage_num=0;
    rpage=0;
    Count=0;
    for(i=0;i<pageInfo.pf;i++){
	  pageframe[i]=-1;    // 清除页框信息
	  pagehistory[i]=0;   // 清除页框历史
    }
    start=clock();
    pageInfo.flag=1;    //缺页标志，0为缺页，1为不缺页
    for(vpage=0;vpage<pageInfo.pn;vpage++){  // 执行算法
        pstate=sreachPage(pageInfo.List[vpage]);  //查找页面是否在内存
	    if(Count<pageInfo.pf){   // 开始物理块不计算缺页
	        if(pstate==0){
                pageframe[rpage]=pageInfo.List[vpage];
                rpage=(rpage+1)%pageInfo.pf;
                Count++;
            }
	    }
	    else{ // 正常缺页置换
            if(pstate==0){
            max=0;
            for(i=1;i<pageInfo.pf;i++){
			    if(pagehistory[i]>pagehistory[max]){
			        max=i;
				}
            }
            rpage=max;
            pageframe[rpage]=pageInfo.List[vpage];
            pagehistory[rpage]=0;
            pageInfo.dispage_num++;  // 缺页次数加1
            }
        }
	    displayInfo();    // 显示当前状态
    }
    finish=clock();
    c1=(float)(pageInfo.dispage_num)*100.0/pageInfo.pn;
    printf("缺页率%3.1f\n",c1);
    c2=double(finish - start)/CLOCKS_PER_SEC;
    cout <<"算法开销时间:"<<setprecision(4)<<c2<< endl;
}

//改进型Clock置换算法
void Mclock(){
    int i,Count,pstate,t;
    pageInfo.dispage_num=0;
    rpage=0;
    Count=0;
    for(i=0;i<pageInfo.pf;i++){
	  pageframe[i]=-1;    // 清除页框信息
    }
    //初始化将所有页的访问位置0
	for(i=0;i<pageInfo.pf;i++) {
		A[i]=0;
	}
    start=clock();
    pageInfo.flag=1;    //缺页标志，0为缺页，1为不缺页
    //printf("\n");
    for(vpage=0;vpage<pageInfo.pn;vpage++){  // 执行算法
        pstate=sreachPage(pageInfo.List[vpage]);  //查找页面是否在内存
        //printf("vpage=%d,pageInfo.List=%d\n",vpage,pageInfo.List[vpage]);
	    if(Count<pageInfo.pf){   // 开始物理块不计算缺页
	        if(pstate==0){
                pageframe[rpage]=pageInfo.List[vpage];
                rpage=(rpage+1)%pageInfo.pf;
                A[Count]=1;
                Count++;
            }
	    }
	    else{ // 正常缺页置换
            if(pstate==0){
                t=Mloop(vpage);
                rpage=t;
                //printf("T=%d\n",t);
                pageframe[rpage]=pageInfo.List[vpage];
                A[rpage]=1;
                pageInfo.dispage_num++;
            }
	    }
	    displayInfo();    // 显示当前状态
    }
    finish=clock();
    d1=(float)(pageInfo.dispage_num)*100.0/pageInfo.pn;
    printf("缺页率%3.1f\n",d1);
    d2=double(finish - start)/CLOCKS_PER_SEC;
    cout <<"算法开销时间:"<<setprecision(4)<<d2<< endl;
}

//循环扫描
//① 从查寻指针当前位置起扫描内存分页循环队列，选择A=0且M=0的第一个页面淘汰；若未找到，转②
//② 开始第二轮扫描，选择A=0且M=1的第一个页面淘汰，同时将经过的所有页面访问位置0；若不能找到，转①
int Mloop(int f){
    int i,j;
    while(1){
        for(i=0,j=f;i<pageInfo.pf&&j<pageInfo.pn;i++,j++){
            if(A[i]==0&&M[j]==0){
                return i;
            }
        }
        for(i=0,j=f;i<pageInfo.pf&&j<pageInfo.pn;i++,j++){
            if(A[i]==0&&M[j]==1){
                return i;
            }
            A[i]=0;
        }
    }
    return pageInfo.pf+1;
}

//显示当前状态及缺页情况
void displayInfo(void)
{
    int i,j;
    if(vpage==0){
        printf("\n=============页面访问序列=============\n");
        for(i=0;i<pageInfo.pn;i++){
            printf("%4d",pageInfo.List[i]);
		    if((i+1)%10==0)
                printf("\n");   //每行显示10个
        }
        printf("\n======================================\n");
    }
    printf("访问%3d : 内存<",pageInfo.List[vpage]);
    for(j=0;j<pageInfo.pf;j++){     // 页框信息
        if(pageframe[j]>=0)
            printf("%3d",pageframe[j]);
        else
            printf("   ");
    }
    printf(" >");
    if(pageInfo.flag==0){     //缺页标志，0为缺页，1为不缺页
        printf(" ==>缺页次数:%d",pageInfo.dispage_num);
    }
    printf("\n");
}

//查找页面是否存在，1为存在，0为不存在即缺页
int sreachPage(int page)
{
    int i;
    for(i=0;i<pageInfo.pf;i++){
        pagehistory[i]++;   // 访问历史加1
    }
	for(i=0;i<pageInfo.pf;i++){
        if(pageframe[i]==page){
            pageInfo.flag=1;    //0为缺页，1为不缺页
		    pagehistory[i]=0;    //置访问历史为0
		    return 1;
        }
	}
	pageInfo.flag=0; //0为缺页，1为不缺页
    return 0;
}

bool isInNodes(int n)
{
    int i;
    for(i=0;i<3;i++){
        if(nodes[i].data==access[n])
            return true;
    }
    return false;
}

LNode* isinLinks(int n)
{
    LNode*p,*q;
    p=idle.next;
    q=NULL;
    while(p){
        if(p->data==access[n]){
            if(q!=NULL){
                q->next=p->next;
                p->next=NULL;
                idle.num--;
                break;
            }
            else
                idle.next=NULL;
        }
        q=p;
        p=p->next;
    }
    if(p==NULL){
        p=modified.next;
        while(p!=NULL){
            if(p->data==access[n]){
                if(p==modified.next)
                    modified.next = p->next;
                else{
                    q->next=p->next;
                    p->next=NULL;
                    modified.num--;
                }
                if(modified.num==0)
                    modified.next = NULL;
                break;
            }
            q=p;
            p=p->next;
        }
    }
    return p;
}


void PBA(int n)
{
    if(isInNodes(n))
        printf ("已装入内存\n");
    else
        if(index==size){
            LNode *p;
            if((p=isinLinks(n))!=NULL){
                nodes=(LNode*)realloc(nodes,(size + 1)*sizeof(LNode));
                nodes[size].data=p->data;
                nodes[size].flag=p->flag;
                nodes[size].modify=p->modify;
                nodes[size].next=p->next;
                free(p);
                size++;
                index++;
            }
            else{
                lost++;//缺页
                if(nodes[n%3].modify==1)
                    addToLink(nodes[n%3].data,1);
                else
                    addToLink(nodes[n%3].data,0);
                nodes[n%3].data=access[n];
                nodes[n%3].flag=1;
                nodes[n%3].next=NULL;
                if(rand()%10<4)
                    nodes[n%3].modify=0;
                else
                    nodes[n%3].modify=1;
            }
        }
        else{
            nodes[index].data=access[n];
            nodes[index].flag=1;
            nodes[index].next=NULL;
            if(rand()%10<4)
                nodes[index].modify=1;
            else
                nodes[index].modify=0;
            index++;
        }
}

void addToLink(int data,int type)
{
    LNode* p;
    LNode* q;
    q=(LNode*)malloc(sizeof(LNode));
    q->data=data;
    q->flag=1;
    if(type==1){
        q->modify=1;
        p=modified.next;
    }
    else{
        q->modify=0;
        p=idle.next;
    }
    q->next=NULL;
    if(p==NULL){
        if(type==0)
            idle.next=q;
        else
            modified.next=q;
    }
    else{
        while(p){
            if(p->next==NULL){
                p->next=q;
                break;
            }
            else
                p=p->next;
        }
    }
    if(type==0){
        idle.num+=1;
        if(idle.num==10)
            emptyIdle();
    }
    else{
        modified.num+=1;
        if(modified.num==10)
            emptyModi();
    }
}
void emptyIdle()
{
    LNode* p;
    p=idle.next;
    while(p){
        idle.next=p->next;
        free(p);
        p=idle.next;
    }
    idle.num=0;
}
void emptyModi()
{
    LNode* p;
    p=modified.next;
    while(p){
        modified.next=p->next;
        free(p);
        p=modified.next;
    }
    modified.num=0;
}

void PBAmian(){
    int i,j;
    printf("\n=============页面访问序列=============\n");
    for(i=0;i<pageInfo.pn;i++){
        printf("%4d",access[i]);
        if((i+1)%10==0)
            printf("\n");   //每行显示10个
    }
    printf("\n======================================\n");
    idle.num=0;
    idle.next=NULL;
    modified.num=0;
    modified.next=NULL;
    nodes=(LNode*)malloc(size*sizeof(LNode));
    for(i=0;i<size;i++){
        nodes[i].data=-1;
        nodes[i].flag=0;
        nodes[i].modify=0;
        nodes[i].next=NULL;
    }
    start=clock();
    for(i=0;i<32;i++){
        PBA(i);
       // printf("i=%d\n",i);
       printf("<");
        for(j=0;j<size;j++)
            printf("%2d",nodes[j].data);
        printf(">  ==>缺页lost=%d\n",lost);
    }
    finish=clock();
    e1=(float)(lost)*100.0/pageInfo.pn;
    printf("缺页率%3.1f\n",e1);
    e2=double(finish - start)/CLOCKS_PER_SEC;
    cout <<"算法开销时间:"<<setprecision(4)<<e2<< endl;
}
