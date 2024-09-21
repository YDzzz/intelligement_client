//
// Created by YDzzz on 2024/6/20.
//
#include <linux/input.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include "Tools.h"

#define LCD_INIT_COLOR 0x0000ff00
#define COM2 "/dev/ttySAC1"
#define COM3 "/dev/ttySAC2"

struct Event_Range{
    int x_start;
    int x_end;
    int y_start;
    int y_end;
    void (*fun)();
};

struct Event_Range events_list[100];
int events_list_count = 0;

int screen_fd; //屏幕文件标识符
int *plcd;
static  char *screenPath = "/dev/fb0";

void draw_pixel(int i,int j,int color)//画点
{
    if(i>0&&i<480&&j>0&&j<800)
        *(plcd+ i*800+j)=color;  //将屏幕上任意一个点显示颜色
}

void lcd_init()//初始化屏幕
{
    screen_fd = open(screenPath, O_RDWR);//打开屏幕文件
    plcd = mmap(NULL, 800*480*4, PROT_READ | PROT_WRITE, MAP_SHARED, screen_fd, 0);//内存映射
    for (int i = 0; i < 480; i++)
        for (int j = 0; j < 800; j++)
            *(plcd+ i*800+j)= LCD_INIT_COLOR;  //将屏幕上任意一个点显示颜色
}
void lcd_close()//解映射和关闭屏幕
{
    munmap(plcd,800*480*4);//解映射
    close(screen_fd);//关闭文件
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
            color = buf[2 + 3*n]<<16 | buf[1+3*n]<<8 | buf[0+3*n];
            n++;
            draw_pixel(h-i+y0,j+x0,color);
        }
}

void bmp2(int x0,int y0,char *bmpfile)
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
            if(color<=0xc8c8c8)
                draw_pixel(h-i+y0,j+x0,color);
            else
                draw_pixel(h-i+y0,j+x0,0x0000ff00);
        }
}

void bmp_button(int x0,int y0,int *w, int *h, char *bmpfile)
{
    int fd_bmp;
    fd_bmp = open(bmpfile,O_RDONLY);

    lseek(fd_bmp,0x12,SEEK_SET);//读取图片的宽度
    read(fd_bmp,w,4);

    lseek(fd_bmp,0x16,SEEK_SET);//读取图片的高度
    read(fd_bmp,h,4);

    lseek(fd_bmp,54,SEEK_SET);//读取像素数组
    char buf[*w * *h*3];   //保存所有像素点
    read(fd_bmp,buf,*w * *h*3);
    int i,j;
    int color;
    int n=0;
    for(i=0;i<*h;i++)
        for(j=0;j<*w;j++)
        {
            //color = R<<16  |   G<<8   |   B
            color = buf[2 + 3*n]<<16 | buf[1+3*n]<<8 | buf[0+3*n];
            n++;
            if(color<=0xc8c8c8)
                draw_pixel(*h-i+y0,j+x0,color);
        }
}

int touch_fd;

void touch_init(){
    touch_fd = open("/dev/input/event0",O_RDONLY);//打开触摸屏
    if(touch_fd == -1){
        perror("error opening /dev/input/event0");
        exit(-1);
    }
}

void touch_close(){
    close(touch_fd);
}

void get_touch_test(int x, int y){
    for(int i = 0; i < events_list_count; i++){
        if (x>events_list[i].x_start && x<events_list[i].x_end && y>events_list[i].y_start && y<events_list[i].y_end){
            events_list[i].fun();
            break;
        }
    }
}

