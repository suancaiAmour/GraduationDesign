#include "sys.h"
using namespace periph;

#define ILI93XX_CPP
#include "lcd.h"

#include "debug.h"

void LCD_Screen::IO_Init() {
	rcc.ClockCmd(&port, ENABLE);
	rcc.ClockCmd(&gpioc, ENABLE);
	rcc.ClockCmd(&afio, ENABLE);
	afio.DebugConfig(SWD_DIS_JTAG_DIS);	/* 引脚都不够用了，还调试个鸟 */

	cs.Config(OUTPUT, PUSH_PULL);
	CS_DeSelect();
	rs.Config(OUTPUT, PUSH_PULL);
	RS_ControlSelect();	/* 默认是数据传送 */
	wr.Config(OUTPUT, PUSH_PULL);
	wr.Set();
	rd.Config(OUTPUT, PUSH_PULL);
	rd.Set();		
	port.Config(PinAll, OUTPUT, PUSH_PULL);
	port=0XFFFF;
	
	bk.Config(OUTPUT, PUSH_PULL);
}

void LCD_Screen::Init() {
	IO_Init();
	Select();	// 选中 LCD
	
	#define WriteReg	SlowWriteReg
	#define ReadReg		SlowReadReg
	
	delay_ms(50); // delay 50 ms 
	WriteReg(0x0000,0x0001);
	delay_ms(50); // delay 50 ms 
	DeviceCode = ReadReg(0x0000);   
	dout<<" LCD ID: "<<(u32)DeviceCode<<endl; //打印LCD ID  
	if((DeviceCode==0x9325)||(DeviceCode==0x9328))//ILI9325
	{
  		WriteReg(0x00e7,0x0010);      
        WriteReg(0x0000,0x0001);//开启内部时钟
        WriteReg(0x0001,0x0100);     
        WriteReg(0x0002,0x0700);//电源开启                    
		//WriteReg(0x0003,(1<<3)|(1<<4) ); 	//65K  RGB
		//DRIVE TABLE(寄存器 03H)
		//BIT3=AM BIT4:5=ID0:1
		//AM ID0 ID1   FUNCATION
		// 0  0   0	   R->L D->U
		// 1  0   0	   D->U	R->L
		// 0  1   0	   L->R D->U
		// 1  1   0    D->U	L->R
		// 0  0   1	   R->L U->D
		// 1  0   1    U->D	R->L
		// 0  1   1    L->R U->D 正常就用这个.
		// 1  1   1	   U->D	L->R
        WriteReg(0x0003,(1<<12)|(3<<4)|(0<<3) );//65K    
        WriteReg(0x0004,0x0000);                                   
        WriteReg(0x0008,0x0207);	           
        WriteReg(0x0009,0x0000);         
        WriteReg(0x000a,0x0000);//display setting         
        WriteReg(0x000c,0x0001);//display setting          
        WriteReg(0x000d,0x0000);//0f3c          
        WriteReg(0x000f,0x0000);
		//电源配置
        WriteReg(0x0010,0x0000);   
        WriteReg(0x0011,0x0007);
        WriteReg(0x0012,0x0000);                                                                 
        WriteReg(0x0013,0x0000);                 
        delay_ms(50); 
        WriteReg(0x0010,0x1590);   
        WriteReg(0x0011,0x0227);
        delay_ms(50); 
        WriteReg(0x0012,0x009c);                  
        delay_ms(50); 
        WriteReg(0x0013,0x1900);   
        WriteReg(0x0029,0x0023);
        WriteReg(0x002b,0x000e);
        delay_ms(50); 
        WriteReg(0x0020,0x0000);                                                            
        WriteReg(0x0021,0x013f);           
		delay_ms(50); 
		//伽马校正
        WriteReg(0x0030,0x0007); 
        WriteReg(0x0031,0x0707);   
        WriteReg(0x0032,0x0006);
        WriteReg(0x0035,0x0704);
        WriteReg(0x0036,0x1f04); 
        WriteReg(0x0037,0x0004);
        WriteReg(0x0038,0x0000);        
        WriteReg(0x0039,0x0706);     
        WriteReg(0x003c,0x0701);
        WriteReg(0x003d,0x000f);
        delay_ms(50); 
		
        WriteReg(0x0050,0x0000); //水平GRAM起始位置 
        WriteReg(0x0051,0x00ef); //水平GRAM终止位置                    
        WriteReg(0x0052,0x0000); //垂直GRAM起始位置                    
        WriteReg(0x0053,0x013f); //垂直GRAM终止位置  
        
        WriteReg(0x0060,0xa700);        
        WriteReg(0x0061,0x0001); 
        WriteReg(0x006a,0x0000);
        WriteReg(0x0080,0x0000);
        WriteReg(0x0081,0x0000);
        WriteReg(0x0082,0x0000);
        WriteReg(0x0083,0x0000);
        WriteReg(0x0084,0x0000);
        WriteReg(0x0085,0x0000);
      
        WriteReg(0x0090,0x0010);     
        WriteReg(0x0092,0x0000);  
        WriteReg(0x0093,0x0003);
        WriteReg(0x0095,0x0110);
        WriteReg(0x0097,0x0000);        
        WriteReg(0x0098,0x0000);  
        //开启显示设置
        WriteReg(0x0007,0x0133);   
        WriteReg(0x0020,0x0000);                                                            
        WriteReg(0x0021,0x013f);
	}else if(DeviceCode==0x9320||DeviceCode==0x9300)
	{
		WriteReg(0x00,0x0000);
		WriteReg(0x01,0x0100);	//Driver Output Contral.
		WriteReg(0x02,0x0700);	//LCD Driver Waveform Contral.
		WriteReg(0x03,0x1030);//Entry Mode Set.
		//WriteReg(0x03,0x1018);	//Entry Mode Set.
	
		WriteReg(0x04,0x0000);	//Scalling Contral.
		WriteReg(0x08,0x0202);	//Display Contral 2.(0x0207)
		WriteReg(0x09,0x0000);	//Display Contral 3.(0x0000)
		WriteReg(0x0a,0x0000);	//Frame Cycle Contal.(0x0000)
		WriteReg(0x0c,(1<<0));	//Extern Display Interface Contral 1.(0x0000)
		WriteReg(0x0d,0x0000);	//Frame Maker Position.
		WriteReg(0x0f,0x0000);	//Extern Display Interface Contral 2.	    
		delay_ms(50); 
		WriteReg(0x07,0x0101);	//Display Contral.
		delay_ms(50); 								  
		WriteReg(0x10,(1<<12)|(0<<8)|(1<<7)|(1<<6)|(0<<4));	//Power Control 1.(0x16b0)
		WriteReg(0x11,0x0007);								//Power Control 2.(0x0001)
		WriteReg(0x12,(1<<8)|(1<<4)|(0<<0));				//Power Control 3.(0x0138)
		WriteReg(0x13,0x0b00);								//Power Control 4.
		WriteReg(0x29,0x0000);								//Power Control 7.
	
		WriteReg(0x2b,(1<<14)|(1<<4));   
		WriteReg(0x50,0);	//Set X Star
		// 水平GRAM终止位置Set X End.
		WriteReg(0x51,239);	//Set Y Star
		WriteReg(0x52,0);	//Set Y End.t.
		WriteReg(0x53,319);	//
	
		WriteReg(0x60,0x2700);	//Driver Output Control.
		WriteReg(0x61,0x0001);	//Driver Output Control.
		WriteReg(0x6a,0x0000);	//Vertical Srcoll Control.
	
		WriteReg(0x80,0x0000);	//Display Position? Partial Display 1.
		WriteReg(0x81,0x0000);	//RAM Address Start? Partial Display 1.
		WriteReg(0x82,0x0000);	//RAM Address End-Partial Display 1.
		WriteReg(0x83,0x0000);	//Displsy Position? Partial Display 2.
		WriteReg(0x84,0x0000);	//RAM Address Start? Partial Display 2.
		WriteReg(0x85,0x0000);	//RAM Address End? Partial Display 2.
	
		WriteReg(0x90,(0<<7)|(16<<0));	//Frame Cycle Contral.(0x0013)
		WriteReg(0x92,0x0000);	//Panel Interface Contral 2.(0x0000)
		WriteReg(0x93,0x0001);	//Panel Interface Contral 3.
		WriteReg(0x95,0x0110);	//Frame Cycle Contral.(0x0110)
		WriteReg(0x97,(0<<8));	//
		WriteReg(0x98,0x0000);	//Frame Cycle Contral.	   
		WriteReg(0x07,0x0173);	//(0x0173)
	}
	
	BackLightOn();		/* 背光 */
	
	#undef WriteReg	
	#undef ReadReg		
	
}

