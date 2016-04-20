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
ALArrPoints::ALArrPoints(int initSize, int increaseSize) {
	m_count = 0;
	m_capability = initSize<=0 ? 1:initSize;
	m_increaseSize = increaseSize;
	int memSize = initSize*sizeof(ALPoint);
	m_points = (ALPoint*)malloc(memSize);
	memset(m_points, 0, memSize);
}

ALArrPoints::~ALArrPoints() {
	if(m_points) free(m_points); 
}

bool ALArrPoints::append(ALPoint point) {
	if(m_capability==m_count){
		if(!increase()){
			return false; 
		}
	}
	m_points[m_count++] = point;
	return true;
}

bool ALArrPoints::append(float x,float y) {
	ALPoint point = {x, y, 0.1};
	return append(point); 
}

bool ALArrPoints::append(float x,float y,float w) {
	ALPoint point = {x, y, w};
	return append(point); 
}

ALPoint * ALArrPoints::getPoint(int index) {
	if(index >= m_count)
		return NULL;
	return m_points + index;
}

ALPoint& ALArrPoints::operator[](unsigned int i) {
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

bool ALArrPoints::increase() {
	ALPoint *tmp = (ALPoint*)realloc(m_points, (m_count+m_increaseSize)*sizeof(ALPoint)); 
	if(tmp) {
		m_capability += 5;
		m_points = tmp;
	}
	return tmp ? true:false;
}

ALPoint bezierControlPoint(ALPoint b,ALPoint e,ALPoint n,ALPoint *c,float f) {
	if( f>0.5 ) f = 0.5;
	else if( f<0.1 ) f = 0.1;
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

