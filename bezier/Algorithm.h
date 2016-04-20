#pragma once
 struct ALPoint {
	float x,y;
	int w; // 记录这一点上,线的宽度!!!!
}; 
class ALArrPoints {
public:
	ALArrPoints(int initSize, int increaseSize);
	~ALArrPoints();
	bool isEmpty(){return m_count==0?true:false;};
	int count(){return m_count;};
	bool append(ALPoint point);
	bool append(float x,float y);
	bool append(float x,float y,float w);
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
// 传入bezier曲线的起点,终点,第一个控制点,下一节bezier曲线的终点,以及一个(0-0.5)控制拟合程度的因子
// 返回两个点,其中靠近起点和终点的这个点为当前bezier曲线的第二个控制点,另外一个为下一节bezier曲线的
// 在最开始的时候,把起始点作为第一个控制点!!!  // 在结尾的时候,把结束点作为第二个控制点!!!
ALPoint bezierControlPoint(ALPoint b,ALPoint e,ALPoint n,ALPoint *c,float f);
float distance(ALPoint p1, ALPoint p2);
float square(float f);
float cubic(float f);
void doDraw(ALArrPoints points, float f);
