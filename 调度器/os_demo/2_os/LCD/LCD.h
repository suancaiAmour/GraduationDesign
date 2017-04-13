#ifndef __LCD_H
#define __LCD_H

#include "sys.h"
using namespace periph;

#define WRITE_WAIT	0

/**	@brief 画图颜色 */
#define COLOR_WHITE		RGB565(31,63,31)
#define COLOR_BLACK		RGB565(0,0,0)	  
#define COLOR_BLUE		RGB565(0,0,31)  
#define COLOR_RED		RGB565(31,0,0)  
#define COLOR_PURPLE	RGB565(31,0,31)

struct RGB565{
	u16 code;	/*!< 颜色代码 */
	operator u16&() {return code;}
	finline RGB565(){}
	finline RGB565(const u16 init_code):code(init_code){}
	finline RGB565(const u8 r,const u8 g,const u8 b):code(((r&BitFromTo(4,0))<<(6+5))|((g&BitFromTo(5,0))<<5)|((b&BitFromTo(4,0)))) {}
};

class LCD_Screen{

private:
	/********************************* 信号时序层 *********************************
	***/
	
	#define rd		pc6
	#define wr		pc7
	#define rs		pc8
	#define cs		pc9
	#define bk		pc10
	#define port	gpiob

	static void IO_Init();
	
	/**	@brief LCD 片选信号，开启片选信号后，和LCD相连的引脚不用做其它用途 */
	static void finline CS_Select()	{ cs.Reset(); }
	static void finline CS_DeSelect()	{ cs.Set(); }

	/**	@brief select an index or status register */
	static void finline RS_IndexSelect()	{ rs.Reset(); }
	/**	@brief select a control register */
	static void finline RS_ControlSelect()	{ rs.Set(); }

	static void finline  WriteData(u16 const data){
		port.Write(data);
	#if (WRITE_WAIT>0)
		for(int volatile i=WRITE_WAIT;i;i--);
	#endif
		wr.Reset();		/* 写脉冲 */
		wr.Set();
	}
	
	static void finline PutDataOnPort(u16 const data)	{ port.Write(data); }
	static void finline WritePulse()	{ wr.Reset(); wr.Set(); }
	
	/**	@brief 读准备函数，因为IO口是双向的，
      * 默认在写状态，所以在读之前要配置为读状态
      */
	static void finline ReadBegin()	{port.Set(PinAll); port.Config(INPUT, PULL_UP); }

    /**	@brief 从LCD中读完数据后，
      * 把IO口重新配置回写状态 
      */
	static void finline ReadEnd()	{ port.Config(OUTPUT, PUSH_PULL); }
	static u16 finline ReadData() {
		ReadBegin();
		/* 多次写，只是为了延时，才能读到正确的值 */
		/* 用 __nop() 可能会被编译器优化掉 */
		rd.Reset();
 		for(int volatile register i=1;i;i--)
 			rd.Reset();
		u16 t = port.Read(); 
		rd.Set();
		ReadEnd();
		return t;
	}

	/**	@brief 点亮背光灯 */
	static finline void BackLightOn() {pc10.Set();}
		
	/**	@brief 寄存器地址 */
	static void finline WriteIndex(u16 const data) {
		RS_IndexSelect();//写地址  
		WriteData(data);
		RS_ControlSelect();		/* rs 默认高电平 */
	}

	/**	@brief 写寄存器 */
	static void finline WriteReg(u16 const index, u16 const LCD_RegValue) {	
		WriteIndex(index);  
		WriteData(LCD_RegValue);	    		 
	}
	
	/**	@brief 读寄存器 */
	static u16 finline ReadReg(u8 const index) {			   
		u16 t;
		WriteIndex(index);  //写入要读的寄存器号  
		t= ReadData();  
		return t;  
	}
	
	/***********慢速操作，为了能够正常初始化而设置的函数***********
	***/
	
	static void SlowWriteData(u16 data) {
		port.Write(data);
		Delay_us(2);
		wr.Reset();
		Delay_us(5);
		wr.Set();
		Delay_us(2);
	}
	static u16 SlowReadData() {
		ReadBegin();
		Delay_us(2);
		rd.Reset();
 		Delay_us(5);
		u16 t = port.Read(); 
		rd.Set();
		Delay_us(2);
		ReadEnd();
		return t;
	}
	static void finline SlowWriteIndex(u16 const data) {
		RS_IndexSelect();//写地址  
		SlowWriteData(data);
		RS_ControlSelect();		/* rs 默认高电平 */
	}
	
