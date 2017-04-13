#include "sys.h"
using namespace periph;

// 软件延时函数
void delay(volatile int i){
    while(i--);
}

int main(){
    
    rcc.ClockCmd(&gpioa, ENABLE);   // 使能 GPIOA
    pa8.Config(OUTPUT); // 配置 pa8 为输出

    while(1){
        delay(500000);  // 延时
        pa8 != pa8;     // 取反
    }
}

/*
int main(){
    
    rcc.ClockCmd(&gpioa, ENABLE);   // 使能 GPIOA
    pa8.Config(OUTPUT); // 配置 pa8 为输出
    
    rcc.ClockCmd(&gpiod,ENABLE);
    pd2.Config(OUTPUT);
    
    while(1){
        delay(500000);  // 延时
        pa8 != pa8;     // 取反
        pd2 != pd2;
    }
}

*/