//*x,*y:用来保存获取到的x,y轴坐标
void get_touch()
{
    int x, y;
    struct input_event ev;  //保存输入事件结构体
    while(1)
    {
        read(touch_fd,&ev,sizeof(ev));//读取输入事件

        if(ev.type==EV_ABS&&ev.code==ABS_X)//获取触摸屏X轴坐标
        {
            x = ev.value*(1.0*800/1040);
        } else if(ev.type==EV_ABS&&ev.code==ABS_Y)//获取触摸屏Y轴坐标
        {
            y = ev.value*(1.0*480/600);
        } else if(ev.type==EV_KEY&&ev.code==BTN_TOUCH&&ev.value==0)
        {
            for(int i = events_list_count - 1; i >= 0; i--){
                if (x>events_list[i].x_start && x<events_list[i].x_end && y>events_list[i].y_start && y<events_list[i].y_end){
                    events_list[i].fun();
                    break;
                }
            }
            break;//只获取一次坐标
        }
    }
    printf("%d, %d",x,y);
}


void button(int x, int y, char *file_path, void (*fun)()){
    if(events_list_count == 100)
        exit(-1);
    int w = 50,h = 50;
    bmp_button(x, y, &w, &h, file_path);
    events_list[events_list_count].x_start = x;
    events_list[events_list_count].x_end = x+w;
    events_list[events_list_count].y_start = y;
    events_list[events_list_count].y_end = y+h;
    events_list[events_list_count].fun = fun;
    events_list_count++;
}


char digit_buf[][16/8*21] = {
        /*--  文字:  0  --*/
/*--  宋体16;  此字体下对应的点阵为：宽x高=11x21   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=16x21  --*/
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0x00,0x3B,0x80,0x71,0xC0,0x71,0xC0,
                0x71,0xC0,0xE1,0xE0,0xE0,0xE0,0xE0,0xE0,0xE1,0xE0,0x71,0xC0,0x71,0xC0,0x71,0xC0,
                0x3B,0x80,0x1F,0x00,0x00,0x00,0x00,0x00,0x00,0x00},

/*--  文字:  1  --*/
/*--  宋体16;  此字体下对应的点阵为：宽x高=11x21   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=16x21  --*/
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x00,0x3E,0x00,0x1E,0x00,0x0E,0x00,
                0x0E,0x00,0x0E,0x00,0x0E,0x00,0x0E,0x00,0x0E,0x00,0x0E,0x00,0x0E,0x00,0x0E,0x00,
                0x0E,0x00,0x3F,0x80,0x00,0x00,0x00,0x00,0x00,0x00},

/*--  文字:  2  --*/
/*--  宋体16;  此字体下对应的点阵为：宽x高=11x21   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=16x21  --*/
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0x80,0x73,0xC0,0x61,0xC0,0x71,0xC0,
                0x71,0xC0,0x01,0xC0,0x03,0x80,0x07,0x00,0x0E,0x00,0x1C,0x00,0x38,0x40,0x70,0xC0,
                0x7F,0xC0,0xFF,0xC0,0x00,0x00,0x00,0x00,0x00,0x00},

/*--  文字:  3  --*/
/*--  宋体16;  此字体下对应的点阵为：宽x高=11x21   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=16x21  --*/
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0x00,0x73,0x80,0x71,0xC0,0x71,0xC0,
                0x01,0x80,0x07,0x80,0x0F,0x00,0x03,0x80,0x01,0xC0,0x01,0xC0,0x71,0xC0,0x71,0xC0,
                0x73,0x80,0x3F,0x80,0x00,0x00,0x00,0x00,0x00,0x00},

/*--  文字:  4  --*/
/*--  宋体16;  此字体下对应的点阵为：宽x高=11x21   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=16x21  --*/
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x80,0x07,0x80,0x07,0x80,0x0F,0x80,
                0x1F,0x80,0x3B,0x80,0x33,0x80,0x63,0x80,0xE3,0x80,0xFF,0xE0,0x03,0x80,0x03,0x80,
                0x03,0x80,0x0F,0xE0,0x00,0x00,0x00,0x00,0x00,0x00},

