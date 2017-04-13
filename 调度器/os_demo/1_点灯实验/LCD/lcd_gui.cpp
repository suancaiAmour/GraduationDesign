#include "lcd_gui.h"

#include "font.h"

#include "debug.h"

LCD_GUI lcd;

/**	@brief ������� */
void LCD_GUI::FillRect(u16 xsta,u16 ysta,u16 xend,u16 yend,ColorType color) {
	/* �����ѭ���У����� color ��Ӧ��ʹ���������ͣ�������ȫ�ֱ�����
	��Ϊ�������ܻᵼ�±��������ܽ����Ż�(�ⲿ�������п��ܻ����ⲿ���ı䣬
	���Ա���������ʱ�򻹻����´��ڴ��ж�����ֵ */
	Write_SetWindow(xsta, ysta, xend, yend);
	WritePixelPrepareColor(color);
	for(u32 register cnt = (xend-xsta+1)*(yend-ysta+1); cnt; cnt--) {
		WritePixel();
	}
	Write_SetWindow(0 ,0 ,x_max ,y_max);	/* �ָ�ȫ�� */
}

/**	@brief lcd �������� */
void LCD_GUI::ClearScreen() {
	FillRect(0, 0, x_max , y_max, backColor);
}

int static finline abs(int a){return a<0?-a:a;}
void static finline swap(int& a, int& b){int c; c = a; a = b; b = c;}
void LCD_GUI::DrawLine(int x0,int y0, int x1, int y1){
	/* Bresenham's line algorithm 
	�㷨�Ļ���˼�룺
	�ȼ���ֱ�ߵ�б��С�ڵ��� 1 (�ԱȽϳ�����Ϊ���ửͼ����ֱ�߲Ż��Ե�����)��
	�ټٶ� y1>=y0���� deltay = y1-y0, deltax = x1 - x0
	��ôÿһ����λ�� x ���������� y �������� deltaerr = deltay / deltax��
	Ȼ��� (x,y) = (x0,y0) ��ʼ�������һ��㣬�ǳ�ʼ����� error Ϊ 0��
	ÿ����һ�����x ���� 1����������� error += deltaerr
	ֻ�е������ >= 0.5 ʱ��y ������ 1��ͬʱ������Լ� 1
	���ݼ�������ص㣬���㷨���������������Ϳ��Եõ����������
	*/
	bool steep = abs(y1 - y0) > abs(x1 - x0);
	if (steep) {
		swap(x0, y0);
		swap(x1, y1);
	}
	if (x0 > x1) {
		swap(x0, x1);
		swap(y0, y1);
	}
	
	int deltax = x1 - x0;
	int deltay = abs(y1 - y0);
	
	/* ԭ�㷨�� real deltaerr = deltay / deltax (ÿһ����λ�� x ������Ӧ�� y ����)������Ϊ�˲��ڼ�����
	����С������ deltaerr �йصĲ��������� deltax*2 */
	int deltaerr = deltay*2;
	/* ����ԭ�㷨��ʼֵΪ 0������Ϊ���� for �п���ֱ���ж� error>=0��
	�͸���һ����ʼֵ -deltax */
	int error = -deltax;

	int ystep = y0<y1? 1 : -1;
	
	for (int x = x0, y = y0; x <= x1; x++ ) {
		/* ����б�ʴ��� 1 ��ֱ�ߣ�Ҫ���⴦�� */
		if (steep) WritePixel(y,x, frontColor);
		else WritePixel(x,y, frontColor);
		
		error += deltaerr;
		
		if (error >= 0){
			y += ystep;
			error = error - 2*deltax;
		}
	}
}

/**	@brief */
void LCD_GUI::DrawRect(Rect const* p){
 	DrawLine(p->x1,p->y1,p->x2,p->y1);
 	DrawLine(p->x1,p->y1,p->x1,p->y2);
 	DrawLine(p->x1,p->y2,p->x2,p->y2);
 	DrawLine(p->x2,p->y1,p->x2,p->y2);
}

void LCD_GUI::SelectFont(FontBase const * const p) {
	assert(p);
	pFont = p; 
}

