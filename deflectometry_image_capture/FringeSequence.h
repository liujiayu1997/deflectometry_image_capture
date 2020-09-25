#pragma once
#include "fringe.h"
#include <vector>
#include <memory>
typedef std::vector<std::shared_ptr<Fringe> > FringeList;

class FringeSequence {
public:
	FringeSequence() {};
	FringeSequence(FringeList l) { m_FringeList = l; }
	~FringeSequence() {};

	inline bool init(
		unsigned int fringeNum, //一张图像中的条纹数量
		unsigned int pWidth,//屏幕的宽
		unsigned int pHeight,//屏幕的高
		unsigned int patternNum,//条纹图案的数量
		bool is_vertical_fringe//是否为竖条纹
	);
	inline bool init(
		unsigned int fringeNum, //一张图像中的条纹数量
		unsigned int pWidth,//屏幕的宽
		unsigned int pHeight,//屏幕的高
		cv::Mat weight,//权值图
		unsigned int patternNum,//条纹图案的数量
		bool is_vertical_fringe//是否为竖条纹
	);

	bool isVerticalFringe() const{
		return m_is_vertical_fringe;
	};

	FringeList getFringeList() const {
		return m_FringeList;
	};

	size_t getPatternNumber() const{
		return m_FringeList.size();
	};

private:
	FringeList m_FringeList;
	bool m_is_vertical_fringe;

};

bool FringeSequence::init(
	unsigned int fringeNum, //一张图像中的条纹数量
	unsigned int pWidth,//屏幕的宽
	unsigned int pHeight,//屏幕的高
	unsigned int patternNum,//条纹图案的数量
	bool is_vertical_fringe//是否为竖条纹
) 
{
	m_is_vertical_fringe = is_vertical_fringe;
	m_FringeList.clear();
	if (m_is_vertical_fringe)
	{
		for (unsigned int i = 0; i < patternNum; i++) {
			std::shared_ptr<Fringe> fringe(new Fringe(pWidth, pHeight, fringeNum, m_is_vertical_fringe));
			fringe->setPhase(float(float(i) * 2 * M_PI / patternNum));
			fringe->generate();//每次需要生成条纹，否则没有图像
			m_FringeList.push_back(fringe);
		}
	}
	else
	{
		for (unsigned int i = 0; i < patternNum; i++) {
			std::shared_ptr<Fringe> fringe(new Fringe(pWidth, pHeight, fringeNum, m_is_vertical_fringe));
			fringe->setPhase(float(float(i) * 2 * M_PI / patternNum));
			fringe->generate();//每次需要生成条纹，否则没有图像
			m_FringeList.push_back(fringe);
		}
	}

	return true;
}