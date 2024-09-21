#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/mman.h>
#include <linux/input.h>
#include <stdlib.h>
#include<pthread.h>
char *buf_bmp[]={"./res/0.bmp","./res/1.bmp","./res/2.bmp"};
int fd;
int *plcd;
#define up 1
#define down 2
#define left  3
#define  right 4

int exit_flag = 0;

int get_way()   //获取滑动方向
{
    int fd = open("/dev/input/event0",O_RDONLY);//打开触摸屏文件
    struct input_event ev;  //保存输入事件结构体
    int x0 = -1,y0 = -1;//初始坐标
    int x1,y1;//实时坐标
    while(1)
    {
        read(fd,&ev,sizeof(ev));//读取输入事件
        
        if(ev.type==EV_ABS&&ev.code==ABS_X)//获取触摸屏X轴坐标
        {
            if(x0==-1)
            {
                x0 = ev.value*(1.0*800/1040);//初始x坐标
            }
            x1 = ev.value*(1.0*800/1040);//实时X轴坐标
        }
        if(ev.type==EV_ABS&&ev.code==ABS_Y)//获取触摸屏Y轴坐标
        {
            if(y0==-1)
            {
                y0 = ev.value*(1.0*480/600);
            }
            y1 = ev.value*(1.0*480/600);
        }
        if(ev.type==EV_KEY&&ev.code==BTN_TOUCH&&ev.value==0)
        {
            break;//只获取一次坐标
        }
    }
        printf("start(%d,%d),leave(%d,%d)\n",x0,y0,x1,y1);

        int way;
        if(y1 == y0 && x1 == x0){
            close(fd);
            if (y1 <= 64 && x1 <= 64){
                exit_flag = 1;
            }
            return 5;
        }
        if(((y1-y0)/(x1-x0))*((y1-y0)/(x1-x0))>=1)//上下滑动
        {
            if(y1>y0)
            {
                printf("down\n");
                way = down;
            }

            else
            {
                printf("up\n");
                way = up;
            }
        }
        else  //左右滑动
        {
            if(x1>x0)
            {
                printf("right\n");
                way = right;
            }
            else
            {
                printf("left\n");
                way = left;
            }
        }
    close(fd);

    return way;

}
int wayyy = 0;
void *get_way2(void *)   //获取滑动方向
{
    int fd = open("/dev/input/event0",O_RDONLY);//打开触摸屏文件
    struct input_event ev;  //保存输入事件结构体
    int x0 = -1,y0 = 0;//初始坐标
    int x1,y1;//实时坐标
    while(1){

        read(fd,&ev,sizeof(ev));//读取输入事件
        
        if(ev.type==EV_ABS&&ev.code==ABS_X)//获取触摸屏X轴坐标
        {
            if(x0==-1)
            {
                x0 = ev.value*(1.0*800/1040);//初始x坐标
            }
            x1 = ev.value*(1.0*800/1040);//实时X轴坐标
        }
        if(ev.type==EV_ABS&&ev.code==ABS_Y)//获取触摸屏Y轴坐标
        {
            if(y0==0)
            {
                y0 = ev.value*(1.0*480/600);
            }
            y1 = ev.value*(1.0*480/600);
        }
        printf("start(%d,%d),leave(%d,%d)\n",x0,y0,x1,y1);

        int way = -1;
        if(y1 == y0 && x1 == x0){
            close(fd);
            wayyy = 1; 
            close(fd);
            pthread_exit(NULL);
        }
    }
}

void drawpoint(int i,int j,int color)//画点
{
	if(i>0&&i<480&&j>0&&j<800)
	*(plcd+ i*800+j)=color;  //将屏幕上任意一个点显示颜色
}
void lcdinit()//初始化屏幕
{
    int fd = open("/dev/fb0",O_RDWR);//打开屏幕文件
    plcd = mmap(NULL, 800*480*4, PROT_READ | PROT_WRITE, MAP_SHARED,fd, 0);//内存映射
}
void lcduninit()//解映射和关闭屏幕
{
    munmap(plcd,800*480*4);//解映射
    close(fd);//关闭文件
}



//x0,y0:图片开始显示的起始位置
//bmpfile: 图片的路径
void bmp(int x0,int y0,char *bmpfile)
{
    int fd_bmp;
    fd_bmp = open(bmpfile,O_RDONLY);

    lseek(fd_bmp,0x12,SEEK_SET);//读取图片的宽度
    int w;
    read(fd_bmp,&w,4);

    lseek(fd_bmp,0x16,SEEK_SET);//读取图片的高度
    int h;
    read(fd_bmp,&h,4);

    lseek(fd_bmp,54,SEEK_SET);//读取像素数组
    char buf[w*h*3];   //保存所有像素点
    read(fd_bmp,buf,w*h*3);  
    int i,j;
    int color;
    int n=0;
    for(i=0;i<h;i++)
        for(j=0;j<w;j++)
        {
            //color = R<<16  |   G<<8   |   B  
            color = buf[2 + 3*n]<<16 | buf[1+3*n]<<8 | buf[0+3*n];
            n++;
            drawpoint(h-i-1+y0,j+x0,color);
        }
}



int main()
{
    int x=0,y=0;
    lcdinit();
    int n=0;
    bmp(0,0,buf_bmp[n]);
    while(1)
    {
        int way;
        way = get_way();
        if(way == up||way == left)
        {
            n = (n + 3 + 1) % 3;
        }
        else if(way == down || way == right)
        {
            n = (n + 3 - 1) % 3;
        }else{
            if (exit_flag){
                system("./main");
                exit(0);
            }
            wayyy = 0;
            pthread_t id;
            pthread_create(&id,NULL,get_way2,NULL);
            while(1){
                if(wayyy == 1){
                    break;
                }
                printf("change");
                sleep(1);
                n = (n + 3 + 1) % 3;
                printf("%d\n", n);
                bmp(0,0,buf_bmp[n]);

            }
            continue;
        }
        n%=3;
        bmp(0,0,buf_bmp[n]);
    }
    
    printf("(%d,%d)\n",x,y);
    lcduninit(); 
    return 0;
}
