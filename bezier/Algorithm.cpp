#include "Algorithm.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
/*
#include <GdiPlus.h>
using namespace Gdiplus;
#pragma comment(lib, "gdiplus.lib")
*/
#define min(a,b) (a<b?a:b)
float square(float f) { return f*f; }
float cubic(float f) { return f * square(f); } 

float distance(ALPoint p1, ALPoint p2) {
	return sqrt( square(p1.x-p2.x) + square(p1.y-p2.y) );
}
AlpointsList::AlpointsList(int initSize, int increaseSize) {
	m_count = 0;
	m_capability = initSize<=0 ? 1:initSize;
	m_increaseSize = increaseSize;
	int memSize = initSize*sizeof(ALPoint);
	m_points = (ALPoint*)malloc(memSize);
	memset(m_points, 0, memSize);
}

AlpointsList::~AlpointsList() {
	if(m_points) free(m_points); 
}

bool AlpointsList::append(ALPoint point) {
	if(m_capability==m_count){
		if(!increase()){
			return false; 
		}
	}
	m_points[m_count++] = point;
	return true;
}

bool AlpointsList::append(float x,float y) {
	ALPoint point = {x, y, 0.1};
	return append(point); 
}

bool AlpointsList::append(float x,float y,float w) {
	ALPoint point = {x, y, w};
	return append(point); 
}

ALPoint AlpointsList::pup() {
	ALPoint point = {0,0,0};
	if( 0==m_count ) return point;
	point = m_points[0];
	for(int i=0; i<(m_count-1); i++){
		m_points[i] = m_points[i+1];
	}
	m_count--;
	return point; 
}

ALPoint * AlpointsList::getPoint(int index) {
	if(index >= m_count)
		return NULL;
	return m_points + index;
}

ALPoint& AlpointsList::operator[](unsigned int i) {
	if(!m_points){ 
		int size = sizeof(ALPoint)*m_capability;
		m_points = (ALPoint*)malloc(size);
		memset(m_points, 0, size); 
	}
	if( i>= m_count ){
		i = m_count-1;
		 std::clog<<"the index over flow, reset index to count-1"<<std::endl;
	}
	return m_points[i];
}

bool AlpointsList::increase() {
	ALPoint *tmp = (ALPoint*)realloc(m_points, (m_count+m_increaseSize)*sizeof(ALPoint)); 
	if(tmp) {
		m_capability += 5;
		m_points = tmp;
	}
	return tmp ? true:false;
}

ALPoint cubicBezierControlPoint(ALPoint b,ALPoint e,ALPoint n,ALPoint *c,float f) {
#if 1
	// 计算AB,BC的中点坐标,连接后记为线段M
	float xm1 = (e.x + b.x)/2;
	float ym1 = (e.y + b.y)/2;
	float xm2 = (n.x + e.x)/2;
	float ym2 = (n.y + e.y)/2;
	// 计算线段AB,BC的长度,分别记为l1,l2
	float l1 = distance(b, e);
	float l2 = distance(e, n);
	float k = l1/(l2+l1);
	// 把线段M,以l1:l2的比例分割,分割点记为T
	float xt = xm1 + (xm2-xm1) * k;
	float yt = ym1 + (ym2-ym1) * k;
	// 将线段M平行移动到顶点B,将线段上的分割点T对其到B,
	// 线段的两个端点就是控制点!!!!,加上平滑因子f,计算控制点!!!
	float xc1 = e.x - (xt - xm1) * f;
	float yc1 = e.y - (yt - ym1) * f;
	float xc2 = e.x + (xm2 - xt) * f;
	float yc2 = e.y + (ym2 - yt) * f;
	ALPoint ctrlPoint = {xc1, yc1, 0};
	if( c ){
		c->x = xc2; c->y = yc2;
	}
	return ctrlPoint;
 #else
	// 取角平分线的垂线上的点作为控制点的算法!!!!
	float a1 = atan2f(e.y - b.y, e.x - b.x);
	float a2 = atan2f(e.y - n.y, e.x - n.x);
	float r = tan((a1+a2)/2 + M_PI_2);
	AlLine line(r, e); 
	float d = distance(b, e);
	d = d * f;
	ALPoint *ps = line.distancePointsTo(e.x, d);
	d = distance(ps[0], e);
	ALPoint point = ( distance(b, ps[0]) < distance(b, ps[1]) ) ?  ps[0]:ps[1];
	free(ps);
	if(c) {
		d = distance(e, n) * f;
		ps = line.distancePointsTo(e.x, d);
		*c = ( distance(n, ps[0]) < distance(n, ps[1]) ) ? ps[0]:ps[1];
		free(ps);
	}
	return point;
#endif
}

AlLine::AlLine(float radian, ALPoint point) {
	m_r = radian;
	m_point = point;
}

AlLine::AlLine(ALPoint p1, ALPoint p2) {
	m_r = (p1.y-p2.y) / (p1.x-p2.x);
	m_point = p1;
} 

ALPoint* AlLine::distancePointsTo(float x, float dist) {
	float y = m_r*(x-m_point.x) + m_point.y;
	ALPoint *point = (ALPoint*)malloc(sizeof(ALPoint)*2);
	float t = sqrtf(square(dist)/(1+square(m_r)));
	point[0].x = x + t;
	point[1].x = x - t;
	point[0].y = m_r * (point[0].x-x) + y;
	point[1].y = m_r * (point[1].x-x) + y;
	return point;
}

// b(t) = p0*(1-t)^3 + 3p1*t*(1-t)^2 + 3*p2*t^2(1-t) + p3*t^3
float p0(float v, float t){ return v * cubic(1-t); }
float p1(float v, float t){ return 3 * v * t * square( 1-t ); }
float p2(float v, float t){ return 3 * v * square(t) * (1-t); }
float p3(float v, float t){ return v * cubic(t); }
ALPoint calcuBezierPoint(ALPoint b, ALPoint e, ALPoint c1, ALPoint c2, float t){
	ALPoint point = {
		p0(b.x,t) + p1(c1.x,t) + p2(c2.x,t) + p3(e.x,t),
		p0(b.y,t) + p1(c1.y,t) + p2(c2.y,t) + p3(e.y,t),
		b.w + (e.w-b.w)*t
	};
	return point;
}