/**	@brief LCD开启显示 */
void LCD_Screen::DisplayOn(void) {		   
	WriteReg(0x07, 0x0173); //26万色显示开启
}

/**	@brief LCD关闭显示 */
void LCD_Screen::DisplayOff(void) {
	WriteReg(0x07, 0x0);//关闭显示
}

void LCD_Screen::Write_SetWindow(u16 x_start, u16 y_start, u16 x_end, u16 y_end) {
	WriteReg(0x50, x_start);
	WriteReg(0x51, x_end);
	WriteReg(0x52, y_start);
	WriteReg(0x53, y_end);
	Write_SetCursor(x_start, y_start);
}

/**	@brief 设置 lcd 内部的GRAM指针位置 */
void LCD_Screen::Write_SetCursor(u16 x, u16 y){
	
	WriteReg(0x20, x);
	WriteReg(0x21, y);
	WriteIndex(0x22);	/* 把 Index 重新设置回 GRAM */
}

/**	@brief 设置 lcd 内部的GRAM指针位置 */
void LCD_Screen::Read_SetCursor(u16 x, u16 y){
	WriteReg(0x20, x);
	WriteReg(0x21, y);
	WriteIndex(0x22);	/* 把 Index 重新设置回 GRAM */
	ReadData();	// dummy read
}



