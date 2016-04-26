#pragma once
 struct ALPoint {
	float x,y;
	int w; // 记录这一点上,线的宽度!!!!
}; 
class AlpointsList{
public:
	AlpointsList(int initSize, int increaseSize);
	~AlpointsList();
	bool isEmpty(){return m_count==0?true:false;};
	int  count(){return m_count;};
	bool append(ALPoint point);
	bool append(float x,float y);
	bool append(float x,float y,float w);
	// AlpointsList sub(int b, int e);
	ALPoint pup();
	void setEmpty(){m_count=0;}
	ALPoint *getPoint(int index);
	float distanceTo(ALPoint point);
	ALPoint& operator[](unsigned int i);
private: 
	bool increase();
private:
	int m_capability;
	int m_count;
	int m_increaseSize;
	ALPoint *m_points;
};
class AlLine{
public:
	AlLine(float radian, ALPoint point);
	AlLine(ALPoint p1, ALPoint p2);
	~AlLine(){};
	ALPoint* distancePointsTo(float x, float dist);
private:
	float m_r;
	ALPoint m_point;
};
ALPoint cubicBezierControlPoint(ALPoint b,ALPoint e,ALPoint n,ALPoint *c,float f);
ALPoint calcuBezierPoint(ALPoint b, ALPoint e, ALPoint c1, ALPoint c2, float t);
float distance(ALPoint p1, ALPoint p2);
float square(float f);
float cubic(float f);
