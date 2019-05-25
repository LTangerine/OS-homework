#include<iostream>
using namespace std;
#include <stdio.h>
#include<ctime>
#include<algorithm>
#include <iomanip>
#define MAX 100
#define MAX1 20
#define randomNum(a,b) rand() % (b - a + 1) + a//����[a,b]֮�����
#define random(x) rand()%x
//-----------------���ݶ���------------
int N;            //�����ڴ�ߴ�
int e;   //ҳ�����
int p;        //��������ʼҳ��
int m;     //�������ƶ�����
double t;
double r;
clock_t start,finish;     //��ʼʱ���Լ�����ʱ��
double a1,a2,b1,b2,c1,c2,d1,d2,e1,e2;
int vpage=0;             //ҳ����ʱ���
int rpage;             //ҳ���滻����
int pageframe[MAX1];      // �����ҳ�򣬴�������
int pagehistory[MAX1];    //��¼ҳ�������ݵķ�����ʷ,lru�û�
int pagefuture[MAX1];     //��¼ҳ����δ�������ݣ�opt�û�
int A[MAX];     //ҳ�����λ���Ľ�clock�û�
int M[MAX];  //ҳ���޸�λ���Ľ�clock�û�

struct PageInfo       //ҳ����Ϣ�ṹ
{
   int List[MAX];  //ҳ������
   int flag;         // ��־λ��1��ʾ��ҳ���������
   int dispage_num;    // ȱҳ����
   int pf;     // ������������
   int pn;     // ����ҳ�����г���
}pageInfo;

void createRandom();      //������ɷ�������
void opt();          //����û��㷨
void fifo();          //�Ƚ��ȳ��㷨
void lru();           //������δʹ���㷨
void Mclock();        //�Ľ���Clock�û��㷨
int Mloop(int f);      //ѭ��ɨ��
void displayInfo();   //��ʾ��ǰ״̬��ȱҳ���
int sreachPage(int page);      //����ҳ���Ƿ����ڴ�
void compare();   //���ܱȽ�


//ҳ�滺���㷨PBA
struct LNode
{
    int data;
    int flag;//����λ
    int modify;//�޸�λ
    LNode* next;
};
struct Link
{
    int num;//��ǰ�����ϵĽ����
    LNode* next;
};
int size=3;
int access[MAX]; //��������
int lost=0;//ȱҳ��
int index=0;//��ǰҳ��
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
    createRandom();        // ������ɷ�������
    //opt
    printf("\n\n----------*****ִ��opt�㷨*****-----------\n");
    opt();
    //fifo
    printf("\n\n----------*****ִ��FIFO�㷨*****-----------\n");
    fifo();
    //lru
    printf("\n\n----------*****ִ��LRU�㷨*****----------\n");
    lru();
    //�Ľ�clock
    printf("�޸�λ��\n");
    for(i=0;i<pageInfo.pn;i++){
        printf("%d  ",M[i]);
    }
    printf("\n\n----------*****ִ�иĽ���clock�㷨*****-----------\n");
    Mclock();
    //PBA
    printf("\n\n----------*****ִ��PBA�㷨*****-----------\n");
    PBAmian();
    compare();
    return 0;
}

void compare(){
	cout<<"����    "<<"\t"<<"opt"<<"\t"<<"fifo"<<"\t"<<"lru"<<"\t"<<"Mclock"<<"\t"<<"PBA"<<endl;
	cout<<"ȱҳ��  "<<"\t"<<setprecision(2)<<a1<<"%\t"<<b1<<"%\t"<<c1<<"%\t"<<d1<<"%\t"<<e1<<"%"<<endl;
	cout<<"ʱ�俪��"<<"\t"<<setprecision(4)<<a2<<"\t"<<b2<<"\t"<<c2<<"\t"<<d2<<"\t"<<e2<<endl;
}