	static u16 SlowReadReg(u16 const index) {
		u16 t;
		SlowWriteIndex(index);  //写入要读的寄存器号  
		t= SlowReadData();  
		return t;  
	}
	
	static void SlowWriteReg(u16 index, u16 data) {
		SlowWriteIndex(index);  
		SlowWriteData(data);
	}
	
	/***
	***********慢速操作，为了能够正常初始化而设置的函数***********/
	
#ifndef ILI93XX_CPP
	#undef cs
	#undef rs
	#undef wr
	#undef rd
	#undef port
#endif
	/***
	********************************* 信号时序层 *********************************/
	
	static u16 BGR2RGB(u16 c) {
		u16  r,g,b,rgb;   
		b=(c>>0)&0x1f;
		g=(c>>5)&0x3f;
		r=(c>>11)&0x1f;	 
		rgb=(b<<11)|(g<<5)|(r<<0);		 
		return(rgb);
	}

	u32 DeviceCode;

protected:
	/**	@brief 不给外部用户使用，因为这里只是为了通用性和给下一级作为接口而定义
	*	的变量，这个类型的本身所表示的具体数据的意义并不精确，用户只在设置颜色时
	*	只需要使用类 RGB565，RGB555 或 RGB888 等，这些类型定义有类型安全机制，所
	*	表达的意思也很明确 */
	typedef RGB565	ColorType;

public:

	/**	@brief LCD 片选信号，开启片选信号后，和LCD相连的引脚不用做其它用途 */
	static void finline Select()	{ CS_Select(); }
	static void finline DeSelect()	{ CS_DeSelect(); }

	LCD_Screen(){}
	/**	@brief 初始化 LCD，初始化完成后片选信号为开启状态 */
	void Init();
	void DisplayOn();	/* 开 LCD 显示 */
	void DisplayOff();	 
	
	enum{
		  width = 240
		, height = 320
		
		, x_max = width-1
		, y_max = height-1
	};
		
	/* 画图函数 */
	//void FillRect(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 color);
	//void ClearScreen(u16 RGB_Code);
	
	/**	@brief 下面的函数是为用户自定义画图而设置的，作为纯软件层的接口 */
	/**	@{ */
	
	/**	@brief 开一个写窗口
	*	@note 这个函数只有用户自定义写入而设置，跟随这个函数的可以是
	*	Write_SetCursor 和 WritePixel，一旦调用了其它函数，这个函数的作用即被取消
	*	在下一次调用 Write_SetCursor 和 WritePixel 之前，必须重新设置一个写窗口
	*/
	void Write_SetWindow(u16 x_start, u16 y_start, u16 x_end, u16 y_end);
	void Write_SetCursor(u16 x, u16 y);
	void WritePixel(u16 x, u16 y, ColorType color);
	/**	@broef 用指定的颜色写一个像素点，如果在这之后调用 WritePixel() (无参数)，
	*	就会使用当前的颜色继续填充下一个像素点 */
	finline void  WritePixel(const ColorType& color)	{ WriteData(color.code); }
	/**	@brief 选择写在像素点上的颜色，这个函数后面只能跟随 WritePixel() (无参数)，
	*	@note 一旦跟随了其它函数，这个函数的作用即被取消 */
	finline void WritePixelPrepareColor(const ColorType& color)	{ PutDataOnPort(color.code); }
	finline void WritePixel()		{ WritePulse(); }
	
	/**	@brief 开一个读窗口
	*	@note 这个函数只有用户自定义读像素点而设置，跟随这个函数的可以是
	*	Read_SetCursor 和 ReadPixel，一旦调用了其它函数，这个函数的作用即被取消
	*	在下一次调用 Read_SetCursor 和 ReadPixel 之前，必须重新设置一个读窗口
	*/
	void Read_SetWindow(u16 x_start, u16 y_start, u16 x_end, u16 y_end);
	ColorType  ReadPixel(u16 x, u16 y);
	 
	/**	@note 这两个函数有错误 */
	 //void finline SkipPixel()	{ ReadPixel();/* dummy read */ WritePixel(ReadPixel()); }
	//finline ColorType ReadPixel()	{ RGB565 r; r.code = ReadData(); return BGR2RGB(r);}
	void Read_SetCursor(u16 x, u16 y);
	
	/**	@} */
};


#endif  
