#include "fringe.h"

cv::Mat Fringe::m_weight = cv::Mat();

bool Fringe::generate() {
	if (!m_weight.empty()) {
		//TODO:���Ȩ��
	}
	else {
		if (m_is_vertical_fringe)
		{
			for (unsigned int w = 0; w < m_width; w++) {
				unsigned int intensity =
					int(
						255.0 * (
							cos(float(floor(int(w - m_width / 2))) * m_fringeNum / m_width * 2 * M_PI + m_phase)//��ͼ���м�Ϊ0��λ
							+ 1.0
							) / 2
						);
				for (unsigned int h = 0; h < m_height; h++) {
					m_fringe.at<uchar>(h, w) = intensity;
				}
			}
		}
		else
		{
			for (unsigned int w = 0; w < m_height; w++) {
				unsigned int intensity =
					int(
						255.0 * (
							cos(float(floor(int(w - m_height / 2))) * m_fringeNum / m_height * 2 * M_PI + m_phase)//��ͼ���м�Ϊ0��λ
							+ 1.0
							) / 2
						);
				for (unsigned int h = 0; h < m_width; h++) {
					m_fringe.at<uchar>(w, h) = intensity;
				}
			}
		}
	}
	return true;
}