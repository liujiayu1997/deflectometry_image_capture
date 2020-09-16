#pragma once
//���õ�����ӿڣ����ž��(jyzhang@sjtu.edu.cn)��д
//ʵ�����¹��ܣ�

#include <string>
#include <iostream>
#include <vector>
#include <functional>

#include <atomic>

using namespace std;

class Camera {
public:
	enum class State{
		Close,				//˵����ǰ������ڹر�״̬
		Waiting,			//˵����ǰ������ڵȴ�����״̬
		NonBlocking,		//˵�����ڻص�״̬���ȴ�����
		Blocking			//˵�����ڶ���״̬
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

	//�����ʼ��
	virtual bool init() = 0;

	//����ͷ���Դ
	virtual bool uninit() = 0;

	//��ȡ�����豸��Ϣ
	virtual void printDeviceInfo() = 0;


	//��֡����
	virtual bool capture(void*& data) = 0;

	//�ص��ɼ�
	virtual bool startCaptureCallback() = 0;
	virtual bool emitTriggerSoftware() = 0;
	virtual bool getCallbackData(void* &data, int millisec) = 0;
	virtual bool stopCaptureCallback() = 0;

	//����ͼƬ�ֱ���
	virtual bool setPictureResolution(int& width, int& height) = 0;

	//�鿴ͼƬ�ֱ���
	virtual bool getPictureResolution(int& width, int& height) = 0;

	//����ͼ���ʽ
	//BayerRG8�� BayerRG10
	virtual bool setPictureFormat(string mode) = 0;

	//�����ع����
	virtual bool setExposureTime(float t) = 0;
	virtual bool setExposureAuto(string mode) = 0;

	//���ð�ƽ��
	virtual bool setBalanceWhiteAuto(string mode) = 0;
	virtual bool setBalanceRatio(float r, float g, float b) = 0;

	//��������
	virtual bool setGainAuto(string mode, float min = 0, float max = 23.9) = 0;
	virtual bool setGain(float g) = 0;
	static void getCameraID(vector<string>& ID) {};

private:
	State m_state;
};


