//YUIL_2021
#include <math.h>
#include <time.h>
#include "CDraw.h"
#include "mes.h"

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

//#define MES_DESTRUCT

class CFig
{
public:
	virtual void Draw(HDC hdc, RECT rc)=0;
	CFig()
	{		
	}
	virtual ~CFig() 
	{
  #ifdef MES_DESTRUCT
		mes("~CFig()"); 
  #endif
	}
};

class CCircle :public CFig
{
public:
	CCircle(){}
	~CCircle() 
	{
  #ifdef MES_DESTRUCT
		mes("~CCircle()"); 
  #endif
	}
	void Draw(HDC hdc, RECT rc)
	{
		Ellipse(hdc, rc.left, rc.top, rc.right, rc.bottom); //Polygon(hdc, const POINT * apt,int         cpt);
	}
};

class CSquare :public CFig
{
public:
	CSquare() {}
	~CSquare() 
	{
  #ifdef MES_DESTRUCT
		mes("~CSquare()"); 
  #endif
	}
	void Draw(HDC hdc, RECT rc)
	{
		Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom); //Polygon(hdc, const POINT * apt,int         cpt);
	}
};

class CRectanle :public CFig
{
public:
	CRectanle() {}
	~CRectanle() 
	{
  #ifdef MES_DESTRUCT
		mes("~CRectanle()"); 
  #endif
	}
	void Draw(HDC hdc, RECT rc)
	{
		float centerY = (rc.bottom + rc.top) / 2;
		float rad = (rc.bottom - rc.top) / 2;
		const float coef = 0.7;
		Rectangle(hdc, rc.left, centerY - rad * coef, rc.right, centerY + rad * coef); //Polygon(hdc, const POINT * apt,int         cpt);
	}
};

class CTrapez :public CFig
{
public:
	CTrapez() {}
	~CTrapez() 
	{
  #ifdef MES_DESTRUCT
		mes("~CTrapez()"); 
  #endif
	}
	void Draw(HDC hdc, RECT rc)
	{
		float centerY = (rc.bottom + rc.top) / 2;
		float radY = (rc.bottom - rc.top) / 2;
		const float coefY = 0.6;
		float yMin = centerY - radY * coefY;
		float yMax = centerY + radY * coefY;

		float centerX = (rc.right + rc.left) / 2;
		float radX = (rc.right - rc.left) / 2;
		const float coefX = 0.8;
		float xMin = centerX - radX * coefX;
		float xMax = centerX + radX * coefX;

		const int cPoints = 4;
		POINT pt[cPoints] = { {rc.left, yMax}, {rc.right, yMax} , {xMax, yMin} , {xMin, yMin} };
		Polygon(hdc, pt, cPoints);
	}
};


CDraw::CDraw(HWND hwnd, int StartY, ETypeFig tf, unsigned quantity, long color, EMethod eMethod)
{
	HDC hdc = GetDC(hwnd);
	RECT rc;
	if (!GetClientRect(hwnd, &rc)) { return; }
	rc.top = StartY;
	HPEN hpenWhite = (HPEN)GetStockObject(WHITE_PEN);
	SelectObject(hdc, hpenWhite);
	Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);

	HPEN hpen = CreatePen(PS_SOLID, 0, color);
	HBRUSH hbrush = CreateSolidBrush(color);
	SelectObject(hdc, hpen);
	SelectObject(hdc, hbrush);

	switch(eMethod)
	{ 
	 case EMethod::BY_CIRCLE: DrawByCircle(tf, hdc, rc, quantity); break;
		case EMethod::RANDOM: DrawRandom(tf, hdc, rc, quantity); break;
	}

	DeleteObject(hpen);
	DeleteObject(hbrush);

	ReleaseDC(hwnd, hdc);
}

CDraw::~CDraw()
{
}

void CDraw::DrawFig(ETypeFig tf, HDC hdc, const RECT& rc)
{
	CFig* pFig = 0;
	switch (tf)
	{
	case ETypeFig::CIRCLE: pFig = new CCircle(); break;
	case ETypeFig::SQUARE: pFig = new CSquare(); break;
	case ETypeFig::RECTANLE: pFig = new CRectanle(); break;
	case ETypeFig::TRAPEZ: pFig = new CTrapez(); break;
	}
	if (!pFig) { mes("Ошибка выделения памяти"); return; }
	
	pFig->Draw(hdc, rc);

	delete pFig;
}

void CDraw::DrawByCircle(ETypeFig tf, HDC hdc, const RECT& rc, unsigned quantity)
{
	POINT center = { (rc.left + rc.right) / 2,  (rc.top + rc.bottom) / 2 };
	POINT rad= { (rc.right- rc.left) / 2,  (rc.bottom - rc.top) / 2 };
	float size = rad.y / 20; 
	rad.y -= size;

	float anStep = 2*M_PI/ quantity;
	float an = 0;
	while (an< 2 * M_PI)
	{
		float x = center.x + rad.y * cos(an);
		float y = center.y + rad.y * sin(an);
		RECT rcE = { x - size,y - size,x + size,y + size };
		
		DrawFig(tf, hdc, rcE);

		an += anStep;
	}
}

void CDraw::DrawRandom(ETypeFig tf, HDC hdc, const RECT& rc, unsigned quantity)
{
	srand(clock());
	float size = ((rc.bottom - rc.top) / 2) / 20;
	for (unsigned i = 0; i < quantity; ++i)
	{
		float x = rc.left+ size +(rc.right- rc.left -2* size)* ((float)rand() / (float)RAND_MAX);
		float y = rc.top + size+(rc.bottom - rc.top-2* size) * ((float)rand() / (float)RAND_MAX);
		RECT rcE = { x - size,y - size,x + size,y + size };

		DrawFig(tf, hdc, rcE);
	}
}