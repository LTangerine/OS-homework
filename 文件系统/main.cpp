#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iomanip>
#include "Define.h"
using namespace std;

int main()
{
    cout<<"欢迎使用文件系统"<<endl;
    showHelp();//显示帮助
    Init();
    //系统在打开一个文件的时候，会先默认打开三个文件标准输入，标准输出，标准错误，它们三个分别占据了这个文件描述符数组的前三个
    create("stdin");
    create("stdout");
    create("stderr");
    char ch[CH_LENGTH];
    Init_block(ch,CH_LENGTH);
    cout<<"请输入指令"<<endl;
    while(cin>>ch){
        int cmd;
        char filename[FILE_NAME_LENGTH];
        cmd=-1;
        Init_block(filename,FILE_NAME_LENGTH);
        if(strncmp("menu",ch,4)==0){
            cmd=MENU;
        }
        if(strncmp("create",ch,6)==0){
            cmd=CREATE;
            strcat(filename,ch+7);
        }
        if(strncmp("delete",ch,6)==0){
            cmd=DELETE;
            strcat(filename,ch+7);
        }
        if(strncmp("open",ch,4)==0){
            cmd=OPEN;
            strcat(filename,ch+5);
        }
        if(strncmp("close",ch,5)==0){
            cmd=CLOSE;
            strcat(filename,ch+6);
        }
        if(strncmp("read",ch,4)==0){
            cmd=READ;
            strcat(filename,ch+5);
        }
        if(strncmp("write",ch,5)==0){
            cmd=WRITE;
            strcat(filename,ch+6);
        }
        if(strncmp("help",ch,4)==0){
            cmd=HELP;
        }
        if(strncmp("exit",ch,4)==0){
            cmd=EXIT;
            break;
        }
        int index,count,pos;
        switch(cmd){
            case MENU:
                directory();
                cout<<endl;
                break;
            case CREATE:
                if(create(filename)==1){
                    cout<<"创建文件成功"<<endl;
                    directory();
                }
                cout<<endl;
                break;
            case DELETE:
                if(destroy(filename)==1){
                    cout<<"删除文件成功"<<endl;
                    directory();
                }
                cout<<endl;
                break;
            case OPEN:
                if(open(filename)==1){
                    cout<<"打开文件成功"<<endl;
                    show_openlist();
                }
                cout<<endl;
                break;
            case CLOSE:
                if(close(filename)==1){
                    cout<<"关闭文件成功"<<endl;
                    show_openlist();
                }
                cout<<endl;
                break;
            case READ:
                cout<<"请输入读取文件的开始位置"<<endl;
                cin>>pos;
                cout<<"请输入读取文件长度"<<endl;
                cin>>count;
                if(read(filename,pos,count)==1)
                    cout<<"读取文件成功"<<endl;
                cout<<endl;
                break;
            case WRITE:
                cout<<"请输入写入内存的开始位置,首次写入时为0"<<endl;
                cin>>pos;
                cout<<"请输入写入文件长度"<<endl;
                cin>>count;
                if(write(filename,pos,count)==1){
                    cout<<"写入文件成功"<<endl;
                    show_openlist();
                }
                cout<<endl;
                break;
            case HELP:
                showHelp();
                break;
            default:
                cout<<"指令错误"<<endl;
                cout<<endl;
                break;
        }
        Init_block(ch,CH_LENGTH);
        cout<<"请输入指令"<<endl;
    }
    return 0;
}

