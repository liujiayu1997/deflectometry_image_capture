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
		unsigned int fringeNum, //һ��ͼ���е���������
		unsigned int pWidth,//��Ļ�Ŀ�
		unsigned int pHeight,//��Ļ�ĸ�
		unsigned int patternNum,//����ͼ��������
		bool is_vertical_fringe//�Ƿ�Ϊ������
	);
	inline bool init(
		unsigned int fringeNum, //һ��ͼ���е���������
		unsigned int pWidth,//��Ļ�Ŀ�
		unsigned int pHeight,//��Ļ�ĸ�
		cv::Mat weight,//Ȩֵͼ
		unsigned int patternNum,//����ͼ��������
		bool is_vertical_fringe//�Ƿ�Ϊ������
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
	unsigned int fringeNum, //һ��ͼ���е���������
	unsigned int pWidth,//��Ļ�Ŀ�
	unsigned int pHeight,//��Ļ�ĸ�
	unsigned int patternNum,//����ͼ��������
	bool is_vertical_fringe//�Ƿ�Ϊ������
) 
{
	m_is_vertical_fringe = is_vertical_fringe;
	m_FringeList.clear();
	if (m_is_vertical_fringe)
	{
		for (unsigned int i = 0; i < patternNum; i++) {
			std::shared_ptr<Fringe> fringe(new Fringe(pWidth, pHeight, fringeNum, m_is_vertical_fringe));
			fringe->setPhase(float(float(i) * 2 * M_PI / patternNum));
			fringe->generate();//ÿ����Ҫ�������ƣ�����û��ͼ��
			m_FringeList.push_back(fringe);
		}
	}
	else
	{
		for (unsigned int i = 0; i < patternNum; i++) {
			std::shared_ptr<Fringe> fringe(new Fringe(pWidth, pHeight, fringeNum, m_is_vertical_fringe));
			fringe->setPhase(float(float(i) * 2 * M_PI / patternNum));
			fringe->generate();//ÿ����Ҫ�������ƣ�����û��ͼ��
			m_FringeList.push_back(fringe);
		}
	}

	return true;
}