void LCD_GUI::DrawChar(u16 x,u16 y,char c) {
	u8 const * buf= pFont->GetDotMetrix(c);
	if(buf==NULL){
		dout<<"LCD_GUI::DrawChar -> invalid character"<<endl;
		return;
	}

	Size s;
	pFont->GetSize(c, &s);
	Write_SetWindow(x, y, x+s.width-1, y+s.height-1);
	//WritePixel_Set
	
	/* ����һ�����ص���ֽ��� */
	int const quotient = s.width/8;
	int const remainder = s.width %8;		/* ���� */
	int const n = quotient + (remainder?1:0);
	
	register char temp;
	if(backMode == OPAQUE){
		register ColorType f = frontColor;
		register ColorType b = backColor;
		/* ��Ϊһ������ֶ���ռ���С������Ĭ�ϱ���ɫ�Ļ�Ч�ʻ�Ƚϸ� */		
		WritePixelPrepareColor(b);
		for(int i=0; i<s.height; i++) {
			for(register int j=0; j<quotient; j++) {
				temp = buf[n*i+j];
				for(register int k=8;k;k--) {
					if(temp&0x01){
						WritePixel(f);
						WritePixelPrepareColor(b);	/* �ָ�����ɫ */
					}
					else{
						WritePixel();
					}
					temp >>= 1;			/* ��һ�����ص� */
				}
			}
			if(remainder==0) continue;
			temp = buf[n*i+quotient];
			/* ��һ���в����Ĳ���һ���ֽڵ����ص���д��� */
			for(register int k=remainder; k; k--) {
				if(temp&0x01){ 
					WritePixel(f);
					WritePixelPrepareColor(b);
				}
				else{
					WritePixel();
				}
				temp >>= 1;	
			}
		}
	}
	else if(backMode == TRANSPARANT){
		register ColorType f = frontColor;
		for(register int i=0; i<s.height; i++) {
			for(int j=0; j<quotient; j++) {
				temp = buf[n*i+j];
				for(int k=8;k;k--) {
					if(temp&0x01) WritePixel(x+8*j+k, y+i, f);
					temp >>= 1;			/* ��һ�����ص� */
				}
			}
			if(remainder==0) continue;
			temp = buf[n*i+quotient];
			/* ��һ���в����Ĳ���һ���ֽڵ����ص���д��� */
			for(register int k=remainder; k; k--) {	
				if(temp&0x01) WritePixel(x+8*quotient+k, y+i, f);
				temp >>= 1;	
			}
		}
	}
	
	Write_SetWindow(0, 0, x_max, y_max);	/* �������û�ȫ�� */
}


/**	@brief ��һ��Բ 
*	@param x: Բ�ĵĺ�����
*	@param y: Բ�ĵ�������
*	@param r: Բ�İ뾶
*/
void LCD_GUI::DrawCircle(register int x0,register int y0,int r)
{
	/*
	ԭ�㷨�� di = 5/4 - r; Ϊ�˱��⸡�����㣬
	��ʼֵ�Լ� di ��صĵ���������� 4��
	�磬�� d'[i] = c*d[i], ��֪�� d[i+1] = a*d[i]+b ��
	�Ϳ��Եó� d'[i+1]=a*d'[i]+ c * b;
	*/
	int  di = 5 - r*4;
	register ColorType c = frontColor;	/* ���ڴ��е��� */
	for(register int a=0, b=r;
		a<=b; a++)
	{
		/* 
		�� (x0+0, y0+r) ��ʼ��㻭 1/8 Բ��
		����Բ�ĶԳ��ԣ�ÿ��һ���㶼����֪��
		Բ���������ֵĶ�Ӧ�� 7 ����
		*/
		WritePixel(x0+b,y0-a, c);                 
		WritePixel(x0-a,y0+b, c);             
		WritePixel(x0-b,y0-a, c);             
		WritePixel(x0-a,y0-b, c);                      
		WritePixel(x0+b,y0+a, c);                     
		WritePixel(x0+a,y0-b, c);          
		WritePixel(x0+a,y0+b, c);           
		WritePixel(x0-b,y0+a, c);   
		if(di<0) {
			di += (8*a + 3);		/* ��û���⸡������֮ǰ���� di += (2*x + 3); */
		}
		else {
			di+=( 20 +8*(a-b) );   /* ��û���⸡������֮ǰ���� di+= ( 5+2*(a-b) ); */
			b--;
		}
	}
} 

// #include "sys.h"
// using namespace periph;
// #include "utilities.H"

// #include "stm32f10x__tim.h"

// #include "lcd_gui.h"

// #include "debug.h"

// u16 cnt = 0;
// volatile u16 testEn = 1;


// /* LCD ֡��ͳ�� */
// void TIM_Init() {
// 	rcc.ClockCmd(tim1, ENABLE);
// 	
// 	TIM_BaseConfig base;
//     base.    prescaler = sys.GetSysclk()/10000-1;
//     base.    autoReloadValue = 10000-1;
//     tim1.    BaseConfig(base);
// 	
// 	tim1. SetCounter(0);
// 	
// 	tim1.UpdateEventIntCmd(ENABLE);
// 	nvic. Config(TIM1_UP_IRQn, ENABLE);
// 	
// 	pa8.Config(OUTPUT);
// }