//初始化系统数据
void Init(){
    int i;
    char temp[B];
    for(i=0;i<L;i++){//初始化磁盘
        Init_block(temp,B);
        write_block(i,temp);
    }
    for(i=K;i<L;i++){//初始化位图
        read_block((i-K)/B,temp);
        temp[(i-K)%B]=FREE;
        write_block((i-K)%B,temp);
    }
    //初始化目录文件
    filesign temp_cnt_sign;
    temp_cnt_sign.filesign_flag=1;
    temp_cnt_sign.file_length=0;
    temp_cnt_sign.file_block=FILE_BLOCK_LENGTH;
    Init_block(temp,B);
    temp[0]=temp_cnt_sign.filesign_flag;
    temp[1]=temp_cnt_sign.file_length;
    temp[2]=temp_cnt_sign.file_block;
    for(i=0;i<FILE_BLOCK_LENGTH;i++){
        temp[i+3]=K+i; //默认数据区的前FILE_BLOCK_LENGTH个被目录占用
    }
    write_block(FILE_SIGN_AREA,temp);
    read_block(0,temp);
    for(i=0;i<FILE_NUM;i++)
        temp[i]=FREE;
    write_block(0,temp);
}
//初始化字符数组
void Init_block(char *temp,int length){
    int i;
    for(i=0;i<length;i++)
        temp[i]='\0';
}
//该函数把逻辑块i的内容读入到指针p指向的内存位置，拷贝的字符个数为存储块的长度B
void read_block(int i,char *p){
    char *temp=(char *)malloc(sizeof(char));
    temp=p;
    int z,c,s;
    z=i/(H*S);//柱面号=块号/（磁头数*扇区数）
    c=(i%(H*S))/S;//磁头号=（块号%（磁头数*扇区数）)/扇区数
    s=(i%(H*S))%S;//扇区号=（块号%（磁头数*扇区数）)%扇区数
    for(int a=0;a<B;){
        *temp=ldisk[z][c][s];
        *temp=disk[i][a];
        a++;
        temp++;
    }
}
//该函数把指针p指向的内容写入逻辑块i，拷贝的字符个数为存储块的长度B
void write_block(int i,char *p){
    char *temp=(char *)malloc(sizeof(char));
    temp=p;
    int z,c,s;
    z=i/(H*S);//柱面号=块号/（磁头数*扇区数）
    c=(i%(H*S))/S;//磁头号=（块号%（磁头数*扇区数）)/扇区数
    s=(i%(H*S))%S;//扇区号=（块号%（磁头数*扇区数）)%扇区数
    for(int a=0;a<B;){
        disk[i][a]=*temp;
        ldisk[z][c][s]=*temp;
        a++;
        temp++;
    }
}
// 根据指定的文件名创建新文件
int create(char *filename){
    int i,frees;
    int freed;
    int freed2;
    char temps[B],tempc[B],temp[B];
    for(i=K;i<K+FILE_NUM;i++){//查看文件名是否存在
        read_block((i-K)/B,temp);
        if(temp[(i-K)%B] == BUSY){
            read_block(i,temp);
            if(strncmp(temp+1,filename,FILE_NAME_LENGTH)==0){
                cout<<"该目录已经存在文件名为"<<filename<<"的文件"<<endl;
                return 0;
            }
        }
    }
    for(i=FILE_SIGN_AREA;i<K;i++){//寻找空闲文件描述符
        read_block(i,temp);
        if(temp[0]==FREE){
            frees=i;
            break;
        }
    }
    if(i==K){
        cout<<"没有空闲文件描述符"<<endl;
        return 0;
    }
    for(i=K;i<K+FILE_NUM;i++){//寻找数据区的目录描述符所指定的空闲存储块
        read_block((i-K)/B,temp);
        if(temp[(i-K)%B]==FREE){
            freed=i;
            break;
        }
    }
    if(i==K+FILE_NUM){
        cout<<"文件个数已达上限"<<endl;
        return 0;
    }
    for(i=K+FILE_NUM;i<L;i++){//寻找文件区的空闲块分配给新创建的文件
        read_block((i-K)/B,temp);
        if(temp[(i-K)%B]==FREE){
            freed2=i;
            break;
        }
    }
    if(i==L){
        cout<<"磁盘已满,分配失败"<<endl;
        return 0;
    }
    filesign temp_filesign;//创建临时文件描述符
    contents temp_contents;//创建临时目录项
    temp_filesign.filesign_flag=1;//构建文件描述符
    temp_filesign.file_length=0;
    temp_filesign.file_block=1;
    Init_block(temps,B);
    temps[0]=temp_filesign.filesign_flag;
    temps[1]=temp_filesign.file_length;
    temps[2]=temp_filesign.file_block;
    temps[3]=freed2;
    for(i=4;i<FILE_BLOCK_LENGTH;i++){
        temps[i]='\0';
    }
    write_block(frees,temps);//写入磁盘
    //构建目录项插入目录文件描述符所指定的数组块
    temp_contents.filesignnum=frees-FILE_SIGN_AREA;
    strncpy(temp_contents.filename,filename,FILE_NAME_LENGTH);
    Init_block(tempc,B);
    tempc[0]=temp_contents.filesignnum;
    tempc[1]='\0';
    strcat(tempc,temp_contents.filename);
    write_block(freed,tempc);
    //更改位图状态
    read_block((freed-K)/B,temp);
    temp[(freed-K)%B]=BUSY;
    write_block((freed-K)/B,temp);
    read_block((freed2-K)/B,temp);
    temp[(freed2-K)%B]=BUSY;
    write_block((freed2-K)/B,temp);
    //增加目录文件描述符的长度
    read_block(FILE_SIGN_AREA,temp);
    temp[1]++;
    write_block(FILE_SIGN_AREA,temp);
    return 1;
}
//删除指定文件
int destroy(char *filename){
    int i,dtys,dtyd,use_block;
    int index;
    char temp[B];
    char tempd[B];
    //寻找目录项和文件描述符
    for(i=K;i<K+FILE_NUM;i++){
        read_block((i-K)/B,temp);
        if(temp[(i-K)%B]==BUSY){
            read_block(i,temp);
            if(strncmp(temp+1,filename,FILE_NAME_LENGTH)==0){
                dtyd=i;
                dtys=temp[0]+FILE_SIGN_AREA;
                index=temp[0];
                break;
            }
        }
    }
    if(i==K+FILE_NUM){
        cout<<"没有找到该文件"<<endl;
        return 0;
    }
    int list=-1;
    //查看文件是否打开
    for(i=0;i<FILE_NUM;i++){
        if(open_list[i].filesignnum==index){
            list=i;
            break;
        }
    }
    if(open_list[list].flag=BUSY&&list!=-1){
        cout<<"该文件已经被打开,需要关闭才能删除"<<endl;
        return 0;
    }
    read_block(dtys,temp);
    use_block=temp[2];
    for(i=0;i<use_block;i++){
        read_block((temp[i+3]-K)/B,tempd);
        tempd[(temp[i+3]-K)%B]=FREE;
        write_block((temp[i+3]-K)/B,tempd);
    }
    //删除该目录项
    Init_block(temp,B);
    write_block(dtys,temp);
    //删除文件描述符
    Init_block(temp,B);
    write_block(dtyd,temp);
    //更改位图
    read_block((dtyd-K)/B,temp);
    temp[(dtyd-K)%B]=FREE;
    write_block((dtyd-K)/B,temp);
    //目录文件描述长度减1
    read_block(FILE_SIGN_AREA,temp);
    temp[1]--;
    write_block(FILE_SIGN_AREA,temp);
    return 1;
}
//打开文件
int open(char *filename){
    int i,opd,ops;
    int list;
    char temp[B];
    int index;
    for(i=K;i<K+FILE_NUM;i++){//寻找文件目录项和文件描述符
        read_block((i-K)/B,temp);
        if(temp[(i-K)%B]=BUSY){
            read_block(i,temp);
            if(strncmp(temp+1,filename,FILE_NAME_LENGTH)==0){
                opd=i;
                ops=temp[0];//文件描述符
                break;
            }
        }
    }
    if(i==K+FILE_NUM){
        cout<<"没有找到该文件"<<endl;
        return 0;
    }
    for(i=0;i<FILE_NUM;i++){//查看文件是否被打开
        if(open_list[i].filesignnum==ops&&open_list[i].flag==BUSY){
            cout<<"该文件己经被打开"<<endl;
            return 0;
        }
    }
    for(i=0;i<FILE_NUM;i++){//找一块未使用的区域分配
        if(open_list[i].flag!=BUSY){
            list=i;
            break;
        }
    }
    open_list[list].filesignnum=ops;//写入文件描述符
    open_list[list].flag=BUSY;//置标志位为占用
    index=open_list[list].filesignnum;//生成索引号即文件描述符
    lseek(index,0);
    Init_block(open_list[list].buffer,BUFFER_LENGTH);
    read_block(open_list[list].pointer[0],temp);
    strncpy(open_list[list].buffer,temp,BUFFER_LENGTH);
    return 1;
}
//关闭指定文件
int close(char *filename){
    int i,opd,ops;
    char temp[B];
    int index;
    for(i=K;i<K+FILE_NUM;i++){//寻找文件目录项和文件描述符
        read_block((i-K)/B,temp);
        if(temp[(i-K)%B]=BUSY){
            read_block(i,temp);
            if(strncmp(temp+1,filename,FILE_NAME_LENGTH)==0){
                opd=i;
                ops=temp[0];
                break;
            }
        }
    }
    index=ops;//找到要操作文件的文件描述符
    int list=-1;
    for(i=0;i<FILE_NUM;i++){
        if(open_list[i].filesignnum==index){
            list=i;
            break;
        }
    }
    if(list==-1||open_list[list].flag!=BUSY){
        cout<<"该文件没有被打开"<<endl;
        return 0;
    }
    Init_block(open_list[list].buffer,BUFFER_LENGTH);
    open_list[list].filesignnum=FREE;
    open_list[list].flag=FREE;
    open_list[list].pointer[0]=NULL;
    open_list[list].pointer[1]=NULL;
    return 1;
}
//从指定文件中从指定的内存位置(mem_area)顺序读入count个字节
int read(char *filename,int mem_area,int count){
    int i,opd,ops;
    char temp[B];
    int index;
    for(i=K;i<K+FILE_NUM;i++){//寻找文件目录项和文件描述符
        read_block((i-K)/B,temp);
        if(temp[(i-K)%B]=BUSY){
            read_block(i,temp);
            if(strncmp(temp+1,filename,FILE_NAME_LENGTH)==0){
                opd=i;
                ops=temp[0];
                break;
            }
        }
    }
    index=ops;//找到要操作文件的文件描述符
    int list=-1;
    for(i=0;i<FILE_NUM;i++){
        if(open_list[i].filesignnum==index){
            list=i;
            break;
        }
    }
    if(list==-1||open_list[list].flag!=BUSY){
        cout<<"该文件没有被打开"<<endl;
        return 0;
    }
    cout<<"读取内容为：";
    for(i=0;i<count;i++)
        cout<<open_list[list].buffer[i];
    cout<<endl;
    return 1;
}
//从指定的内存位置(mem_area)开始的count个字节顺序写入指定文件
int write(char *filename,int mem_area,int count){
    int i,opd,ops;
    char temp[B];
    int index;
    for(i=K;i<K+FILE_NUM;i++){//寻找文件目录项和文件描述符
        read_block((i-K)/B,temp);
        if(temp[(i-K)%B]=BUSY){
            read_block(i,temp);
            if(strncmp(temp+1,filename,FILE_NAME_LENGTH)==0){
                opd=i;
                ops=temp[0];
                break;
            }
        }
    }
    index=ops;//找到要操作文件的文件描述符
    int list=-1;
    int input_length;
    for(i=0;i<FILE_NUM;i++){
        if(open_list[i].filesignnum==index){
            list=i;
            break;
        }
    }
    if(list==-1||open_list[list].flag!=BUSY){
        cout<<"该文件没有被打开"<<endl;
        return 0;
    }
    char input[INPUT_LENGTH]={0};
    Init_block(input,INPUT_LENGTH);
    cin>>input;
    for(i=0;input[i]!=0;i++);
    input_length=i;
    char temps[B];
    if(count<=BUFFER_LENGTH){
        strncat(open_list[list].buffer,input+mem_area,count);
        open_list[list].length=count+open_list[list].length;
        file_list[index].file_length=open_list[list].length;
    }
    return 1;
}
//把文件的读写指针移动到pos指定的位置
int lseek(int index,int pos){
    int i;
    int list=-1;
    char temp[B];
    int pos_i=pos/B;//计算在所有文件中的第几块
    int pos_j=pos%B;//在第几块中的第几个位置
    for(i=0;i<FILE_NUM;i++){
        if(open_list[i].filesignnum==index){
            list=i;
            break;
        }
    }
    if(list==-1){
        cout<<"没找到当前文件,操作失败"<<endl;
        return 0;
    }
    if(open_list[list].flag!=BUSY){
        cout<<"输入索引号有误，操作失败"<<endl;
        return 0;
    }
    read_block(open_list[list].filesignnum+FILE_SIGN_AREA,temp);
    if(pos_i>temp[2]-1){
        cout<<"异常越界，定位失败"<<endl;
        return 0;
    }
    open_list[list].pointer[0]=temp[3+pos_i];
    open_list[list].pointer[1]=pos_j;
    return 1;
}
//列表显示所有文件及其长度
void directory(){
    int i;
    int filenum;
    int filelength;
    char filename[FILE_NAME_LENGTH];
    char *filename1;
    char temp[B];
    char tempd[B];
    char temps[B];
    read_block(FILE_SIGN_AREA,temp);
    filenum=temp[1];
    if(filenum==0){
       cout<<"该目录没有文件"<<endl;
    }
    printf("文件名\t\t\t大小\n");
    for(i=1;i<FILE_NUM;i++){
        read_block(temp[2+i],tempd);
        if(tempd[0]!=0){
            read_block(tempd[0]+FILE_SIGN_AREA,temps);
            if(temps[0]==BUSY&&tempd[0]!=0){
                filelength=file_list[i].file_length;
                strcpy(filename,tempd+1);
                //cout<<setw(10)<<left<<filename;
                //cout<<setw(8)<<left<<filelength<<"字节"<<endl;;
                printf("%-10s\t\t%-2d字节\n",filename,filelength);
            }
        }
    }
    if(filenum!=0)
        cout<<"共"<<filenum<<"个文件"<<endl;
}
//显示已打开文件
int show_openlist(){
    int i,j;
    int openfile=0;
    char temp[B];
    int index;
    //cout<<"文件名    大小"<<endl;
    printf("文件名\t\t\t大小\n");
    for(i=0;i<FILE_NUM;i++){
        if(open_list[i].flag==BUSY){
            index=open_list[i].filesignnum;
            openfile++;
            read_block(FILE_SIGN_AREA+index,temp);
            for(j=K;j<K+FILE_NUM;j++){
                read_block(j,temp);
                if(temp[0]==index){
                    //cout<<setw(10)<<left<<temp+1<<endl;
                    cout<<temp+1;
                }
            }
            //cout<<setw(10)<<left<<temp[1];
            printf("\t\t\t%d\n",open_list[i].length);
        }
    }
    return openfile;
}
//显示帮助
void showHelp()
{
    cout<<"*********************帮助********************"<<endl;
    cout<<"命令               说明"<<endl;
    cout<<"menu               显示目录"<<endl;
    cout<<"create+filename    新建文件名为filename的文件"<<endl;
    cout<<"delete+filename    删除文件名为filename的文件"<<endl;
    cout<<"open+filename      打开文件名为filename的文件"<<endl;
    cout<<"close+filename     关闭文件名为filename的文件"<<endl;
    cout<<"read+filename      读取文件名为filename的文件"<<endl;
    cout<<"write+filename     写入文件名为filename的文件"<<endl;
    cout<<"help               帮助"<<endl;
    cout<<"exit               退出"<<endl;
    cout<<endl;
}
