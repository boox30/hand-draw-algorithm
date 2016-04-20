#pragma once

#include "resource.h"
#include <windows.h>
#include "Algorithm.h"
POINT alpointToPoint(ALPoint p,bool negative); 
void drawBezier(HDC hdc, ALArrPoints &points, float f, COLORREF color, int width,int d);
void alPolyBezier(HDC hdc,ALPoint b, ALPoint e, ALPoint c1, ALPoint c2,bool negative);
ALPoint bezierPoint(ALPoint b, ALPoint e, ALPoint c1, ALPoint c2, float t);
void aldrawPoly(HDC hdc,ALArrPoints &points);
void gdiDrawPoly(HDC hdc,ALArrPoints &points,COLORREF color);
void gdiplusDrawPoly(HDC hdc,ALArrPoints &points,COLORREF color);