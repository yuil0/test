//YUIL_2021
#ifndef _CDraw_h_
#define _CDraw_h_

#include <windows.h>

enum class ETypeFig
{
	CIRCLE = 0,
	SQUARE,
	RECTANLE,
	TRAPEZ,
	//
	QUANTITY,
};

enum class EMethod
{
	BY_CIRCLE = 0,
	RANDOM,
	//
	QUANTITY,
};

class CDraw
{
	void DrawFig(ETypeFig tf, HDC hdc, const RECT& rc);
	void DrawByCircle(ETypeFig tf, HDC hdc, const RECT& rc, unsigned quantity);
	void DrawRandom(ETypeFig tf, HDC hdc, const RECT& rc, unsigned quantity);
	public:
	CDraw(HWND hwnd, int StartY, ETypeFig tf, unsigned quantity, long color, EMethod eMethod);
	~CDraw();
};

#endif