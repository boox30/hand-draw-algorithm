#include "bezier.h"
#include "Algorithm.h"
#include <windows.h>
#include <stdio.h>
#include <math.h>

using namespace Gdiplus;
#pragma comment(lib, "gdiplus.lib")

void drawCubicBezier(HDC hdc,ALPoint b, ALPoint e, ALPoint c1, ALPoint c2);
void drawPoly(HDC hdc,AlpointsList &points);
void drawPloyGdi(HDC hdc,AlpointsList &points,COLORREF color);
void drawPolyGdiPlus(HDC hdc,AlpointsList &points,COLORREF color);
POINT alpoint2Point(ALPoint p,bool negative); 

#define RadianToAngle(r) (int)(0.5 + 180*(r/M_PI))
void drawOneBezier(HDC hdc,ALPoint b, ALPoint e, ALPoint c1,ALPoint c2){
	POINT point[4];
	point[0] = alpoint2Point(b);
	point[1] = alpoint2Point(c1);
	point[2] = alpoint2Point(c2);
	point[3] = alpoint2Point(e);
	HPEN pen =CreatePen(PS_SOLID, 1, RGB(0xa0,0xa0,0xa0));
	HPEN oldPen = (HPEN)SelectObject(hdc,pen);
	MoveToEx(hdc, point[0].x, point[0].y, NULL);
	// LineTo(hdc, point[3].x, point[3].y);
	SelectObject(hdc, oldPen);
#if 1
	drawCubicBezier(hdc,b,e,c1,c2);
#else 
	PolyBezier(hdc,point,4); 
#endif
}
#define ThroghtEveryPoint
// #define DrawPolygonMacro
void drawBezierSmoothPoly(HDC hdc, AlpointsList &points, float f, COLORREF color) {
	if(points.count()<3) return;
	GdiPlusIniter ginit;
	Graphics *graphics = Graphics::FromHDC(hdc);
	graphics->SetSmoothingMode(SmoothingModeHighQuality);
	graphics->SetCompositingMode(CompositingModeSourceOver);
	Pen pen(Color::Red);
	pen.SetLineJoin(LineJoinRound);
	pen.SetLineCap(LineCapRound,LineCapRound, DashCapRound);
// 绘制多边形
#if defined(DrawPolygonMacro)
	pen.SetColor(Color::Blue);
	pen.SetDashStyle(DashStyleDash);
	for(int i=0; i<points.count() - 1; i++){
		Point p1(points[i+0].x, -points[i+0].y);
		Point p2(points[i+1].x, -points[i+1].y);
		graphics->DrawLine(&pen, p1, p2 );
	}
#endif
#if defined(ThroghtEveryPoint)
	ALPoint b, e, n, c1, c2, c3;
	int last = 0;
	for(int i=0; i<points.count()-1; i++){
		if( distance(points[last], points[i+1])<4 ){
			continue;
		}
		b = points[last];
		e = points[i+1];
		n = points[i+2];
		c1 = last==0? points[0]:c3;
		c2 = cubicBezierControlPoint(b, e, n, &c3, f);
		{
			// 绘制控制点!!!
#if 0
			pen.SetDashStyle(DashStyleDot);
			pen.SetColor(Color::Green);
			Point p1(c2.x, -c2.y);
			Point p2(c3.x, -c3.y);
			graphics->DrawEllipse(&pen, p1.X-1, p1.Y-1,2,2);
			graphics->DrawEllipse(&pen, p2.X-1, p2.Y-1,2,2);
			graphics->DrawLine(&pen, p1, p2);
#endif
		}
		pen.SetDashStyle(DashStyleSolid);
		pen.SetColor(Color::Red);
		Point p1(b.x, -b.y);
		Point p2(c1.x, -c1.y);
		Point p3(c2.x, -c2.y);
		Point p4(e.x, -e.y);
		graphics->DrawBezier(&pen, p1,p2,p3,p4); 
		last = i+1;
	} 
#else  // 使用顶点作为控制点!!!!!
	pen.SetColor(Color::Red);
	pen.SetDashStyle(DashStyleSolid);
	int last = 0;
	float bx,by,cx,cy,ex,ey;
	for(int i=0; i<points.count(); i++){
		bx = i==0?points[0].x:ex;
		by = i==0?points[0].y:ey;
		ex = (points[i+1].x + points[i+2].x)/2;
		ey = (points[i+1].y + points[i+2].y)/2;
		cx = points[i+1].x; cy = points[i+1].y;
		// b(t) = (1-t)^2*p0 + 2t(1-t)*p1 + t^2*p2
		float x0 = bx, y0 = -by;
		for(float t=0.02; t<=1.0; t+=0.02){
			float x1 = square(1-t)*bx + 2*t*(1-t)*cx + square(t)*ex;
			float y1 = square(1-t)*by + 2*t*(1-t)*cy + square(t)*ey;
			y1 = -y1;
			graphics->DrawLine(&pen,x0, y0,x1,y1);
			x0 = x1; y0 = y1;
		}
	}

#endif
}

