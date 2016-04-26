#pragma once

#include "resource.h"
#include <windows.h>
#include <GdiPlus.h>
#include "Algorithm.h"
void drawBezierSmoothPoly(HDC hdc, AlpointsList &points, float f, COLORREF color);
POINT   alpoint2Point(const ALPoint &point);
ALPoint point2AlPoint(const POINT &point);

class GdiPlusIniter{
public:
	GdiPlusIniter();
	~GdiPlusIniter();
	bool ok();
private:
	ULONG_PTR m_gdiplusToken;
	Gdiplus::Status m_ok;
}; 

