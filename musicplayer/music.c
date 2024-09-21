
#include <stdlib.h>
#include <pthread.h>
#include "../tools/Tools.h"

//*x,*y:用来保存获取到的x,y轴坐标
int falg = 0;
int current = 0;
char command[1024];
#define MUSIC_NUM 2



void *statement(void *){
    if (falg == 0){
        falg = 1;
        system("killall -9 madplay");
        sprintf(command,"madplay ./res/%d.mp3", current);
        bmp2(350,200,"./res/stop.bmp");
        printf("%s", command);
        system(command);
    }else if(falg == 1){
        falg = 2;
        bmp2(350,200,"./res/start.bmp");
        system("killall -STOP madplay &");
    }else if(falg == 2){
        falg = 1;
        bmp2(350,200,"./res/stop.bmp");
        system("killall -CONT madplay &");
    }
    pthread_exit(NULL);
}

void stop(){
    pthread_t id;
    int ret = pthread_create(&id, NULL, statement, NULL);
}

void previous(){
    falg = 0;
    current = (current + MUSIC_NUM - 1)% MUSIC_NUM;
    system("killall -9 madplay");
    pthread_t id;
    int ret = pthread_create(&id, NULL, statement, NULL);
}

void next(){
    falg = 0;
    current = (current + MUSIC_NUM + 1)% MUSIC_NUM;
    system("killall -9 madplay");
    pthread_t id;
    int ret = pthread_create(&id, NULL, statement, NULL);
}

void myexit(){
    system("killall -9 madplay");
    system("./main");
    exit(0);
}

int main(void) {
    lcd_init();
    touch_init();
    button(350,200,"./res/start.bmp", &stop);
    button(200,200,"./res/previous.bmp", &previous);
    button(500,200,"./res/next.bmp", &next);
    button(0,0, "./res/exit.bmp", &myexit);
    printf("bmp show success");
    while(1){
        get_touch();
    }
    touch_init();
    lcd_close();
}
