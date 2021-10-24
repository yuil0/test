//YUIL_2021
#include <windows.h>
#include <winbase.h>
#include <stdio.h>
#include "CDraw.h"
#include "mes.h"

HINSTANCE               g_hInst = NULL;
HWND                    g_hWnd = NULL;
int gStartY;
#define TEST_YUIL L"TEST_YUIL"

typedef enum
{
	EIC_COMBO = 0,
	EIC_LABEL_QUANTITY,
	EIC_EDIT_QUANTITY,
	EIC_LABEL_COLOR,
	EIC_EDIT_COLOR,
	EIC_LABEL_METHOD,
	EIC_EDIT_METHOD,
	EIC_COMBO_METHOD,
	EIC_BUT_DRAW,
}EIdChild;

void _WM_KEYDOWN(WPARAM wParam, LPARAM lParam)
{
	int vk = wParam;
	switch (vk)
	{
	case VK_ESCAPE: DestroyWindow(g_hWnd);
	}
}

int GetCurSelCombo(HWND hwnd, int id) { return SendDlgItemMessageA(hwnd, id, CB_GETCURSEL, 0, (LPARAM)0); }


void _WM_COMMAND(WPARAM wParam, LPARAM lParam)
{
	int id = LOWORD(wParam);
	switch (id)
	{
		case EIC_BUT_DRAW: 
		{
			int ind= GetCurSelCombo(g_hWnd, EIC_COMBO);
			BOOL fTrans;
			BOOL fSigned = FALSE;			
			unsigned quantity = GetDlgItemInt(g_hWnd, EIC_EDIT_QUANTITY, &fTrans, fSigned);
			if (!fTrans) { mes("Ошибка получения значения количества"); return; }			

			const unsigned cuMax = 10000;
			if (quantity > cuMax) 
			{ 
				mes("Указано количество выше возможного"); 
				SetDlgItemInt(g_hWnd, EIC_EDIT_QUANTITY, cuMax, fSigned);
				return; 
			}


			const int cLenColor = 8;
			char bufColor[cLenColor +1]; //YUIL_2021. +1 нуль терминатор.
			GetDlgItemTextA(g_hWnd, EIC_EDIT_COLOR, bufColor, cLenColor+1);
			long color;
			sscanf(bufColor, "%x", &color);			
			int indM = GetCurSelCombo(g_hWnd, EIC_COMBO_METHOD);

			CDraw cDraw(g_hWnd, gStartY, (ETypeFig)ind, quantity, color, (EMethod)indM);
		}		
		break;
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:PostQuitMessage(0);break;
	case WM_KEYDOWN:_WM_KEYDOWN(wParam, lParam); break;
	case WM_COMMAND:_WM_COMMAND(wParam, lParam); break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

HWND CrChild(HINSTANCE hInstance, HWND hParentWnd, const char* _class, const char* _text, int id, RECT rc, DWORD add_type = 0);
HWND CrChild(HINSTANCE hInstance, HWND hParentWnd, const char* _class, const char* _text, int id, RECT rc, DWORD add_type)
{
	DWORD type = WS_CHILD | WS_VISIBLE| add_type;
	HWND hwnd= CreateWindowA(_class, _text, type, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, hParentWnd, (HMENU)id, hInstance, NULL);
	return hwnd;
}

void AddStrCombo(HWND hwndCombo, const char* s) {SendMessageA(hwndCombo, CB_ADDSTRING, 0, (LPARAM)s);}
void ComboSetCursel(HWND hwndCombo, int index) { SendMessageA(hwndCombo, CB_SETCURSEL, index, 0); }

void CrCtrls(HINSTANCE hInstance, HWND hWnd)
{
	const int cHeightRow= 26;
	const int cHeightCtrl = 22;
	{
		RECT rcCombo = { 4,0,200,cHeightCtrl * 5 };
		HWND hwnd = CrChild(hInstance, hWnd, "combobox", "", EIC_COMBO, rcCombo, CBS_DROPDOWNLIST);
		if (hwnd)
		{
			AddStrCombo(hwnd, "Круг");
			AddStrCombo(hwnd, "Квадрат");
			AddStrCombo(hwnd, "Прямоугольник");
			AddStrCombo(hwnd, "Трапеция");
			ComboSetCursel(hwnd, 0);
		}
	}
	{
		RECT rc = { 4,1 * cHeightRow,100,1 * cHeightRow+ cHeightCtrl };
		HWND hwnd = CrChild(hInstance, hWnd, "static", "Количество", EIC_LABEL_QUANTITY, rc, SS_CENTER);
		RECT rcE = { 4 + 1 * 100,1 * cHeightRow,2 * 100,1 * cHeightRow + cHeightCtrl };
		HWND hwndE = CrChild(hInstance, hWnd, "edit", "10", EIC_EDIT_QUANTITY, rcE, WS_BORDER);
	}
	{
		RECT rc = { 4,2 * cHeightRow,100,2 * cHeightRow+ cHeightCtrl };
		HWND hwnd = CrChild(hInstance, hWnd, "static", "Цвет", EIC_LABEL_COLOR, rc, SS_CENTER);
		RECT rcE = { 4 + 1 * 100,2 * cHeightRow,2 * 100,2 * cHeightRow + cHeightCtrl };
		HWND hwndE = CrChild(hInstance, hWnd, "edit", "0x00FF00", EIC_EDIT_COLOR, rcE, WS_BORDER);
	}
	{
		RECT rc = { 4,3 * cHeightRow,100,3 * cHeightRow+ cHeightCtrl };
		HWND hwnd = CrChild(hInstance, hWnd, "static", "Метод", EIC_LABEL_METHOD, rc, SS_CENTER);
		RECT rcCombo = { 4 + 1 * 100,3 * cHeightRow,2 * 100,3 * cHeightRow +4* cHeightCtrl };
		HWND hwndE = CrChild(hInstance, hWnd, "combobox", "", EIC_COMBO_METHOD, rcCombo, CBS_DROPDOWNLIST);
		if (hwndE)
		{
			AddStrCombo(hwndE, "По кругу");
			AddStrCombo(hwndE, "Случайно");
			ComboSetCursel(hwndE, 0);
		}
	}
	{
		RECT rc = { 5,4 * cHeightRow,100,4 * cHeightRow+ cHeightCtrl };
		HWND hwnd = CrChild(hInstance, hWnd, "button", "Отобразить", EIC_BUT_DRAW, rc, 0);
		gStartY=rc.bottom+ cHeightCtrl;
	}
}

HRESULT InitWindow(HINSTANCE hInstance)
{
	// Регистрация класса
	WNDCLASSEX wcex; memset(&wcex, 0, sizeof(WNDCLASSEX));
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = TEST_YUIL; //wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_ICON1);
	int err = 0;
	if (!RegisterClassEx(&wcex))
	{
		err = GetLastError();		
		return E_FAIL;
	}
		

	// Создание окна
	g_hInst = hInstance;
	RECT rc = { 0, 0, 640, 480 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	g_hWnd = CreateWindow(TEST_YUIL, TEST_YUIL, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance, NULL);
	if (!g_hWnd)
		return E_FAIL;
	
	CrCtrls(hInstance, g_hWnd);

	ShowWindow(g_hWnd, SW_SHOW);
	return S_OK;
}

int main(int argc, char** argv)
//int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	HINSTANCE hInstance = GetModuleHandleA(0);
	if (InitWindow(hInstance)!=S_OK) { return 0; }

	// Главный цикл сообщений
	MSG msg = { 0 };
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else // Если сообщений нет
		{
			if (GetAsyncKeyState(VK_ESCAPE)) {				DestroyWindow(g_hWnd);			}
		}
	}

	return 1;
}
