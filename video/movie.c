//
// Created by YDzzz on 2024/6/28.
//

#include <stdlib.h>
#include <string.h>
#include "../tools/Tools.h"
#include <pthread.h>


#define CURRENT 0
#define NEXT 1
#define PREVIOUS 2
#define VIDEO_NUM 2
int current = 0;
int v_fd;
int launch_mode = 0;
int launch_flag = 1;
char *buf[]={"0.avi", "1.mp4"};

void *launch(void *){
    launch_flag = 0;
    char command[1024];
    switch (launch_mode) {
        case CURRENT:{
            sprintf(command, "./mplayer -ac mad -slave -quiet -input file=./video_fifo -geometry 0:0 -zoom -x 670 -y 402 ./res/%s", buf[current]);
            system(command);
            break;
        }
        case NEXT:{
            system("killall -9 mplayer");
            current = (current + VIDEO_NUM + 1) % VIDEO_NUM;
            sprintf(command, "./mplayer -ac mad -slave -quiet -input file=./video_fifo -geometry 0:0 -zoom -x 670 -y 402 ./res/%s", buf[current]);
            system(command);
            break;
        }
        case PREVIOUS:{
            system("killall -9 mplayer");
            current = (current + VIDEO_NUM - 1) % VIDEO_NUM;
            sprintf(command, "./mplayer -ac mad -slave -quiet -input file=./video_fifo -geometry 0:0 -zoom -x 670 -y 402 ./res/%s", buf[current]);
            system(command);
            break;
        }
    }
    pthread_exit(NULL);
}

void myexit(){
    system("killall -9 mplayer");
    system("./main");
    printf("exit\n");
    exit(0);
}

void start(){
    pthread_t pid;
    if (launch_flag){
        launch_mode = CURRENT;
        int ret = pthread_create(&pid, NULL, launch, NULL);
        printf("start\n");
    } else{
        system("killall -CONT mplayer");
        printf("continue\n");
    }
}

void stop(){
    system("killall -STOP mplayer");
    printf("stop\n");
}

void ahead(){
    write(v_fd, "seek +10\n", strlen("seek +10\n"));
    printf("ahead\n");
    }

void back(){
    write(v_fd, "seek -10\n", strlen("seek -10\n"));
    printf("back\n");
}

void next(){
    pthread_t pid;
    launch_mode = NEXT;
    int ret = pthread_create(&pid, NULL, launch, NULL);
    printf("next\n");
}

void previous(){
    pthread_t pid;
    launch_mode = PREVIOUS;
    int ret = pthread_create(&pid, NULL, launch, NULL);
    printf("previous\n");
}

void low_voice(){
    write(v_fd, "volume -5\n", strlen("volume -5\n"));
    printf("low voice");
}

void up_voice(){
    write(v_fd, "volume +5\n", strlen("volume +5\n"));
    printf("up voice");
}


int main(){
    touch_init();
    lcd_init();
    v_fd = open("./video_fifo", O_RDWR);
    printf("init success");


    button(0,416,"./res/exit.bmp", &myexit);
    button(100,416,"./res/start.bmp", &start);
    button(200, 416, "./res/stop.bmp", &stop);
    button(300,416,"./res/back_10.bmp", &back);
    button(400,416,"./res/ahead_10.bmp", &ahead);
    button(500,416,"./res/previous.bmp", &previous);
    button(600,416,"./res/next.bmp", &next);



    while (1){
        get_touch();
    }

    lcd_close();
    touch_close();
}