//
// Created by YDzzz on 2024/6/30.
//

#include <stdlib.h>
#include "./tools/Tools.h"


void music(){
    touch_close();
    lcd_close();
    system("./music");
    exit(0);
}

void video(){
    touch_close();
    lcd_close();
    system("/movie");
    exit(0);
}

void measure(){
    touch_close();
    lcd_close();
    system("./measure");
    exit(0);
}

void photo(){
    touch_close();
    lcd_close();
    system("./picture");
    exit(0);
}

void game(){
    touch_close();
    lcd_close();
    system("./game");
    exit(0);
}

int main(){
    lcd_init();
    touch_init();

    button(50,50,"./res/video_icon.bmp",&video);
    button(200,50,"./res/music_icon.bmp",&music);
    button(350,50,"./res/measure_icon.bmp",&measure);
    button(500,50,"./res/photo_icon.bmp",&photo);
    button(650,50,"./res/game_icon.bmp",&game);
    while(1){
        get_touch();
    }
}