// struct{
// 	RGB565 color;
// 	char* str;
// }color[] = {
// 	  {RGB565(31,0,0), "��ɫ"}
// 	, {RGB565(0,63,0), "��ɫ"}
// 	, {RGB565(0,0,31), "��ɫ"}
// // 	, {RGB565(0,63,0), "��ɫ"}
//  	, {RGB565(31,0,31), "��ɫ"}
// // 	, {RGB565(31,31,0), "��ɫ"}
// // 	, {RGB565(31,0,31), "��ɫ"}
// // 	, {RGB565(31,31,0), "��ɫ"}
// };
// int const COLOR_NUM = (sizeof(color)/sizeof(color[0]));

// int main(void)
// {
// 	sys.Init();
// 	
// 	lcd.Init(); 	
// 	Delay_ms(1000);
// 	
// 	void ColorTest();
// 	void RateTest();
// 	void FillTest();
// 	void LineTest() ;
// 	void CharDisplayTest();
// 	
// 	ColorTest();
// 	RateTest();

// 	CharDisplayTest();
// 	FillTest();
// 	LineTest();

// 	while(1);
// }

// void ColorTest() {
// 	output<<endl
// 		<<"��ʼ������ɫ����"<<endl<<endl;
// 	for(int i=0;i<sizeof(color)/sizeof(color[0]);i++){
// 		lcd.backColor = color[i].color;
// 		lcd.ClearScreen();
// 		output<<(u32)i<<". д�����ɫ����Ϊ"<< color[i].color.code<<"����"<<color[i].str<<endl
// 				<<"��GRAM����������ɫΪ"<< lcd.ReadPixel(120,160).code <<endl<<endl;
// 		Delay_ms(500);
// 	}
// 	output<<endl;
// }

// void RateTest() {
// 	TIM_Init();
// 	output<<"��ʼ����֡�ʲ���"<<endl;
// 	tim1.CounterCmd(ENABLE);	/* ����ʱ�� */
// 	RGB565 c = 0;
// 	while(testEn){
// 		lcd.backColor = c++;
// 		lcd.ClearScreen();
// 		cnt++;
// 	}
// 	tim1.CounterCmd(DISABLE);
// 	output<<"֡�ʲ��Խ���"<<endl<<endl;
// }

// void FillTest() {
// 	output<<"�����β���"<<endl<<endl;
// 	int x_max=lcd.x_max;
// 	//int y_max=lcd.y_max;
// 	for(int i=0, step = lcd.height/COLOR_NUM; i<COLOR_NUM; i++){
// 		lcd.FillRect(0, i*step, x_max/2, (i+1)*step-1, color[i].color);
// 	}
// 	for(int i=0, step = lcd.height/COLOR_NUM; i<COLOR_NUM; i++){
// 		lcd.FillRect(x_max/2+1, i*step, x_max, (i+1)*step-1, color[COLOR_NUM-1-i].color);
// 	}

// }

// void LineTest() {
// 	output<<"���߲���"<<endl<<endl;
// 	int const x_max = lcd.x_max;
// 	int const y_max = lcd.y_max;
// 	lcd.frontColor = COLOR_BLACK;
// 	lcd.DrawLine(0,0, x_max, y_max);
// 	lcd.DrawLine(x_max ,0,0, y_max);
// 	lcd.DrawLine(0, y_max/2, x_max, y_max/2);
// 	lcd.DrawLine(x_max/2, 0, x_max/2, y_max);
// 	Delay_ms(1000);
// 	
// 	output<<"�����β���"<<endl<<endl;
// 	Rect r(40,120,200,200);
// 	lcd.DrawRect(&r);
// 	Delay_ms(1000);
// 	
// 	output<<"��Բ����"<<endl<<endl;
// 	for(int i=10; i<= 110; i+=5) {
// 		lcd.DrawCircle(120,160,i);
// 		Delay_ms(500);
// 	}
// }

// void CharDisplayTest(){
// 	output<<"�ַ�ˢ������"<<endl<<endl;
// 	testEn = 1;
// 	tim1.CounterCmd(ENABLE);
// 	lcd.frontColor = COLOR_RED;
// 	lcd.backColor = COLOR_BLACK;
// 	while(testEn){
// 		for(int i=0; i+8<=240; i+= 8)
// 			for(int j=0; j+16<=320; j+=16)
// 				lcd.DrawChar(i,j,'1');
// 		cnt++;
// 		lcd.frontColor++;
// 	}
// 	tim1.CounterCmd(DISABLE);
// }

// ARMAPI void TIM1_UP_IRQHandler() {
// 	output<<"�ڹ�ȥ�� 1 ����ˢ�� "<<cnt<<" ����"<<endl;
// 	cnt = 0;
// 	pa8 = !pa8;
// 	
// 	static u8 testCnt = 0;
// 	testCnt++;
// 	if(testCnt>=5){
// 		testEn = 0;
// 		testCnt = 0;
// 	}
// 	
// 	tim1.ClearUpdateEventIntFlag();
// 	tim1.SetCounter(0);
// }

