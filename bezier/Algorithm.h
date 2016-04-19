#pragma once
 struct AlPoint {
	float x,y;
	int w; // 记录这一点上,线的宽度!!!!
}; 
class Points {
public:
	Points(int initSize, int increaseSize);
	~Points();
	bool isEmpty(){return m_count==0?true:false;};
	int count(){return m_count;};
	bool append(AlPoint point);
	bool append(float x,float y);
	AlPoint *getPoint(int index);
	float distanceTo(AlPoint point);
	AlPoint& operator[](unsigned int i);
private: 
	bool Increase();
private:
	int m_capability;
	int m_count;
	int m_increaseSize;
	AlPoint *m_points;
};

class AlLine{
public:
	AlLine(float radian, AlPoint point);
	AlLine(AlPoint p1, AlPoint p2);
	~AlLine(){};
	AlPoint* distancePointsTo(float x, float dist);
private:
	float m_r;
	AlPoint m_point;
};

// 传入bezier曲线的起点,终点,第一个控制点,下一节bezier曲线的终点,以及一个(0-0.5)控制拟合程度的因子
// 返回两个点,其中靠近起点和终点的这个点为当前bezier曲线的第二个控制点,另外一个为下一节bezier曲线的
// 在最开始的时候,把起始点作为第一个控制点!!!  // 在结尾的时候,把结束点作为第二个控制点!!!
AlPoint bezierControlPoint(AlPoint b,AlPoint e,AlPoint n,AlPoint *c,float f);
float distance(AlPoint p1, AlPoint p2);
float square(float f);
void doDraw(Points points, float f);
