#include "sys.h"
#include "stm32f10x__tim.h"
#include "IOstream.h"
using namespace periph;

// 软件延时
void delay(volatile int i){ while(i--); }

typedef void(*ins_ptr)(void);

struct task_init_stack_frame {
    u32 r4_to_r11[8];       // lower address
    u32 r0;
    u32 r1;
    u32 r2;
    u32 r3;
    u32 r12;
    u32 lr;
    ins_ptr pc;
    u32 xpsr;               // higher address
	// 构造函数
    task_init_stack_frame(u32 r0_init, u32 lr_init, ins_ptr pc_init):
         r0(r0_init),lr(lr_init),pc(pc_init),xpsr(0x61000000){}
};
STATIC_ASSERT(sizeof(task_init_stack_frame)==(8*4+8*4));

void task(u32 a){	// 任务函数
    while(!a){
        delay(100000);
        pd2 = !pd2;
    }
}

task_init_stack_frame dummy(0,0,(ins_ptr)&task ); /* 为防止栈溢出而定义的数据空间 */
__align(8)  // arm 标准规定栈要 8 字节对齐
task_init_stack_frame base(0,0,(ins_ptr)&task );    // higher address

/* 任务调度算法函数  */
extern "C" void* tick_and_switch(void* cur_stack){
    static void* next_stack = &base;
    void* temp = next_stack;     /* 只有两个任务交替 */
    next_stack = cur_stack;
    return temp;
}

int main(){

    rcc.ClockCmd(&gpioa,ENABLE);
    pa8.Config(OUTPUT);
    rcc.ClockCmd(&gpiod,ENABLE);
    pd2.Config(OUTPUT);
    
    rcc.ClockCmd(&tim1,ENABLE);
    TIM_BaseConfig cfg;
    cfg.prescaler = 720;
    cfg.autoReloadValue = 100;
    // 72 000 000 / (720*100) -> 1000 Hz 左右
    tim1.BaseConfig(cfg);
    tim1.UpdateEventIntCmd(ENABLE);
    tim1.CounterCmd(ENABLE);
    nvic.Config(TIM1_UP_IRQn, ENABLE);
    while(1){
        delay(500000);
        pa8 = !pa8;
    }
}

extern "C" __asm void TIM1_UP_IRQHandler(){

    import tick_and_switch  // 任务调度算法函数
    import clear_int_flag   // 定时器1中断清零函数
    REQUIRE8    // 加这两条对齐伪指令防止链接器报错
    PRESERVE8   // 8 字对齐

    mov r0, sp          // step 1
    sub r0, #(8*4)      // 调整为 r4-r11 压入栈后的状态
    push {lr}
    bl.w tick_and_switch    // 调用算法
    pop {lr}

    cmp r0,#0    // step 2
    beq end

    push {r4-r11}   // r4-r11 入栈
    mov sp,r0       // 更新SP
    pop  {r4-r11}   // 用新的SP恢复r4-r11

end             // step 3
    b.w clear_int_flag
}

extern "C" void clear_int_flag(){    
    tim1.ClearUpdateEventIntFlag();
    return ;
}
