// bezier.cpp : 定义应用程序的入口点。
//

#include "bezier.h"
#include "Algorithm.h"
#include <windows.h>
#include <stdio.h>
#include <math.h>

#define RadianToAngle(r) (int)(0.5 + 180*(r/M_PI))
#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名

// 此代码模块中包含的函数的前向声明:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
void OnPaintTest(HDC hdc);

int APIENTRY wWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
 	// TODO: 在此放置代码。
	MSG msg;
	HACCEL hAccelTable;
	// 初始化全局字符串
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_BEZIER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);
	// 执行应用程序初始化:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}
	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_BEZIER));
	// 主消息循环:
	while (GetMessage(&msg, NULL, 0, 0)) {
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	} 
	return (int) msg.wParam;
} 
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BEZIER));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_BEZIER);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	return RegisterClassEx(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // 将实例句柄存储在全局变量中

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: 处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 分析菜单选择:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:{
		hdc = BeginPaint(hWnd, &ps);
		OnPaintTest(hdc);
		// TODO: 在此添加任意绘图代码...
		EndPaint(hWnd, &ps);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}


int main(int argcount, void *args){
	HINSTANCE instance = GetModuleHandle(NULL);
	return wWinMain(instance,NULL,NULL,SW_SHOW);
}

void OnPaintTest(HDC hdc) {
	AlPoint lefttop = {100, 100};
	int length = 200;
	Points points(20, 5);
	// 首先假设只有3个点!!!!!
	points.append(200, 90);
	points.append(340, 170);
	points.append(360, 260);
	points.append(300, 340);
	points.append(100, 180);
	points.append(250, 200);
	drawBezier(hdc, points, 0.3, RGB(0xff,0,0), 1, 3);
}

POINT ALPointToPoint(AlPoint p,bool negative)
{
	int n = negative?-1:1;
	POINT r = {(long)p.x,n*(long)p.y};
	return r;
}

void drawOneBezier(HDC hdc,AlPoint b, AlPoint e, AlPoint c1,AlPoint c2,bool negative){
	POINT point[4];
	point[0] = ALPointToPoint(b, negative);
	point[1] = ALPointToPoint(c1,negative);
	point[2] = ALPointToPoint(c2,negative);
	point[3] = ALPointToPoint(e, negative);
	HPEN pen =CreatePen(PS_SOLID, 1, RGB(0xa0,0xa0,0xa0));
	HPEN oldPen = (HPEN)SelectObject(hdc,pen);
	MoveToEx(hdc, point[0].x, point[0].y, NULL);
	LineTo(hdc, point[3].x, point[3].y);
	SelectObject(hdc, oldPen);
#if 1
	myPolyBezier(hdc,b,e,c1,c2,negative);
#else 
	PolyBezier(hdc,point,4); 
#endif
}

void drawBezier(HDC hdc, Points &points, float f, COLORREF color, int width, int d) {
	HPEN hPen = CreatePen(PS_SOLID, width, color);
	HGDIOBJ hobj = SelectObject(hdc, hPen);
	AlPoint b, e, n, c1, c2, c3;
	c3 = points[0];
	c3.y = - c3.y;
	for(int i=2; i<points.count(); i++){
		b = points[i-2];
		e = points[i-1];
		n = points[i];
		c1 = c3; b.y = -b.y; e.y = -e.y; n.y = -n.y;
		c2 = bezierControlPoint(b, e, n, &c3, 0.2);
		drawOneBezier(hdc,b,e,c1,c2,true);
	}
	b = points[points.count()-2];
	e = points[points.count()-1];
	c1 = c3; c1.y = -c1.y; c2 = e;
	drawOneBezier(hdc,b,e,c1,c2,false);
	DeleteObject( SelectObject(hdc,hobj) );
}

void myPolyBezier(HDC hdc,AlPoint b, AlPoint e, AlPoint c1, AlPoint c2,bool negative) {
	float t = 0.02;
	MoveToEx(hdc, b.x, (negative?-1:1)*b.y, NULL);
	while(t <= 1){
		AlPoint	alp = bezierPoint(b,e,c1,c2,t);
		alp.y = (negative?-1:1)*alp.y;
		t += 0.02;
		LineTo(hdc,alp.x,alp.y);
	}
}

// b(t) = p0*(1-t)^3 + 3p1*t*(1-t)^2 + 3*p2*t^2(1-t) + p3*t^3
float p0(float v, float t){ return v * cubic(1-t); }
float p1(float v, float t){ return 3 * v * t * square( 1-t ); }
float p2(float v, float t){ return 3 * v * square(t) * (1-t); }
float p3(float v, float t){ return v * cubic(t); }
AlPoint bezierPoint(AlPoint b, AlPoint e, AlPoint c1, AlPoint c2, float t){
	AlPoint point = {
		p0(b.x,t) + p1(c1.x,t) + p2(c2.x,t) + p3(e.x,t),
		p0(b.y,t) + p1(c1.y,t) + p2(c2.y,t) + p3(e.y,t)
	};
	return point;
}