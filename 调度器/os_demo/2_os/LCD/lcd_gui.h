#ifndef LCD_GUI_H
#define LCD_GUI_H

#include "lcd.h"	/* Ӳ���� */

#include "font.h"	/* ���� */

/**	@brief	�� */
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

/**	@brief ���νṹ�� */
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

// class GUI{	// GUI ����
// public:
// 	virtual void Draw(Point& p) = 0;
// 	// virtual Read();
// 	//virtual void Draw(Line& l);	// һЩ�������㷨������ Draw(Point&) ������Ĭ��ʵ��
// };

/**	@brief ����ģʽ */
enum BackGroundMode{
	  OPAQUE		/*!< ��͸���� */
	, TRANSPARANT	/*!< ͸���� */
};

/**	@brief GUI �࣬�ṩ��ͼ����
*	@note ����ĺ����������� critical section �� */
class LCD_GUI: public LCD_Screen {
	FontBase const* pFont;	/*!< ��ǰ������ */
public:
	void Init()	{LCD_Screen::Init(); ClearScreen(); backMode = OPAQUE; SelectFont(GetSystemFont()); }

	BackGroundMode backMode;
	ColorType backColor;	/*!< ����ɫ */
	ColorType frontColor;	/*!< ǰ��ɫ */

	/**	@brief ������� */
	void FillRect(u16 xsta,u16 ysta,u16 xend,u16 yend,ColorType const register color);

	/**	@brief ������ */
	void FillRect(Rect const& r) { FillRect(r.x1,r.y1,r.x2,r.y2, frontColor); }

	/**	@brief �Ա���ɫ���������Ļ���������� */
	void ClearScreen();

	/**	@brief ��ֱ�� */
	void DrawLine(int x0,int x1,int y0,int y1);
	/**	@brief ��ֱ�ߣ��Ƽ�ʹ��������� */
	void DrawLine( Point const& p0, Point const& p1)	{DrawLine(p0.x, p0.y, p1.x, p1.y);}
	/**	@brief ������ */
	void DrawRect(Rect const * p);
	void DrawCircle(register int x0,register int y0,int r);
	
	
	void SelectFont(FontBase const * const p);
	
	/**	@brief ��ʾ�ַ� */
	void DrawChar(u16 x,u16 y,char c);
};

extern LCD_GUI lcd;

#endif
