#include "sys.h"
using namespace periph;

// �����ʱ����
void delay(volatile int i){
    while(i--);
}

int main(){
    
    rcc.ClockCmd(&gpioa, ENABLE);   // ʹ�� GPIOA
    pa8.Config(OUTPUT); // ���� pa8 Ϊ���

    while(1){
        delay(500000);  // ��ʱ
        pa8 != pa8;     // ȡ��
    }
}

/*
int main(){
    
    rcc.ClockCmd(&gpioa, ENABLE);   // ʹ�� GPIOA
    pa8.Config(OUTPUT); // ���� pa8 Ϊ���
    
    rcc.ClockCmd(&gpiod,ENABLE);
    pd2.Config(OUTPUT);
    
    while(1){
        delay(500000);  // ��ʱ
        pa8 != pa8;     // ȡ��
        pd2 != pd2;
    }
}

*/
