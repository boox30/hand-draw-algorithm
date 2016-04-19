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
float distance(AlPoint p1, AlPoint p2) {
	return sqrt( square(p1.x-p2.x) + square(p1.y-p2.y) );
}
Points::Points(int initSize, int increaseSize) {
	m_count = 0;
	m_capability = initSize<=0 ? 1:initSize;
	m_increaseSize = increaseSize;
	int memSize = initSize*sizeof(AlPoint);
	m_points = (AlPoint*)malloc(memSize);
	memset(m_points, 0, memSize);
}

Points::~Points() {
	if(m_points) free(m_points); 
}

bool Points::append(AlPoint point) {
	if(m_capability==m_count){
		if(!Increase()){
			return false; 
		}
	}
	m_points[m_count++] = point;
	return true;
}

bool Points::append(float x,float y) {
	AlPoint point = {x, y};
	return append(point); 
}

AlPoint * Points::getPoint(int index) {
	if(index >= m_count)
		return NULL;
	return m_points + index;
}

AlPoint& Points::operator[](unsigned int i) {
	if(!m_points){ 
		int size = sizeof(AlPoint)*m_capability;
		m_points = (AlPoint*)malloc(size);
		memset(m_points, 0, size); 
	}
	if( i>= m_count ){
		i = m_count-1;
		 std::clog<<"the index over flow, reset index to count-1"<<std::endl;
	}
	return m_points[i];
}

bool Points::Increase() {
	AlPoint *tmp = (AlPoint*)realloc(m_points, m_count+m_increaseSize); 
	if(tmp) m_points = tmp;
	return tmp ? true:false;
}

AlPoint bezierControlPoint(AlPoint b,AlPoint e,AlPoint n,AlPoint *c,float f) {
	if( f>0.5 ) f = 0.5;
	else if( f<0.1 ) f = 0.1;
	float a1 = atan2f(e.y - b.y, e.x - b.x);
	float a2 = atan2f(e.y - n.y, e.x - n.x);
	float r = tan((a1+a2)/2 + M_PI_2);
	AlLine line(r, e); 
	float d = distance(b, e);
	d = d * f;
	AlPoint *ps = line.distancePointsTo(e.x, d);
	d = distance(ps[0], e);
	AlPoint point = ( distance(b, ps[0]) < distance(b, ps[1]) ) ?  ps[0]:ps[1];
	free(ps);
	if(c) {
		d = distance(e, n) * f;
		ps = line.distancePointsTo(e.x, d);
		*c = ( distance(n, ps[0]) < distance(n, ps[1]) ) ? ps[0]:ps[1];
		free(ps);
	}
	return point;
}

AlLine::AlLine(float radian, AlPoint point) {
	m_r = radian;
	m_point = point;
}

AlLine::AlLine(AlPoint p1, AlPoint p2) {
	m_r = (p1.y-p2.y) / (p1.x-p2.x);
	m_point = p1;
} 

AlPoint* AlLine::distancePointsTo(float x, float dist) {
	float y = m_r*(x-m_point.x) + m_point.y;
	AlPoint *point = (AlPoint*)malloc(sizeof(AlPoint)*2);
	float t = sqrtf(square(dist)/(1+square(m_r)));
	point[0].x = x + t;
	point[1].x = x - t;
	point[0].y = m_r * (point[0].x-x) + y;
	point[1].y = m_r * (point[1].x-x) + y;
	return point;
}

