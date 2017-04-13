#include "lcd_gui.h"

#include "font.h"

#include "debug.h"

LCD_GUI lcd;

/**	@brief 矩形填充 */
void LCD_GUI::FillRect(u16 xsta,u16 ysta,u16 xend,u16 yend,ColorType color) {
	/* 在这个循环中，变量 color 不应该使用引用类型，或者是全局变量，
	因为这样可能会导致编译器不能进行优化(外部变量都有可能会在外部被改变，
	所以编译器读的时候还会重新从内存中读它的值 */
	Write_SetWindow(xsta, ysta, xend, yend);
	WritePixelPrepareColor(color);
	for(u32 register cnt = (xend-xsta+1)*(yend-ysta+1); cnt; cnt--) {
		WritePixel();
	}
	Write_SetWindow(0 ,0 ,x_max ,y_max);	/* 恢复全屏 */
}

/**	@brief lcd 清屏函数 */
void LCD_GUI::ClearScreen() {
	FillRect(0, 0, x_max , y_max, backColor);
}

int static finline abs(int a){return a<0?-a:a;}
void static finline swap(int& a, int& b){int c; c = a; a = b; b = c;}
void LCD_GUI::DrawLine(int x0,int y0, int x1, int y1){
	/* Bresenham's line algorithm 
	算法的基本思想：
	先假设直线的斜率小于等于 1 (以比较长的轴为基轴画图来的直线才会显得连续)，
	再假定 y1>=y0，记 deltay = y1-y0, deltax = x1 - x0
	那么每一个单位的 x 增量带来的 y 的增量是 deltaerr = deltay / deltax，
	然后从 (x,y) = (x0,y0) 开始从左往右画点，记初始总误差 error 为 0，
	每画完一个点后，x 自增 1，并且总误差 error += deltaerr
	只有当总误差 >= 0.5 时，y 才自增 1，同时总误差自减 1
	根据计算机的特点，对算法进行整数化处理后就可以得到这个函数。
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
	
	/* 原算法是 real deltaerr = deltay / deltax (每一个单位的 x 增量对应的 y 增量)，这里为了不在计算中
	出现小数，把 deltaerr 有关的操作都乘以 deltax*2 */
	int deltaerr = deltay*2;
	/* 总误差，原算法初始值为 0，这里为了让 for 中可以直接判断 error>=0，
	就赋了一个初始值 -deltax */
	int error = -deltax;

	int ystep = y0<y1? 1 : -1;
	
	for (int x = x0, y = y0; x <= x1; x++ ) {
		/* 对于斜率大于 1 的直线，要特殊处理 */
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
	
	/* 计算一行像素点的字节数 */
	int const quotient = s.width/8;
	int const remainder = s.width %8;		/* 余数 */
	int const n = quotient + (remainder?1:0);
	
	register char temp;
	if(backMode == OPAQUE){
		register ColorType f = frontColor;
		register ColorType b = backColor;
		/* 因为一般的文字都是占面积小，所以默认背景色的话效率会比较高 */		
		WritePixelPrepareColor(b);
		for(int i=0; i<s.height; i++) {
			for(register int j=0; j<quotient; j++) {
				temp = buf[n*i+j];
				for(register int k=8;k;k--) {
					if(temp&0x01){
						WritePixel(f);
						WritePixelPrepareColor(b);	/* 恢复背景色 */
					}
					else{
						WritePixel();
					}
					temp >>= 1;			/* 下一个像素点 */
				}
			}
			if(remainder==0) continue;
			temp = buf[n*i+quotient];
			/* 对一行中残留的不足一个字节的像素点进行处理 */
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
					temp >>= 1;			/* 下一个像素点 */
				}
			}
			if(remainder==0) continue;
			temp = buf[n*i+quotient];
			/* 对一行中残留的不足一个字节的像素点进行处理 */
			for(register int k=remainder; k; k--) {	
				if(temp&0x01) WritePixel(x+8*quotient+k, y+i, f);
				temp >>= 1;	
			}
		}
	}
	
	Write_SetWindow(0, 0, x_max, y_max);	/* 重新设置回全屏 */
}