/**	@brief 读指定的像素点 */
LCD_Screen::ColorType LCD_Screen::ReadPixel(u16 x, u16 y){
	Read_SetCursor(x,y);
	ReadData();
	u16 t = ReadData();
	
	return BGR2RGB(t);
}

/**	@brief 在指定的坐标上画点 */
void LCD_Screen::WritePixel(u16 x,u16 y,LCD_Screen::ColorType color) {
	Write_SetCursor(x,y);	/*设置光标位置  */
	WriteData(color.code); 
}

void LCD_Screen::Read_SetWindow(u16 x_start, u16 y_start, u16 x_end, u16 y_end) {
	WriteReg(0x50, x_start);
	WriteReg(0x51, x_end);
	WriteReg(0x52, y_start);
	WriteReg(0x53, y_end);
	Read_SetCursor(x_start, y_start);
}

// //在指定位置画一个指定大小的圆
// //(x,y):中心点
// //r    :半径
// void Draw_Circle(u16 x0,u16 y0,u8 r)
// {
// 	int a,b;
// 	int di;
// 	a=0;b=r;	  
// 	di=3-(r<<1);             //判断下个点位置的标志
// 	while(a<=b)
// 	{
// 		LCD_DrawPoint(x0-b,y0-a);             //3           
// 		LCD_DrawPoint(x0+b,y0-a);             //0           
// 		LCD_DrawPoint(x0-a,y0+b);             //1       
// 		LCD_DrawPoint(x0-b,y0-a);             //7           
// 		LCD_DrawPoint(x0-a,y0-b);             //2             
// 		LCD_DrawPoint(x0+b,y0+a);             //4               
// 		LCD_DrawPoint(x0+a,y0-b);             //5
// 		LCD_DrawPoint(x0+a,y0+b);             //6 
// 		LCD_DrawPoint(x0-b,y0+a);             
// 		a++;
// 		//使用Bresenham算法画圆     
// 		if(di<0)di +=4*a+6;	  
// 		else
// 		{
// 			di+=10+4*(a-b);   
// 			b--;
// 		} 
// 		LCD_DrawPoint(x0+a,y0+b);
// 	}
// } 

// //m^n函数
// u32 mypow(u8 m,u8 n)
// {
// 	u32 result=1;	 
// 	while(n--)result*=m;    
// 	return result;
// 