// ������ɷ�������
void createRandom(void )
{
    //��ʼ���ṹ��pageInfo
    int i;
    int pf,pn;
	pageInfo.dispage_num=0;   // ȱҳ����
	pageInfo.flag=1;        // ��־λ��1��ʾ��ҳ���������
    cout<<"������Ҫ�����ҳ������";
	cin>>pf;
	cout<<"���������ɵ�ҳ��������еĸ���:";
	cin>>pn;
	pageInfo.pf=pf;
    pageInfo.pn=pn;
    for(i=0;i<MAX;i++){   // ���ҳ������
       pageInfo.List[i]=-1;
    }
    //�������
	cout<<"�����������ڴ�ĳߴ�N:";
	cin>>N;
	cout<<"��������ʼλ��P��";
	cin>>p;
	cout<<"�����õĹ�����ҳ��e��";
	cin>>e;
	cout<<"�����ù������ƶ�����m��";
	cin>>m;
	cout<<"������һ����Χ��0��1֮���ֵt��";
	cin>>t;
	cout<<"---------------------------------" << endl;
    /*pageInfo.dispage_num=0;   // ȱҳ����
	pageInfo.flag=1;        // ��־λ��1��ʾ��ҳ���������
	pageInfo.pf=3;
    pageInfo.pn=20;
    for(int i=0;i<MAX;i++){   // ���ҳ������
       pageInfo.List[i]=-1;
    }
    N=10;
    p=1;
    e=4;
    m=4;
    t=0.5;*/
	srand((int)time(0));  //��һ���������������������Ժ�ſ��Բ��������
	int j=0;
	for(int i=0;i<pageInfo.pn;i++) {
		if(j<m){//����m��ȡֵ��Χ��p��p + e��������������¼��ҳ��������д���
			int x=p+e,y=N-1;
			pageInfo.List[i]=randomNum(p,min(x,y));
			access[i]=pageInfo.List[i];
			double sn=random(10)*0.1;
			if(sn>0.6){
				M[i]=1;//��д��ʽ����
			}
			else{
				M[i]=0;//�Զ���ʽ����
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


//����û��㷨
void opt()
{
    int i,Count,pstate,Min;
    int t[pageInfo.pf]={0};
    pageInfo.dispage_num=0;
    rpage=0;
    Count=0;
    for(i=0;i<pageInfo.pf;i++){ // ���ҳ����Ϣ
        pageframe[i]=-1;
    }
    start=clock();
    pageInfo.flag=1;   //ȱҳ��־��0Ϊȱҳ��1Ϊ��ȱҳ
    for(vpage=0;vpage<pageInfo.pn;vpage++){
        pstate=sreachPage(pageInfo.List[vpage]);  //����ҳ���Ƿ����ڴ�
        if(Count<pageInfo.pf){    // ��ʼ����鲻����ȱҳ
            if(pstate==0){
			pageframe[rpage]=pageInfo.List[vpage];
			rpage=(rpage+1)%pageInfo.pf;
			Count++;
            }
        }
        else{       // ����ȱҳ�û�
            if(pstate==0){//ȱҳ�û�
                for(i=0;i<pageInfo.pf;i++)
                    t[i]=0;
                int j=0,c=0,b=0;
                Min=0;
                for(i=vpage;i<pageInfo.pn;i++){
                    for(j=0;j<pageInfo.pf;j++){
                        if(pageframe[j]==pageInfo.List[i]){
                            pagefuture[j]=0;
                            for(c=0;c<j;c++)//j֮ǰ
                                pagefuture[c]++;
                            for(c=j+1;c<pageInfo.pf;c++)//j֮��
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
                pageInfo.dispage_num++;  // ȱҳ������1
            }
        }
        displayInfo();   // ��ʾ��ǰ״̬
    }
    finish=clock();
    a1=(float)(pageInfo.dispage_num)*100.0/pageInfo.pn;
    printf("ȱҳ��%3.1f\n",a1);
    a2=double(finish - start)/CLOCKS_PER_SEC;
    cout <<"�㷨����ʱ��:"<<setprecision(4)<<a2<< endl;
}

//FIFOҳ���û��㷨
void fifo(void)
{
    int i,Count,pstate;
    pageInfo.dispage_num=0;
    rpage=0;
    Count=0;
    for(i=0;i<pageInfo.pf;i++){ // ���ҳ����Ϣ
        pageframe[i]=-1;
    }
    start=clock();
    pageInfo.flag=1;   //ȱҳ��־��0Ϊȱҳ��1Ϊ��ȱҳ
    for(vpage=0;vpage<pageInfo.pn;vpage++){
        pstate=sreachPage(pageInfo.List[vpage]);  //����ҳ���Ƿ����ڴ�
        if(Count<pageInfo.pf){    // ��ʼ����鲻����ȱҳ
            if(pstate==0){
			pageframe[rpage]=pageInfo.List[vpage];
			rpage=(rpage+1)%pageInfo.pf;
			Count++;
            }
        }
        else{       // ����ȱҳ�û�
            if(pstate==0){
                pageframe[rpage]=pageInfo.List[vpage];
				rpage=(rpage+1)%pageInfo.pf;
				pageInfo.dispage_num++;     // ȱҳ������1
            }
        }
	    displayInfo();       // ��ʾ��ǰ״̬
    }
    finish=clock();
    b1=(float)(pageInfo.dispage_num)*100.0/pageInfo.pn;
    printf("ȱҳ��%3.1f\n",b1);
    b2=double(finish - start)/CLOCKS_PER_SEC;
    cout <<"�㷨����ʱ��:"<<setprecision(4)<<b2<< endl;
}

//LRUҳ���û��㷨
void lru(void)
{
    int i,Count,pstate,max;
    pageInfo.dispage_num=0;
    rpage=0;
    Count=0;
    for(i=0;i<pageInfo.pf;i++){
	  pageframe[i]=-1;    // ���ҳ����Ϣ
	  pagehistory[i]=0;   // ���ҳ����ʷ
    }
    start=clock();
    pageInfo.flag=1;    //ȱҳ��־��0Ϊȱҳ��1Ϊ��ȱҳ
    for(vpage=0;vpage<pageInfo.pn;vpage++){  // ִ���㷨
        pstate=sreachPage(pageInfo.List[vpage]);  //����ҳ���Ƿ����ڴ�
	    if(Count<pageInfo.pf){   // ��ʼ����鲻����ȱҳ
	        if(pstate==0){
                pageframe[rpage]=pageInfo.List[vpage];
                rpage=(rpage+1)%pageInfo.pf;
                Count++;
            }
	    }
	    else{ // ����ȱҳ�û�
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
            pageInfo.dispage_num++;  // ȱҳ������1
            }
        }
	    displayInfo();    // ��ʾ��ǰ״̬
    }
    finish=clock();
    c1=(float)(pageInfo.dispage_num)*100.0/pageInfo.pn;
    printf("ȱҳ��%3.1f\n",c1);
    c2=double(finish - start)/CLOCKS_PER_SEC;
    cout <<"�㷨����ʱ��:"<<setprecision(4)<<c2<< endl;
}

//�Ľ���Clock�û��㷨
void Mclock(){
    int i,Count,pstate,t;
    pageInfo.dispage_num=0;
    rpage=0;
    Count=0;
    for(i=0;i<pageInfo.pf;i++){
	  pageframe[i]=-1;    // ���ҳ����Ϣ
    }
    //��ʼ��������ҳ�ķ���λ��0
	for(i=0;i<pageInfo.pf;i++) {
		A[i]=0;
	}
    start=clock();
    pageInfo.flag=1;    //ȱҳ��־��0Ϊȱҳ��1Ϊ��ȱҳ
    //printf("\n");
    for(vpage=0;vpage<pageInfo.pn;vpage++){  // ִ���㷨
        pstate=sreachPage(pageInfo.List[vpage]);  //����ҳ���Ƿ����ڴ�
        //printf("vpage=%d,pageInfo.List=%d\n",vpage,pageInfo.List[vpage]);
	    if(Count<pageInfo.pf){   // ��ʼ����鲻����ȱҳ
	        if(pstate==0){
                pageframe[rpage]=pageInfo.List[vpage];
                rpage=(rpage+1)%pageInfo.pf;
                A[Count]=1;
                Count++;
            }
	    }
	    else{ // ����ȱҳ�û�
            if(pstate==0){
                t=Mloop(vpage);
                rpage=t;
                //printf("T=%d\n",t);
                pageframe[rpage]=pageInfo.List[vpage];
                A[rpage]=1;
                pageInfo.dispage_num++;
            }
	    }
	    displayInfo();    // ��ʾ��ǰ״̬
    }
    finish=clock();
    d1=(float)(pageInfo.dispage_num)*100.0/pageInfo.pn;
    printf("ȱҳ��%3.1f\n",d1);
    d2=double(finish - start)/CLOCKS_PER_SEC;
    cout <<"�㷨����ʱ��:"<<setprecision(4)<<d2<< endl;
}

//ѭ��ɨ��
//�� �Ӳ�Ѱָ�뵱ǰλ����ɨ���ڴ��ҳѭ�����У�ѡ��A=0��M=0�ĵ�һ��ҳ����̭����δ�ҵ���ת��
//�� ��ʼ�ڶ���ɨ�裬ѡ��A=0��M=1�ĵ�һ��ҳ����̭��ͬʱ������������ҳ�����λ��0���������ҵ���ת��
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

//��ʾ��ǰ״̬��ȱҳ���
void displayInfo(void)
{
    int i,j;
    if(vpage==0){
        printf("\n=============ҳ���������=============\n");
        for(i=0;i<pageInfo.pn;i++){
            printf("%4d",pageInfo.List[i]);
		    if((i+1)%10==0)
                printf("\n");   //ÿ����ʾ10��
        }
        printf("\n======================================\n");
    }
    printf("����%3d : �ڴ�<",pageInfo.List[vpage]);
    for(j=0;j<pageInfo.pf;j++){     // ҳ����Ϣ
        if(pageframe[j]>=0)
            printf("%3d",pageframe[j]);
        else
            printf("   ");
    }
    printf(" >");
    if(pageInfo.flag==0){     //ȱҳ��־��0Ϊȱҳ��1Ϊ��ȱҳ
        printf(" ==>ȱҳ����:%d",pageInfo.dispage_num);
    }
    printf("\n");
}

//����ҳ���Ƿ���ڣ�1Ϊ���ڣ�0Ϊ�����ڼ�ȱҳ
int sreachPage(int page)
{
    int i;
    for(i=0;i<pageInfo.pf;i++){
        pagehistory[i]++;   // ������ʷ��1
    }
	for(i=0;i<pageInfo.pf;i++){
        if(pageframe[i]==page){
            pageInfo.flag=1;    //0Ϊȱҳ��1Ϊ��ȱҳ
		    pagehistory[i]=0;    //�÷�����ʷΪ0
		    return 1;
        }
	}
	pageInfo.flag=0; //0Ϊȱҳ��1Ϊ��ȱҳ
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
        printf ("��װ���ڴ�\n");
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
                lost++;//ȱҳ
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
    printf("\n=============ҳ���������=============\n");
    for(i=0;i<pageInfo.pn;i++){
        printf("%4d",access[i]);
        if((i+1)%10==0)
            printf("\n");   //ÿ����ʾ10��
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
        printf(">  ==>ȱҳlost=%d\n",lost);
    }
    finish=clock();
    e1=(float)(lost)*100.0/pageInfo.pn;
    printf("ȱҳ��%3.1f\n",e1);
    e2=double(finish - start)/CLOCKS_PER_SEC;
    cout <<"�㷨����ʱ��:"<<setprecision(4)<<e2<< endl;
}
