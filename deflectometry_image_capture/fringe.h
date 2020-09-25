#pragma once
#define _USE_MATH_DEFINES
#include <math.h>

#include "opencv2/core/mat.hpp"

class Fringe {
public:
	Fringe(unsigned int w, unsigned int h, bool is_vertical_fringe) :m_width(w), m_height(h) ,m_is_vertical_fringe(is_vertical_fringe){
		m_fringe.create(m_height, m_width, CV_8UC1);
		//m_weight.create(m_height, m_width, CV_32FC1);
	};
	Fringe(unsigned int w, unsigned int h, unsigned int n, bool is_vertical_fringe) //第三个参数为条纹数量
		:m_width(w), m_height(h), m_fringeNum(n), m_is_vertical_fringe(is_vertical_fringe) {
		m_fringe.create(m_height, m_width, CV_8UC1);
		//m_weight.create(m_height, m_width, CV_32FC1);
	};
	Fringe(unsigned int w, unsigned int h, unsigned int n, float p, bool is_vertical_fringe) //第三个参数为条纹数量
		:m_width(w), m_height(h), m_fringeNum(n), m_phase(p), m_is_vertical_fringe(is_vertical_fringe) {
		m_fringe.create(m_height, m_width, CV_8UC1);
		//m_weight.create(m_height, m_width, CV_32FC1);
	};
	~Fringe() {};

	void setPeriod(float p) { 
		if (m_is_vertical_fringe)
			m_fringeNum = int(m_width / p);
		else
			m_fringeNum = int(m_height / p);
	}
	void setFringeNumber(unsigned int n) { m_fringeNum = n; }
	void setPhase(float p) { m_phase = p; }

	bool setWeightMap() {
		if (m_weight.empty()) {
			m_weight.create(m_height, m_width, CV_32FC1);
		}
		//TODO:如果后面需要修改权重的话，添加修改权重的参数
	}
	bool generate();
	
	cv::Mat getFringe() const { return m_fringe; }
	float getPeriod() const { return (m_is_vertical_fringe ? float(m_width) / m_fringeNum : float(m_height) / m_fringeNum); }
	unsigned int getFringeNumber() { return m_fringeNum; }
	bool isVerticalFringe() const { return m_is_vertical_fringe; }

private:
	const unsigned int m_width;
	const unsigned int m_height;
	bool m_is_vertical_fringe;

	unsigned int m_fringeNum;//一张图案的条纹数,目前设定为整数
	float m_phase;//相位

	cv::Mat m_fringe;//条纹图
	static cv::Mat m_weight;//权重图，用于调制条纹,静态成员，认为不同条纹的权值相同
};