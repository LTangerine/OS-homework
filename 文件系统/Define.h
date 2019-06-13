#ifndef DEFINE_H_INCLUDED
#define DEFINE_H_INCLUDED

#define B 10
#define C 10   //柱面数
#define H 8   //磁头数
#define S 8   //扇区数
#define L 712 //磁盘的存储块总数  块号=柱面号*（磁头数*扇区数）+磁头号*扇区数+扇区号
#define K 100
#define BUSY 1
#define FREE 0
#define FILE_BLOCK_LENGTH (B-3)
#define FILE_NAME_LENGTH (B-1)
#define FILE_SIGN_AREA ((L-1-K)/B+1)
#define FILE_NUM FILE_BLOCK_LENGTH
#define INPUT_LENGTH 100
#define OUTPUT_LENGTH 100
#define BUFFER_LENGTH 25
#define MENU 0
#define CREATE 1
#define DELETE 2
#define OPEN 3
#define CLOSE 4
#define READ 5
#define WRITE 6
#define HELP 7
#define EXIT 8
#define OPLIST 9
#define CH_LENGTH 20

struct filesign{//文件描述符
    int file_length;//文件长度
    int filesign_flag;//占用标识位
    int file_block;//文件分配的磁盘块号
    int file_block_ary[FILE_BLOCK_LENGTH];//文件分配的磁盘块号数组
};

struct contents{//目录
    char filename[FILE_NAME_LENGTH];//文件名
    int file_length;//文件大小
    int filesignnum;//文件描述符序号
};

struct openfilelist{//已打开的文件
    char buffer[BUFFER_LENGTH];//读写缓冲区,存读写内容
    int pointer[2];//读写指针
    int filesignnum;//文件描述符
    int flag;//占用符
    int length;//占用缓冲区大小
};


char ldisk[C][H][S];//磁盘模型，其中CHS 分别表示柱面号，磁头号和扇区号
//z=BlockNum/(H*S);//柱面号=块号/（磁头数*扇区数）
//c=(BlockNum%(H*S))/S;//磁头号=（块号%（磁头数*扇区数）)/扇区数
//s=(BlockNum%(H*S))%S;//扇区号=（块号%（磁头数*扇区数）)%扇区数

char disk[L][B];
openfilelist open_list[FILE_NUM];//已打开文件集合
contents file_list[FILE_NUM];//所有文件集合

void Init();//初始化系统数据
void Init_block(char *temp,int length);//初始化字符数组
void read_block(int i,char *p);//该函数把逻辑块i的内容读入到指针p指向的内存位置，拷贝的字符个数为存储块的长度B
void write_block(int i,char *p);//该函数把指针p指向的内容写入逻辑块i，拷贝的字符个数为存储块的长度B
int create(char *filename);//根据指定的文件名创建新文件
int destroy(char *filename);//删除指定文件
int open(char *filename);//打开文件
int close(char *filename);//关闭指定文件
int read(char *filename,int mem_area,int count);//从指定文件中从指定的内存位置(mem_area)顺序读入count个字节
int write(char *filename,int mem_area,int count);// 从指定的内存位置(mem_area)开始的count个字节顺序写入指定文件
int lseek(int index,int pos);//把文件的读写指针移动到pos指定的位置
void directory();//列表显示所有文件及其长度
int show_openlist();//显示已打开文件
void showHelp();//显示帮助


#endif // DEFINE_H_INCLUDED
