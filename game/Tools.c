//
// Created by YDzzz on 2024/6/20.
//
#include <linux/input.h>
#include "Tools.h"
int fd; //屏幕文件标识符
int *plcd;
static  char *screenPath = "/dev/fb0";

void draw_pixel(int i,int j,int color)//画点
{
    if(i>0&&i<480&&j>0&&j<800)
        *(plcd+ i*800+j)=color;  //将屏幕上任意一个点显示颜色
}

void lcd_init()//初始化屏幕
{
    fd = open(screenPath,O_RDWR);//打开屏幕文件
    plcd = mmap(NULL, 800*480*4, PROT_READ | PROT_WRITE, MAP_SHARED,fd, 0);//内存映射
    for (int i = 0; i < 480; i++)
        for (int j = 0; j < 800; j++)
            *(plcd+ i*800+j)= 0x00ff00ff;  //将屏幕上任意一个点显示颜色
}
void lcd_close()//解映射和关闭屏幕
{
    munmap(plcd,800*480*4);//解映射
    close(fd);//关闭文件
}
void bmp2(int x0,int y0,char *bmpfile)
{
    int fd_bmp;
    fd_bmp = open(bmpfile,O_RDONLY);

    lseek(fd_bmp,0x12,SEEK_SET);//读取图片的宽度
    int w;
    read(fd_bmp,&w,4);
    printf("%d\n",w);

    lseek(fd_bmp,0x16,SEEK_SET);//读取图片的高度
    int h;
    read(fd_bmp,&h,4);
    printf("%d\n",h);

    lseek(fd_bmp,54,SEEK_SET);//读取像素数组
    char buf[w*h*3];   //保存所有像素点
    read(fd_bmp,buf,w*h*3);
    close(fd_bmp);//关闭图片
    int i,j;
    int color;
    int n=0;
    for(i=0;i<h;i++)  //向上逐渐刷屏
    {
        for(j=0;j<w;j++)
        {
            //color = R<<16  |   G<<8   |   B
            color = buf[2 + 3*n]<<16 | buf[1+3*n]<<8 | buf[0+3*n];
            n++;
            if(color<=0xc8c8c8)
            draw_pixel(h-i+y0,j+x0,color);
        }
        usleep(500);    //延时
    }
}
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
                color = buf[2 + 3 * n] << 16 | buf[1 + 3 * n] << 8 | buf[0 + 3 * n];
                n++;
                draw_pixel(h - i + y0, j + x0, color);
            }
   
}
int w;
int h;
void bmp_animation(char *bmp_path,void *animation())
{

    int m=0,n=0;
    int fd_bmp;

    fd_bmp = open(bmp_path, O_RDONLY);//打开BMP格式图片

    lseek(fd_bmp,0x12,SEEK_SET);//读取图片的宽度
    read(fd_bmp,&w,4);
    printf("%d\n",w);

    lseek(fd_bmp,0x16,SEEK_SET);//读取图片的高度
    read(fd_bmp,&h,4);
    printf("%d\n",h);

    char buf[h*w*3];   //保存像素数组
    int color_buf[h*w];	   //保存所有像素点

    lseek(fd_bmp, 54 , SEEK_SET);//跳过bmp图片的前54个位置
    read(fd_bmp , buf , sizeof(buf)); //取读图片像素
    close(fd_bmp);//关闭图片
    for(m=0;m<w*h;m++)//保存所有像素点颜色
    {
        //color = R<<16  |   G<<8   |   B
        color_buf[m] = buf[2 + 3*n]<<16 | buf[1+3*n]<<8 | buf[0+3*n];
        n++;
    }
    (*animation)();
}

void cricle_animation1(){
    int color_buf[h*w];	   //保存所有像素点
    for(int k=0; k<467; k+=3)//动画“圆形扩散”效果
    {
        for(int i=0; i<h; i++)
        {
            for(int j=0; j<w; j++)
            {
                if((j-400)*(j-400)+(i-240)*(i-240) <= k*k)
                {
                    draw_pixel(h-i,j,color_buf[800*i+j]);//圆内图片像素点上色
                }
            }
        }
    }
}

void cricle_animation2(){
    int color_buf[h*w];	   //保存所有像素点
    for(int k=468; k>=0; k-=3)//动画“圆形收缩”效果
    {
        for(int i=0; i<480; i++)
        {
            for(int j=0; j<800; j++)
            {
                if((j-400)*(j-400)+(i-240)*(i-240) >= k*k)
                    draw_pixel(h-i,j,color_buf[800*i+j]);
            }
        }
    }
}


//*x,*y:用来保存获取到的x,y轴坐标
void get_touch(int *x,int *y)
{
    int fd = open("/dev/input/event0",O_RDONLY);//打开触摸屏
    struct input_event ev;  //保存输入事件结构体
    while(1)
    {
        read(fd,&ev,sizeof(ev));//读取输入事件

        if(ev.type==EV_ABS&&ev.code==ABS_X)//获取触摸屏X轴坐标
        {
            *x = ev.value*(1.0*800/1040);
        }
        if(ev.type==EV_ABS&&ev.code==ABS_Y)//获取触摸屏Y轴坐标
        {
            *y = ev.value*(1.0*480/600);
        }
        if(ev.type==EV_KEY&&ev.code==BTN_TOUCH&&ev.value==0)
        {
            break;//只获取一次坐标
        }
    }
    printf("%d, %d",*x,*y);
    close(fd);
}