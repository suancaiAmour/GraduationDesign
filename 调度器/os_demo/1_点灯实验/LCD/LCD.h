#ifndef __LCD_H
#define __LCD_H

#include "sys.h"
using namespace periph;

#define WRITE_WAIT	0

/**	@brief ��ͼ��ɫ */
#define COLOR_WHITE		RGB565(31,63,31)
#define COLOR_BLACK		RGB565(0,0,0)	  
#define COLOR_BLUE		RGB565(0,0,31)  
#define COLOR_RED		RGB565(31,0,0)  
#define COLOR_PURPLE	RGB565(31,0,31)

struct RGB565{
	u16 code;	/*!< ��ɫ���� */
	operator u16&() {return code;}
	finline RGB565(){}
	finline RGB565(const u16 init_code):code(init_code){}
	finline RGB565(const u8 r,const u8 g,const u8 b):code(((r&BitFromTo(4,0))<<(6+5))|((g&BitFromTo(5,0))<<5)|((b&BitFromTo(4,0)))) {}
};

class LCD_Screen{

private:
	/********************************* �ź�ʱ��� *********************************
	***/
	
	#define rd		pc6
	#define wr		pc7
	#define rs		pc8
	#define cs		pc9
	#define bk		pc10
	#define port	gpiob

	static void IO_Init();
	
	/**	@brief LCD Ƭѡ�źţ�����Ƭѡ�źź󣬺�LCD���������Ų�����������; */
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
		wr.Reset();		/* д���� */
		wr.Set();
	}
	
	static void finline PutDataOnPort(u16 const data)	{ port.Write(data); }
	static void finline WritePulse()	{ wr.Reset(); wr.Set(); }
	
	/**	@brief ��׼����������ΪIO����˫��ģ�
      * Ĭ����д״̬�������ڶ�֮ǰҪ����Ϊ��״̬
      */
	static void finline ReadBegin()	{port.Set(PinAll); port.Config(INPUT, PULL_UP); }

    /**	@brief ��LCD�ж������ݺ�
      * ��IO���������û�д״̬ 
      */
	static void finline ReadEnd()	{ port.Config(OUTPUT, PUSH_PULL); }
	static u16 finline ReadData() {
		ReadBegin();
		/* ���д��ֻ��Ϊ����ʱ�����ܶ�����ȷ��ֵ */
		/* �� __nop() ���ܻᱻ�������Ż��� */
		rd.Reset();
 		for(int volatile register i=1;i;i--)
 			rd.Reset();
		u16 t = port.Read(); 
		rd.Set();
		ReadEnd();
		return t;
	}

	/**	@brief ��������� */
	static finline void BackLightOn() {pc10.Set();}
		
	/**	@brief �Ĵ�����ַ */
	static void finline WriteIndex(u16 const data) {
		RS_IndexSelect();//д��ַ  
		WriteData(data);
		RS_ControlSelect();		/* rs Ĭ�ϸߵ�ƽ */
	}

	/**	@brief д�Ĵ��� */
	static void finline WriteReg(u16 const index, u16 const LCD_RegValue) {	
		WriteIndex(index);  
		WriteData(LCD_RegValue);	    		 
	}
	
	/**	@brief ���Ĵ��� */
	static u16 finline ReadReg(u8 const index) {			   
		u16 t;
		WriteIndex(index);  //д��Ҫ���ļĴ�����  
		t= ReadData();  
		return t;  
	}
	
	/***********���ٲ�����Ϊ���ܹ�������ʼ�������õĺ���***********
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
		RS_IndexSelect();//д��ַ  
		SlowWriteData(data);
		RS_ControlSelect();		/* rs Ĭ�ϸߵ�ƽ */
	}
	
	static u16 SlowReadReg(u16 const index) {
		u16 t;
		SlowWriteIndex(index);  //д��Ҫ���ļĴ�����  
		t= SlowReadData();  
		return t;  
	}
	
	static void SlowWriteReg(u16 index, u16 data) {
		SlowWriteIndex(index);  
		SlowWriteData(data);
	}
	
	/***
	***********���ٲ�����Ϊ���ܹ�������ʼ�������õĺ���***********/
	
