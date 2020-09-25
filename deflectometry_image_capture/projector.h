#pragma once

#include <Windows.h>
#include <iostream>
#include <vector>
#include "opencv2/core/mat.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"

#include "fringe.h"
#include "FringeSequence.h"

//#define __debug__

static unsigned int g_left = 0, g_top = 0;
static unsigned int g_width, g_height;

//Code by Zhang Jingyu
//Email: jyzhang@sjtu.edu.cn
inline BOOL CALLBACK MyInfoEnumProc(
	HMONITOR hMonitor,
	HDC hdcMonitor,
	LPRECT lprcMonitor,
	LPARAM dwData
)
{
	MONITORINFOEX mi;
	//ZeroMemory(&mi, sizeof(mi));
	mi.cbSize = sizeof(mi);
	GetMonitorInfo(hMonitor, &mi);
	//wprintf(L"DisplayDevice: %s\n", mi.szDevice);
	if (MONITORINFOF_PRIMARY != mi.dwFlags) {//如果不是主屏幕的话，TODO，现在只有一个屏幕的情况下判断有问题
		//MONITORINFOEX mi;
		//ZeroMemory(&mi, sizeof(mi));
		//mi.cbSize = sizeof(mi);
		//GetMonitorInfo(hMonitor, &mi);
		
		g_width = lprcMonitor->right - lprcMonitor->left;
		g_height = lprcMonitor->bottom - lprcMonitor->top;
		g_left = lprcMonitor->left;
		g_top = lprcMonitor->top;
		return FALSE;
	}
#ifdef __debug__
	std::cout << "无法显示在主屏幕上" << std::endl;
#endif
	return TRUE;
}

//Code by Zhang Jingyu
//Email: jyzhang@sjtu.edu.cn
class Projector {
public:
	Projector():
		m_left(0),
		m_top(0)
	{};
	~Projector() {};

	bool init(){
		if (2 != GetSystemMetrics(SM_CMONITORS)) {
			std::cout << "找不到投影仪" << std::endl;
			return false;
		}
		int count = 0;
		if(!EnumDisplayMonitors(NULL, NULL, &MyInfoEnumProc, 0)) {
			m_width = g_width;
			m_height = g_height;
			m_left = g_left;
			m_top = g_top;

#ifdef __debug__
			std::cout << "left = " << m_left << " top = " << m_top << std::endl;
#endif
			cv::destroyAllWindows();
			cv::namedWindow("fringe", cv::WINDOW_NORMAL);
			cv::moveWindow("fringe", m_left + 1, m_top + 1);
			cv::setWindowProperty("fringe", cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN);//全屏
			return true;
		}
		return false;
	}

	void display(const cv::Mat map){
		if (0 != m_left || 0 != m_top){
			cv::imshow("fringe", map);
			cv::waitKey(20);
		}
		return;
	}
	void displaySequence(
		const std::vector<cv::Mat> mapList,
		const unsigned int interval//时间间隔，单位ms?
	);

	void displayFringe(const Fringe &fringe){
		if (0 != m_left || 0 != m_top){
			cv::imshow("fringe", fringe.getFringe());
			cv::waitKey(20);
		}
		return;
	}

	void displayFringe(const std::shared_ptr<Fringe> fringe) {
		if (0 != m_left || 0 != m_top) {
			cv::imshow("fringe", fringe->getFringe());
			cv::waitKey(20);
		}
		return;
	}
	void displayFringeSequence(
		const FringeList fringeList,
		const unsigned int interval//时间间隔，单位ms?
		) {
		if (0 != m_left || 0 != m_top){
			for (size_t i = 0; i < fringeList.size(); i++) {
				cv::imshow("fringe", fringeList[i]->getFringe());
				cv::waitKey(interval);
			}
		}
		return;
	}

	template<class T, class VMap>
	void displayFringeSequence(
		const FringeList fringeList,
		T* classType,
		VMap& map,
		int capture_num,
		int experiment_num,
		bool save_image
		) {
		if (0 != m_left || 0 != m_top){
			for (size_t i = 0; i < fringeList.size(); i++) {
				cv::imshow("fringe", fringeList[i]->getFringe());
				cv::waitKey(1000);
				classType -> capture(map, capture_num);
				if (save_image)
				{
					int camera_num = map.size() / capture_num;
					for (int j = 0; j < map.size(); j++)
					{
						if (fringeList[i]->isVerticalFringe())
						{
							//开始存图
							std::string image_name = "phase_x" + std::to_string(fringeList[i]->getFringeNumber()) + "_" + std::to_string(i) + "-" + std::to_string(j / camera_num) + ".bmp";
		                    std::string image_path_fill = file_path + "\\" + std::to_string(j % camera_num) + "\\" + std::to_string(experiment_num) + "\\" + image_name;
							cv::imwrite(image_path_fill, map[j]);
						}
						else
						{
							//开始存图
							std::string image_name = "phase_y" + std::to_string(fringeList[i]->getFringeNumber()) + "_" + std::to_string(i) + "-" + std::to_string(j / camera_num) + ".bmp";
							std::string image_path_fill = file_path + "\\" + std::to_string(j % camera_num) + "\\" + std::to_string(experiment_num) + "\\" + image_name;
							cv::imwrite(image_path_fill, map[j]);
						}
					}
					VMap().swap(map);
				}
				cv::waitKey(100);
			}
		}
		return;
	}

	void getProjectorResolution(int& width, int& height){
		width = m_width;
		height = m_height;
		return;
	}


private:
	unsigned int m_left, m_top;
	unsigned int m_width, m_height;

};

