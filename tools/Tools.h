//
// Created by YDzzz on 2024/6/20.
//

#ifndef DEMO_TOOLS_H
#define DEMO_TOOLS_H
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/mman.h>
#include <pthread.h>


/*打开lcd设备*/
void lcd_init();

/*关闭lcd设备*/
void lcd_close();

/* 在（x，y）处显示bmp_path路径下的图片*/
void bmp(int x0,int y0,char *bmp_path);

void bmp2(int x0,int y0,char *bmpfile);

//*x,*y:用来保存获取到的x,y轴坐标
void get_touch();

void get_touch_test(int x, int y);

void touch_init();

void touch_close();

void button(int x, int y, char *file_path, void (*fun)());


//在屏幕上显示数字
void digit(int value,int x0, int y0, int color);

//init serial
int init_serial(const char *file, int baudrate);


void detect_env();


void led_ctrl(int fd, int cmd);
//控制LED灯：1为亮，0为灭


void beep_ctrl(int fd, int cmd);
//控制蜂鸣器：cmd: 1 为 开启  0 为关闭

void word(int x,int y,int w,int h,int color,char *word_buf);

void detect_pthread_exit();

#endif //DEMO_TOOLS_H

