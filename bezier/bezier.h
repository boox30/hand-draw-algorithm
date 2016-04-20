#pragma once

#include "resource.h"
#include <windows.h>
#include "Algorithm.h"
POINT ALPointToPoint(AlPoint p,bool negative); 
void drawBezier(HDC hdc, Points &points, float f, COLORREF color, int width,int d);
void myPolyBezier(HDC hdc,AlPoint b, AlPoint e, AlPoint c1, AlPoint c2,bool negative);
AlPoint bezierPoint(AlPoint b, AlPoint e, AlPoint c1, AlPoint c2, float t);
