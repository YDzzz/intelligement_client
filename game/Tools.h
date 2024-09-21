//
// Created by YDzzz on 2024/6/20.
//

#ifndef DEMO_TOOLS_H
#define DEMO_TOOLS_H
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/mman.h>



/*打开lcd设备*/
void lcd_init();

/*关闭lcd设备*/
void lcd_close();

/* 在（x，y）处显示bmp_path路径下的图片*/
void bmp(int x0,int y0,char *bmp_path);

/*bmp相册图片动画显示，需要传入渲染函数*/
void bmp_animation(char *bmp_path,void *animation());

/*预设渲染函数，从中心扩大的圆*/
void cricle_animation1();

/*预设渲染函数，从中心收缩的圆*/
void cricle_animation2();

void bmp2(int x0,int y0,char *bmpfile);

//*x,*y:用来保存获取到的x,y轴坐标
void get_touch(int *x,int *y);


#endif //DEMO_TOOLS_H