/*--  文字:  5  --*/
/*--  宋体16;  此字体下对应的点阵为：宽x高=11x21   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=16x21  --*/
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0xC0,0x3F,0x80,0x70,0x00,0x70,0x00,
                0x60,0x00,0x7F,0x80,0x7B,0xC0,0x61,0xC0,0x01,0xC0,0x00,0xC0,0x71,0xC0,0x71,0xC0,
                0x73,0x80,0x3F,0x80,0x00,0x00,0x00,0x00,0x00,0x00},

/*--  文字:  6  --*/
/*--  宋体16;  此字体下对应的点阵为：宽x高=11x21   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=16x21  --*/
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0x80,0x3D,0xC0,0x71,0xC0,0x70,0x00,
                0x70,0x00,0xFF,0x80,0xFB,0xC0,0xF1,0xC0,0xE0,0xE0,0xE0,0xE0,0x70,0xE0,0x71,0xC0,
                0x3B,0xC0,0x1F,0x80,0x00,0x00,0x00,0x00,0x00,0x00},

/*--  文字:  7  --*/
/*--  宋体16;  此字体下对应的点阵为：宽x高=11x21   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=16x21  --*/
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0xE0,0x7F,0xC0,0x61,0x80,0x41,0x80,
                0x03,0x00,0x07,0x00,0x06,0x00,0x0E,0x00,0x0E,0x00,0x0C,0x00,0x1C,0x00,0x1C,0x00,
                0x1C,0x00,0x1C,0x00,0x00,0x00,0x00,0x00,0x00,0x00},

/*--  文字:  8  --*/
/*--  宋体16;  此字体下对应的点阵为：宽x高=11x21   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=16x21  --*/
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0x80,0x7B,0xC0,0x61,0xC0,0x60,0xC0,
                0x71,0xC0,0x79,0x80,0x1F,0x00,0x37,0x80,0x63,0xC0,0xE1,0xC0,0xE0,0xC0,0xE1,0xC0,
                0x7B,0xC0,0x3F,0x80,0x00,0x00,0x00,0x00,0x00,0x00},

/*--  文字:  9  --*/
/*--  宋体16;  此字体下对应的点阵为：宽x高=11x21   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=16x21  --*/
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0x00,0x7B,0x80,0x71,0xC0,0xE1,0xC0,
                0xE1,0xC0,0xE1,0xE0,0xE1,0xE0,0x7B,0xC0,0x3F,0xC0,0x01,0xC0,0x01,0xC0,0x73,0x80,
                0x77,0x80,0x3F,0x00,0x00,0x00,0x00,0x00,0x00,0x00}
};

void word(int x,int y,int w,int h,int color,char *word_buf)
{
    for(int i=0;i<w/8*h;i++)//遍历数组的所有字节
    {
        for(int j=7;j>=0;j--)//从高位到低位遍历判断
        {
            if(word_buf[i] >> j & 1) //代表当前位为1，有像素点
            {
                int x0 = x + i%(w/8)*8+(7-j);//开始画点的宽度坐标
                int y0 = y + i/(w/8);
                draw_pixel(y0,x0,color);
            } else{
                int x0 = x + i%(w/8)*8+(7-j);//开始画点的宽度坐标
                int y0 = y + i/(w/8);
                draw_pixel(y0,x0,LCD_INIT_COLOR);
            }
        }
    }
}

void digit(int value,int x0, int y0, int color) //在屏幕上显示数字
{
    //判断是几位数
    int q,b,s,g;
    if (value / 1000)
    {
        q = value/1000;
        word(x0,y0,16,21,color,digit_buf[q]);
        //lcd_draw_num(q,32,100,100,RED);
        b = value/100%10;
        word(x0+20,y0,16,21,color,digit_buf[b]);
        s = value/10%10;
        word(x0+40,y0,16,21,color,digit_buf[s]);
        g = value%10;
        word(x0+60,y0,16,21,color,digit_buf[g]);
    }
    else if (value / 100)
    {
        b = value/100;
        word(x0+20,y0,16,21,color,digit_buf[b]);
        s = value/10%10;
        word(x0+40,y0,16,21,color,digit_buf[s]);
        g = value%10;
        word(x0+60,y0,16,21,color,digit_buf[g]);
    }
    else if (value / 10)
    {
        s = value/10;
        word(x0+40,y0,16,21,color,digit_buf[s]);
        g = value%10;
        word(x0+60,y0,16,21,color,digit_buf[g]);
    }
    else
    {
        word(x0+60,y0,16,21,color,digit_buf[value]);
    }
}

