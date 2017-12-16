/*
 * =====================================================================================
 *       Filename:  z_gdi_render.cpp
 *    Description:  
 *        Version:  2.0
 *        Created:  06/23/2016 14:53:43
 *       Revision:  none
 *         Author:  zl(88911562@qq.com), 
 *   Organization:  
 * =====================================================================================
 */
#include "z_gdi+_render.h"
#pragma comment(lib, "gdiplus.lib")

#include <stdio.h>

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
	return m_ok == Status::Ok ? true : false;
}

Graphics * GdiPlusIniter::get_graphics_from_hdc(HDC hdc)
{
	Graphics *graphics = Graphics::FromHDC(hdc);
	graphics->SetCompositingQuality(CompositingQualityHighQuality);
	graphics->SetSmoothingMode(SmoothingModeAntiAlias);
	return graphics;
}

Z_HandwritingRender::Z_HandwritingRender(float min_pen_width, float max_pen_width):
	m_w_min(min_pen_width), m_w_max(max_pen_width)
{
	m_arr_list = z_new_fpoint_arraylist(); 
}

Z_HandwritingRender::~Z_HandwritingRender()
{ 
	if (m_mem_hdc) DeleteDC(m_mem_hdc);
	if (m_mem_bmp) DeleteBitmap(m_mem_bmp);

	if (m_cur_path) z_drop_fpoint_array(m_cur_path);
	if (m_arr_list) z_drop_fpoint_arraylist(m_arr_list);
}

void Z_HandwritingRender::init_memHdc(unsigned int cx, unsigned int cy)
{ 
	HDC hdc_src = GetDC(NULL); 
	m_cx = cx; m_cy = cy;
	m_mem_bmp = CreateCompatibleBitmap(hdc_src, cx, cy);
	m_mem_hdc = CreateCompatibleDC(hdc_src); 
	SelectObject(m_mem_hdc, m_mem_bmp);
	SelectObject(m_mem_hdc, GetStockBrush(WHITE_BRUSH));
	Rectangle(m_mem_hdc, 0, 0, cx, cy);
	ReleaseDC(NULL, hdc_src);
}

void Z_HandwritingRender::copy_to_hdc(HDC hdc, RECT r)
{
	if (!m_mem_hdc) return; 
	// todo:这里可能应该判断一下rect的范围...!!
	BitBlt(hdc, r.left, r.top, r.right - r.left, r.bottom - r.top, m_mem_hdc, r.left, r.top, SRCCOPY); 
}

void Z_HandwritingRender::insert_first(int x, int y)
{
	z_point point = { (float)x, (float)y };
	if (m_cur_path) 
		z_drop_fpoint_array(m_cur_path); 

	m_cur_path = z_fpoint_arraylist_append_new(m_arr_list, m_w_max, m_w_min);

	z_insert_point(m_cur_path, point); 
}

void Z_HandwritingRender::insert(int x, int y)
{ 
	z_point point = { (float)x, (float)y };
	if(!m_cur_path) 
		m_cur_path = z_fpoint_arraylist_append_new(m_arr_list, m_w_max, m_w_min);

	z_insert_point(m_cur_path, point);
}

void Z_HandwritingRender::insert_last(int x, int y)
{
	if (!m_cur_path)
		return;
	z_point point = { (float)x, (float)y };
	z_insert_last_point(m_cur_path, point);

	z_drop_fpoint_array(m_cur_path);
	m_cur_path = NULL;
}

bool Z_HandwritingRender::draw_to_hdc(HDC hdc, RECT *o_rect)
{
	HDC render_hdc = NULL; 
	if (m_mem_hdc) render_hdc = m_mem_hdc; 
	else render_hdc = hdc;
	
	if (!render_hdc) {
		printf("there are no hdc to render!!\n");
		return false;
	}

	if (!m_arr_list || !m_arr_list->first) return false;

	z_fpoint_arraylist_node *node = m_arr_list->cur;

// 	if (node) node = node->n;
// 	else node = m_arr_list->first; 

	RECT rect;
	SetRectEmpty(&rect);

	GdiPlusIniter gdipluseIniter;
	Graphics *graphics = gdipluseIniter.get_graphics_from_hdc(render_hdc); 

	Pen pen(Color::Red);
	pen.SetLineJoin(LineJoinRound);
	pen.SetStartCap(LineCapRound);
	pen.SetEndCap(LineCapRound); 

	if (!gdipluseIniter.ok()) {
		printf("something wrong with gdi+???, I can't draw path to hdc!!!\n");
		return false;
	}

	int l, t, r, b;
	float fx, fy, tx, ty, fw, tw;
	z_fpoint_array *path = NULL;
	z_fpoint *p = NULL, *last = NULL;

	// 只绘制 新产生的这一部分笔迹, 
	// 由于算法中的y,正向为从下到上, 所以y都需要变成-y
	while (node) {
		m_arr_list->cur = node;

		path = node->a; 
		p = path->point + m_cur_last_index;
		last = path->point + (path->len - 1);

		if (IsRectEmpty(&rect)) {
			SetRect(&rect, p->p.x, p->p.y, p->p.x, p->p.y);
		}

		while ( p < last ) { 
			pen.SetWidth(p->w * m_w_max);

			fx = p->p.x; fy = -p->p.y; fw = p->w;  p++;
			tx = p->p.x; ty = -p->p.y; tw = p->w;

			graphics->DrawLine(&pen, fx, fy, tx, ty); 

			printf("draw line from (%04f, %04f, %04f), (%04f, %04f, %04f)\n", 
				fx, fy, fw,  tx, ty, tw);

#pragma warning(disable:4244)
			if (fx < tx) {
				l = fx; r = tx;
			}
			else {
				r = fx; l = tx;
			}

			if (fy < ty) {
				t = fy; b = ty;
			}
			else {
				b = fy; t = ty;
			}
#pragma warning(default:4244) 
			RECT t_rect = { l, t, r, b };
			UnionRect(&rect, &rect, &t_rect);
		};

		node = node->n;
		m_cur_last_index = 0;
	}; 

	m_arr_list->cur = m_arr_list->end;
	if (o_rect) memcpy(o_rect, &rect, sizeof(RECT)); 
	if (m_mem_hdc && !IsRectEmpty(&rect)) {
// 		BitBlt(hdc, l, t, r - l, b - t, m_mem_hdc, l, t, SRCCOPY); 
// 		BitBlt(hdc, 0, 0, m_cx, m_cy, m_mem_hdc, 0, 0, SRCCOPY); 
	}
	return true; 
}