/**	@brief 画一个圆 
*	@param x: 圆心的横坐标
*	@param y: 圆心的纵坐标
*	@param r: 圆的半径
*/
void LCD_GUI::DrawCircle(register int x0,register int y0,int r)
{
	/*
	原算法是 di = 5/4 - r; 为了避免浮点运算，
	初始值以及 di 相关的迭代计算乘以 4，
	如，令 d'[i] = c*d[i], 又知道 d[i+1] = a*d[i]+b ，
	就可以得出 d'[i+1]=a*d'[i]+ c * b;
	*/
	int  di = 5 - r*4;
	register ColorType c = frontColor;	/* 从内存中调入 */
	for(register int a=0, b=r;
		a<=b; a++)
	{
		/* 
		从 (x0+0, y0+r) 开始描点画 1/8 圆，
		利用圆的对称性，每描一个点都可以知道
		圆的其它部分的对应的 7 个点
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
			di += (8*a + 3);		/* 在没避免浮点运算之前，是 di += (2*x + 3); */
		}
		else {
			di+=( 20 +8*(a-b) );   /* 在没避免浮点运算之前，是 di+= ( 5+2*(a-b) ); */
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


// /* LCD 帧率统计 */
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
// 	  {RGB565(31,0,0), "红色"}
// 	, {RGB565(0,63,0), "绿色"}
// 	, {RGB565(0,0,31), "蓝色"}
// // 	, {RGB565(0,63,0), "绿色"}
//  	, {RGB565(31,0,31), "紫色"}
// // 	, {RGB565(31,31,0), "橙色"}
// // 	, {RGB565(31,0,31), "紫色"}
// // 	, {RGB565(31,31,0), "橙色"}
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
// 		<<"开始进行颜色测试"<<endl<<endl;
// 	for(int i=0;i<sizeof(color)/sizeof(color[0]);i++){
// 		lcd.backColor = color[i].color;
// 		lcd.ClearScreen();
// 		output<<(u32)i<<". 写入的颜色代码为"<< color[i].color.code<<"，是"<<color[i].str<<endl
// 				<<"从GRAM读回来的颜色为"<< lcd.ReadPixel(120,160).code <<endl<<endl;
// 		Delay_ms(500);
// 	}
// 	output<<endl;
// }

// void RateTest() {
// 	TIM_Init();
// 	output<<"开始进行帧率测试"<<endl;
// 	tim1.CounterCmd(ENABLE);	/* 开定时器 */
// 	RGB565 c = 0;
// 	while(testEn){
// 		lcd.backColor = c++;
// 		lcd.ClearScreen();
// 		cnt++;
// 	}
// 	tim1.CounterCmd(DISABLE);
// 	output<<"帧率测试结束"<<endl<<endl;
// }

// void FillTest() {
// 	output<<"填充矩形测试"<<endl<<endl;
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
// 	output<<"画线测试"<<endl<<endl;
// 	int const x_max = lcd.x_max;
// 	int const y_max = lcd.y_max;
// 	lcd.frontColor = COLOR_BLACK;
// 	lcd.DrawLine(0,0, x_max, y_max);
// 	lcd.DrawLine(x_max ,0,0, y_max);
// 	lcd.DrawLine(0, y_max/2, x_max, y_max/2);
// 	lcd.DrawLine(x_max/2, 0, x_max/2, y_max);
// 	Delay_ms(1000);
// 	
// 	output<<"画矩形测试"<<endl<<endl;
// 	Rect r(40,120,200,200);
// 	lcd.DrawRect(&r);
// 	Delay_ms(1000);
// 	
// 	output<<"画圆测试"<<endl<<endl;
// 	for(int i=10; i<= 110; i+=5) {
// 		lcd.DrawCircle(120,160,i);
// 		Delay_ms(500);
// 	}
// }

// void CharDisplayTest(){
// 	output<<"字符刷屏测试"<<endl<<endl;
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
// 	output<<"在过去的 1 秒内刷了 "<<cnt<<" 次屏"<<endl;
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

