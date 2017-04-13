#ifndef LCD_GUI_H
#define LCD_GUI_H

#include "lcd.h"	/* 硬件层 */

#include "font.h"	/* 字体 */

/**	@brief	点 */
struct Point
{
public:
	int x;
	int y;

	Point(int initX = 0, int initY = 0):x(initX), y(initY){}	
	bool operator ==(const Point& p)	const	{ return ((p.x==x)&&(p.y==y)); }
	bool operator !=(const Point& p) const	{ return (p.x!=x)||(p.y!=y); }
	Point operator*(int mul)	{return Point(x*mul, y*mul);}
	Point& operator *= (const int mul) { x *= mul; y *= mul; return *this; }
	Point& operator=(const Point& p) {
		x = p.x;
		y = p.y;
		return *this;
	}
	Point& operator += (const Point& p) {
		x += p.x;
		y += p.y;
		return *this;
	}
	Point operator +(const Point& p) const { return Point(x+p.x, y+p.y); }
	Point operator -(const Point& p) const {return Point(x-p.x,y-p.y);}
};

/**	@brief 矩形结构体 */
class Rect{
public:
	int x1;
	int y1;
	int x2;
	int y2;
	Rect(int a1, int b1, int a2, int b2): x1(a1), y1(b1), x2(a2), y2(b2){}
};

// class Point{}
// class Line{
// 		p1
//		p2
// }

// class GUI{	// GUI 基类
// public:
// 	virtual void Draw(Point& p) = 0;
// 	// virtual Read();
// 	//virtual void Draw(Line& l);	// 一些基本的算法可以用 Draw(Point&) 来进行默认实现
// };

/**	@brief 背景模式 */
enum BackGroundMode{
	  OPAQUE		/*!< 不透明的 */
	, TRANSPARANT	/*!< 透明的 */
};

/**	@brief GUI 类，提供画图函数
*	@note 这里的函数基本都是 critical section 的 */
class LCD_GUI: public LCD_Screen {
	FontBase const* pFont;	/*!< 当前的字体 */
public:
	void Init()	{LCD_Screen::Init(); ClearScreen(); backMode = OPAQUE; SelectFont(GetSystemFont()); }

	BackGroundMode backMode;
	ColorType backColor;	/*!< 背景色 */
	ColorType frontColor;	/*!< 前景色 */

	/**	@brief 矩形填充 */
	void FillRect(u16 xsta,u16 ysta,u16 xend,u16 yend,ColorType const register color);

	/**	@brief 填充矩形 */
	void FillRect(Rect const& r) { FillRect(r.x1,r.y1,r.x2,r.y2, frontColor); }

	/**	@brief 以背景色填充整个屏幕，清屏函数 */
	void ClearScreen();

	/**	@brief 画直线 */
	void DrawLine(int x0,int x1,int y0,int y1);
	/**	@brief 画直线，推荐使用这个函数 */
	void DrawLine( Point const& p0, Point const& p1)	{DrawLine(p0.x, p0.y, p1.x, p1.y);}
	/**	@brief 画矩形 */
	void DrawRect(Rect const * p);
	void DrawCircle(register int x0,register int y0,int r);
	
	
	void SelectFont(FontBase const * const p);
	
	/**	@brief 显示字符 */
	void DrawChar(u16 x,u16 y,char c);
};

extern LCD_GUI lcd;

#endif
