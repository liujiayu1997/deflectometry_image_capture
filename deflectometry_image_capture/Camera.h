#pragma once
//自用的相机接口，由张景瑜(jyzhang@sjtu.edu.cn)编写
//实现以下功能：

#include <string>
#include <iostream>
#include <vector>
#include <functional>

#include <atomic>

using namespace std;

class Camera {
public:
	enum class State{
		Close,				//说明当前相机处于关闭状态
		Waiting,			//说明当前相机处于等待工作状态
		NonBlocking,		//说明处在回调状态，等待触发
		Blocking			//说明处在堵塞状态
	};

public:
	Camera(): m_state(State::Close) {};


	State getState() {
		return m_state;
	}

	void setState(State s) {
		m_state = s;
	}

public:
	virtual ~Camera() {};

	static bool initCameraList() {};

	//相机初始化
	virtual bool init() = 0;

	//相机释放资源
	virtual bool uninit() = 0;

	//获取所有设备信息
	virtual void printDeviceInfo() = 0;


	//单帧拍摄
	virtual bool capture(void*& data) = 0;

	//回调采集
	virtual bool startCaptureCallback() = 0;
	virtual bool emitTriggerSoftware() = 0;
	virtual bool getCallbackData(void* &data, int millisec) = 0;
	virtual bool stopCaptureCallback() = 0;

	//设置图片分辨率
	virtual bool setPictureResolution(int& width, int& height) = 0;

	//查看图片分辨率
	virtual bool getPictureResolution(int& width, int& height) = 0;

	//设置图像格式
	//BayerRG8， BayerRG10
	virtual bool setPictureFormat(string mode) = 0;

	//设置曝光参数
	virtual bool setExposureTime(float t) = 0;
	virtual bool setExposureAuto(string mode) = 0;

	//设置白平衡
	virtual bool setBalanceWhiteAuto(string mode) = 0;
	virtual bool setBalanceRatio(float r, float g, float b) = 0;

	//设置增益
	virtual bool setGainAuto(string mode, float min = 0, float max = 23.9) = 0;
	virtual bool setGain(float g) = 0;
	static void getCameraID(vector<string>& ID) {};

private:
	State m_state;
};


