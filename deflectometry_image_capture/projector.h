#pragma once

#include <Windows.h>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include "opencv2/core/mat.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include <opencv2/imgproc.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

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
	Projector() :
		m_left(0),
		m_top(0),
		m_image_update(false)
	{};
	~Projector() {};

	bool init() {
		if (2 != GetSystemMetrics(SM_CMONITORS)) {
			std::cout << "找不到投影仪" << std::endl;
			return false;
		}
		int count = 0;
		if (!EnumDisplayMonitors(NULL, NULL, &MyInfoEnumProc, 0)) {
			m_width = g_width;
			m_height = g_height;
			m_left = g_left;
			m_top = g_top;

#ifdef __debug__
			std::cout << "left = " << m_left << " top = " << m_top << std::endl;
#endif

			std::thread display(&Projector::displayWorker, this, 40);
			display.detach();
			return true;
		}
		return false;
	}

	void display(cv::Mat map) {
		if (0 != m_left || 0 != m_top) {
			setDisplayImage(map);
		}
		return;
	}
	void displaySequence(
		const std::vector<cv::Mat> mapList,
		const unsigned int interval//时间间隔，单位ms?
	);

	void displayFringe(const Fringe& fringe) {
		if (0 != m_left || 0 != m_top) {
			setDisplayImage(fringe.getFringe());
		}
		return;
	}

	void displayFringe(const std::shared_ptr<Fringe> fringe) {
		if (0 != m_left || 0 != m_top) {
			cv::Mat image_show = fringe->getFringe();

			setDisplayImage(fringe->getFringe());
		}
		return;
	}

	void displayFringeSequence(
		const FringeList fringeList,
		const unsigned int interval//时间间隔，单位ms?
	) {
		if (0 != m_left || 0 != m_top) {
			for (size_t i = 0; i < fringeList.size(); i++) {
				setDisplayImage(fringeList[i]->getFringe());
			}
		}
		return;
	}

	template<class T, class VMap>
	void displayFringeSequence(
		const FringeList fringeList,
		T* classType,
		bool(T::* cameraCallback)(VMap&, bool),
		VMap& map
	) {
		if (0 != m_left || 0 != m_top) {
			for (size_t i = 0; i < fringeList.size(); i++) {
				setDisplayImage(fringeList[i]->getFringe());
				Sleep(50);
				classType->cameraCallback(map, true);
			}
		}
		return;
	}

	void getProjectorResolution(int& width, int& height) {
		width = m_width;
		height = m_height;
		return;
	}


private:
	void setDisplayImage(cv::Mat& image) {
		//std::unique_lock<std::mutex> lock(m_image_mutex);
		//std::cout << "display" << std::endl;
		cv::cvtColor(image, m_cvImage, cv::COLOR_GRAY2RGBA);
		m_image.create(m_cvImage.cols, m_cvImage.rows, m_cvImage.ptr());
		m_image_update = true;
		while (m_image_update) {
			//m_image_cv.wait(lock);
		}
	}

	void displayWorker(
		const unsigned int interval//时间间隔，单位ms?
	) {
		m_window.create(sf::VideoMode(g_width, g_height), "window", sf::Style::None);
		m_window.setPosition(sf::Vector2i(g_left + 1, g_top + 1));

		sf::Texture texture;
		sf::Sprite sprite;

		while (m_window.isOpen()) {
			{
				//std::unique_lock<std::mutex> lock(m_image_mutex);
				if (m_image_update) {
					sf::Event event;
					while (m_window.pollEvent(event))
					{
						if (event.type == sf::Event::Closed)
							m_window.close();
					}
					if (!texture.loadFromImage(m_image))
					{
						break;
					}
					sprite.setTexture(texture);
					m_window.draw(sprite);
					m_window.display();
					m_image_update = false;
				}
			}
		}
	}
	unsigned int m_left, m_top;
	unsigned int m_width, m_height;

	sf::RenderWindow m_window;

	cv::Mat m_cvImage;
	sf::Image m_image;
	std::atomic<bool> m_image_update;
};