POINT alpoint2Point(const ALPoint &point) {
	POINT p = {point.x, -point.y};
	return p; 
}

ALPoint point2AlPoint(const POINT &point) {
	ALPoint p = {point.x, -point.y, 0}; 
	return p;
}

void drawCubicBezier(HDC hdc,ALPoint b, ALPoint e, ALPoint c1, ALPoint c2) {
	GdiPlusIniter ginit;
	Graphics *graphics = Graphics::FromHDC(hdc);
	graphics->SetSmoothingMode(SmoothingModeHighQuality);
	Point p1(b.x, -b.y);
	Point p2(c1.x, -c1.y);
	Point p3(c2.x, -c2.y);
	Point p4(e.x, -e.y);
	Pen pen(Color::Red);
	pen.SetLineJoin(LineJoinRound);
	pen.SetStartCap(LineCapRound);
	pen.SetEndCap(LineCapRound);
	graphics->DrawBezier(&pen, p1,p2,p3,p4); 
	/*
	AlPointsArray points(50,5);
	float t = 0;
	while(t <= 1) {
		ALPoint	alp = calcuBezierPoint(b,e,c1,c2,t);
		alp.y = (negative?-1:1)*alp.y;
		points.append(alp);
		t += 0.02;
	}
	drawPoly(hdc,points);
	*/
}

void drawPoly(HDC hdc,AlpointsList &points) {
	GdiPlusIniter gdiplusInit;
	if(gdiplusInit.ok()) {
		drawPolyGdiPlus(hdc,points,RGB(0xff,0x00,0x00));
	}
	else{ 
		drawPloyGdi(hdc,points,RGB(0xff,0x00,0x00));
	}
}

void drawPloyGdi(HDC hdc,AlpointsList &points,COLORREF color) {
	if(points.count() < 2) return;
	MoveToEx(hdc, points[0].x, points[0].y, NULL);
	HPEN pen = CreatePen(PS_SOLID, points[0].w, color);
	HGDIOBJ oldPen = SelectObject(hdc, CreatePen(PS_SOLID, points[0].w, color));
	MoveToEx(hdc, points[0].x, points[0].y, NULL);
	for(int i=1; i<points.count(); i++) {
		LineTo(hdc, points[i].x, points[i].y);
		DeleteObject(SelectObject(hdc, CreatePen(PS_SOLID, points[i].w, color))); 
	} 
	DeleteObject(SelectObject(hdc, oldPen));
}

void drawPolyGdiPlus(HDC hdc,AlpointsList &points,COLORREF c) {
	if(points.count() < 2) return;
	Graphics graphics(hdc);
	graphics.SetSmoothingMode(SmoothingModeHighQuality);
	Color color(GetRValue(c), GetGValue(c), GetBValue(c));
	Pen pen(color);
	pen.SetLineJoin(LineJoinRound);
	pen.SetStartCap(LineCapRound);
	pen.SetEndCap(LineCapRound);
	for(int i=1; i<points.count(); i++) {
		pen.SetWidth(points[i-1].w);
		Point b(points[i-1].x, points[i-1].y);
		Point e(points[i].x, points[i].y);
		graphics.DrawLine(&pen,b,e);
	} 
}

GdiPlusIniter::GdiPlusIniter()
{
	Gdiplus::GdiplusStartupInput StartupInput;  
	m_ok = GdiplusStartup(&m_gdiplusToken,&StartupInput,NULL);
}

GdiPlusIniter::~GdiPlusIniter()
{
	Gdiplus::GdiplusShutdown(m_gdiplusToken);
}

bool GdiPlusIniter::ok()
{
	return m_ok==Status::Ok;
}
