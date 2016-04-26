#include "bezier.h"
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <iostream>
using namespace Gdiplus;
#define MAX_LOADSTRING 100
// 全局变量:
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名
AlpointsList g_points(200, 100);
// 此代码模块中包含的函数的前向声明:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
void OnPaintTest(HDC hdc);
void OnMouseMove(WPARAM wparam, LPARAM lparam);


HWND g_hwnd = NULL;
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
	case WM_PAINT: {
		hdc = BeginPaint(hWnd, &ps);
		OnPaintTest(hdc);
		EndPaint(hWnd, &ps);
		}
		break;
	case WM_CREATE:{
		g_hwnd = hWnd;
				   }
				   break;
	case WM_MOUSEMOVE:{
		OnMouseMove(wParam, lParam);
					  }
					  break;
	case WM_LBUTTONDOWN:{
		g_points.setEmpty();
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
#if 0
	GdiPlusIniter gdiplus;
	if(!gdiplus.ok()) {
		std::cout<<"gdi+ init faild!!!!!"<<std::endl;
		return;
	}
	Graphics graphics(hdc);
	graphics.SetSmoothingMode(SmoothingModeHighQuality);
	Pen pen(Color::Red);
	pen.SetWidth(2);
	pen.SetLineJoin(LineJoinRound);
	pen.SetLineCap(LineCapRound, LineCapRound, DashCapRound);
	for(int i=0; i<g_points.count(); i++){
		Point b((int)g_points[i].x,  (int)-g_points[i].y);
		Point e((int)g_points[i+1].x,(int)-g_points[i+1].y);
		graphics.DrawLine(&pen, b, e);
	}
#else
	/*
	g_points.setEmpty();
	g_points.append(100,-140);
	g_points.append(150,-200);
	g_points.append(135,-140);
	g_points.append(170,-180);
	g_points.append(250,-200);
	g_points.append(100,-100);
	g_points.append(190,-80);
	g_points.append(200,-150);
	g_points.append(270,-70);
	g_points.append(300,-200);
	g_points.append(400,-170);
	g_points.append(240,-350);
	//*/
	drawBezierSmoothPoly(hdc,g_points,1,RGB(0xff,0,0));
#endif
}

void OnMouseMove(WPARAM wparam, LPARAM lparam) {
	if( (wparam & MK_LBUTTON)!=MK_LBUTTON ) {
		return;
	}
	g_points.append(GET_X_LPARAM(lparam), -GET_Y_LPARAM(lparam), 1);
	InvalidateRect(g_hwnd, NULL, true);
}