int init_serial(const char *file, int baudrate)
{
    int fd;

    fd = open(file, O_RDWR);
    if (fd == -1)
    {
        perror("open device error:");
        return -1;
    }

    struct termios myserial;
    //清空结构体
    memset(&myserial, 0, sizeof (myserial));
    //O_RDWR
    myserial.c_cflag |= (CLOCAL | CREAD);
    //设置控制模式状态，本地连接，接受使能
    //设置 数据位
    myserial.c_cflag &= ~CSIZE;   //清空数据位
    myserial.c_cflag &= ~CRTSCTS; //无硬件流控制
    myserial.c_cflag |= CS8;      //数据位:8

    myserial.c_cflag &= ~CSTOPB;//   //1位停止位
    myserial.c_cflag &= ~PARENB;  //不要校验
    //myserial.c_iflag |= IGNPAR;   //不要校验
    //myserial.c_oflag = 0;  //输入模式
    //myserial.c_lflag = 0;  //不激活终端模式

    switch (baudrate)
    {
        case 9600:
            cfsetospeed(&myserial, B9600);  //设置波特率
            cfsetispeed(&myserial, B9600);
            break;
        case 115200:
            cfsetospeed(&myserial, B115200);  //设置波特率
            cfsetispeed(&myserial, B115200);
            break;
        case 19200:
            cfsetospeed(&myserial, B19200);  //设置波特率
            cfsetispeed(&myserial, B19200);
            break;
    }

    /* 刷新输出队列,清除正接受的数据 */
    tcflush(fd, TCIFLUSH);

    /* 改变配置 */
    tcsetattr(fd, TCSANOW, &myserial);

    return fd;
}

int detect_pthread_flag = 0;

void detect_pthread_exit(){
    detect_pthread_flag = 1;
}

void detect_env()  //gy39
{
    int fd;
    fd = init_serial(COM2, 9600);

    while (1)
    {
        if (detect_pthread_flag)
            pthread_exit(NULL);
        char cmd[] = {0xA5, 0x83, 0x28};
        write(fd, cmd, sizeof(cmd));

        char rcv[24];
        int  ret;
        ret = read(fd, rcv, sizeof(rcv));
        if ((ret != 24) && (rcv[0]!=0x5A) && (rcv[1]!=0x5A))
        {
            usleep(200000);
            continue;
        }
#define X0 400

        int lux;//  光照
        lux = rcv[4]<<24|rcv[5]<<16|rcv[6]<<8|rcv[7];
        //
        digit(lux,X0,90,0x00ffffff);

        int web = ((rcv[13]<<8) | rcv[14])/100;//温度
        //printf("wendu: %d\n", web);
        digit(web,X0, 190, 0x00ffffff);

        int shidu = ((rcv[19]<<8)|rcv[20])/100;//湿度
        //printf("shidu: %d\n", shidu);
        digit(shidu, X0, 290, 0x00ffffff);

        int qiya = ((rcv[15]<<14)|(rcv[16]<<16)|(rcv[17]<<8)|rcv[18])/10000;
        //
        digit(qiya,X0, 390, 0x00ffffff);//气压

        usleep(200000);
    }
}

void beep_ctrl(int fd, int cmd)//控制蜂鸣器：cmd: 1 为 开启  0 为关闭
{
    write(fd, &cmd,1);
}

void led_ctrl(int fd, int cmd)//控制LED灯：1为亮，0为灭
{
    write(fd, &cmd,1);
}