#ifndef ILI93XX_CPP
	#undef cs
	#undef rs
	#undef wr
	#undef rd
	#undef port
#endif
	/***
	********************************* �ź�ʱ��� *********************************/
	
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
	/**	@brief �����ⲿ�û�ʹ�ã���Ϊ����ֻ��Ϊ��ͨ���Ժ͸���һ����Ϊ�ӿڶ�����
	*	�ı�����������͵ı�������ʾ�ľ������ݵ����岢����ȷ���û�ֻ��������ɫʱ
	*	ֻ��Ҫʹ���� RGB565��RGB555 �� RGB888 �ȣ���Щ���Ͷ��������Ͱ�ȫ���ƣ���
	*	������˼Ҳ����ȷ */
	typedef RGB565	ColorType;

public:

	/**	@brief LCD Ƭѡ�źţ�����Ƭѡ�źź󣬺�LCD���������Ų�����������; */
	static void finline Select()	{ CS_Select(); }
	static void finline DeSelect()	{ CS_DeSelect(); }

	LCD_Screen(){}
	/**	@brief ��ʼ�� LCD����ʼ����ɺ�Ƭѡ�ź�Ϊ����״̬ */
	void Init();
	void DisplayOn();	/* �� LCD ��ʾ */
	void DisplayOff();	 
	
	enum{
		  width = 240
		, height = 320
		
		, x_max = width-1
		, y_max = height-1
	};
		
	/* ��ͼ���� */
	//void FillRect(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 color);
	//void ClearScreen(u16 RGB_Code);
	
	/**	@brief ����ĺ�����Ϊ�û��Զ��廭ͼ�����õģ���Ϊ�������Ľӿ� */
	/**	@{ */
	
	/**	@brief ��һ��д����
	*	@note �������ֻ���û��Զ���д������ã�������������Ŀ�����
	*	Write_SetCursor �� WritePixel��һ������������������������������ü���ȡ��
	*	����һ�ε��� Write_SetCursor �� WritePixel ֮ǰ��������������һ��д����
	*/
	void Write_SetWindow(u16 x_start, u16 y_start, u16 x_end, u16 y_end);
	void Write_SetCursor(u16 x, u16 y);
	void WritePixel(u16 x, u16 y, ColorType color);
	/**	@broef ��ָ������ɫдһ�����ص㣬�������֮����� WritePixel() (�޲���)��
	*	�ͻ�ʹ�õ�ǰ����ɫ���������һ�����ص� */
	finline void  WritePixel(const ColorType& color)	{ WriteData(color.code); }
	/**	@brief ѡ��д�����ص��ϵ���ɫ�������������ֻ�ܸ��� WritePixel() (�޲���)��
	*	@note һ������������������������������ü���ȡ�� */
	finline void WritePixelPrepareColor(const ColorType& color)	{ PutDataOnPort(color.code); }
	finline void WritePixel()		{ WritePulse(); }
	
	/**	@brief ��һ��������
	*	@note �������ֻ���û��Զ�������ص�����ã�������������Ŀ�����
	*	Read_SetCursor �� ReadPixel��һ������������������������������ü���ȡ��
	*	����һ�ε��� Read_SetCursor �� ReadPixel ֮ǰ��������������һ��������
	*/
	void Read_SetWindow(u16 x_start, u16 y_start, u16 x_end, u16 y_end);
	ColorType  ReadPixel(u16 x, u16 y);
	 
	/**	@note �����������д��� */
	 //void finline SkipPixel()	{ ReadPixel();/* dummy read */ WritePixel(ReadPixel()); }
	//finline ColorType ReadPixel()	{ RGB565 r; r.code = ReadData(); return BGR2RGB(r);}
	void Read_SetCursor(u16 x, u16 y);
	
	/**	@} */
};


#endif  
