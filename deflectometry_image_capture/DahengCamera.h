#pragma once
//自用的大恒相机接口，由张景瑜(jyzhang@sjtu.edu.cn)编写
//实现以下功能：
//获得相机序列、曝光时间、白平衡、拍摄、错误判断...

#define __debug__
//#define __capturecallback__ //TODO 
#include "Camera.h"

#include "IGXFactory.h"
#include "IGXDevice.h"
#include "GalaxyException.h"

#include <fstream>
#include <assert.h>

#include <string>
#include <iostream>
#include <vector>
//多线程
#include <future>

using namespace std;

class DahengCamera final: public Camera{
private:
	class CaptureEventHandler : public ICaptureEventHandler
	{
	public:
		void DoOnImageCaptured(CImageDataPointer& objImageDataPointer, void* ths) {
#ifdef __debug__
			cout << "开始采图"<< (GX_FRAME_STATUS_SUCCESS == objImageDataPointer->GetStatus()) << endl;
#endif
			if (GX_FRAME_STATUS_SUCCESS == objImageDataPointer->GetStatus()) {
				if (GX_PIXEL_FORMAT_BAYER_RG8 == objImageDataPointer->GetPixelFormat() || GX_PIXEL_FORMAT_BAYER_GR8 == objImageDataPointer->GetPixelFormat())
					((DahengCamera*)ths) -> dataProm.set_value(objImageDataPointer->ConvertToRGB24(GX_BIT_0_7, GX_RAW2RGB_NEIGHBOUR, false));
				else if (GX_PIXEL_FORMAT_BAYER_RG12 == objImageDataPointer->GetPixelFormat())
					((DahengCamera*)ths) -> dataProm.set_value(objImageDataPointer->ConvertToRGB24(GX_BIT_4_11, GX_RAW2RGB_NEIGHBOUR, false));
				else
					//在这里定义其他格式的图像数据
					((DahengCamera*)ths)->dataProm.set_value(nullptr);
#ifdef __debug__
				cout << "采图成功" << endl;
#endif
				/*size_t size = objImageDataPointer->GetWidth() * objImageDataPointer->GetHeight() * 3;
				if (data) {
					free(data);
					data = nullptr;
				}
				data = malloc(size);
				if (GX_PIXEL_FORMAT_BAYER_RG8 == objImageDataPointer->GetPixelFormat())
					memcpy(data, objImageDataPointer->ConvertToRGB24(GX_BIT_0_7, GX_RAW2RGB_NEIGHBOUR, false), size);
				else if (GX_PIXEL_FORMAT_BAYER_RG12 == objImageDataPointer->GetPixelFormat())
					memcpy(data, objImageDataPointer->ConvertToRGB24(GX_BIT_4_11, GX_RAW2RGB_NEIGHBOUR, false), size);*/
			}
			//if (GX_FRAME_STATUS_SUCCESS == objImageDataPointer->GetStatus()) {
			//	if (GX_PIXEL_FORMAT_BAYER_RG8 == objImageDataPointer->GetPixelFormat())
			//		(*static_cast<void**>(data)) = objImageDataPointer->ConvertToRGB24(GX_BIT_0_7, GX_RAW2RGB_NEIGHBOUR, true);
			//	else if (GX_PIXEL_FORMAT_BAYER_RG12 == objImageDataPointer->GetPixelFormat())
			//		(*static_cast<void**>(data)) = objImageDataPointer->ConvertToRGB24(GX_BIT_4_11, GX_RAW2RGB_NEIGHBOUR, true);*/

			//}
		}
	};

public:
	DahengCamera(): m_index(m_counter){
		////TODO这里似乎有线程安全问题
		//if (m_dinfovector.empty()) {
		//	initCameraList();
		//}
		assert(m_counter++ < m_dinfovector.size());
	};
	virtual ~DahengCamera() { m_counter--; };

	//必须在实例化DahengCamera之前调用
	static bool initCameraList() {
		try
		{
			IGXFactory::GetInstance().Init();
			IGXFactory::GetInstance().UpdateDeviceList(1000, *(const_cast<GxIAPICPP::gxdeviceinfo_vector*>(&m_dinfovector)));
			cout << "已完成相机列表初始化，并更新设备信息" << endl;
			cout << "获得设备信息数量为：" << m_dinfovector.size() << endl;
			return true;
		}

		catch (CGalaxyException& e)
		{
			cout << "错误码: " << e.GetErrorCode() << endl;
			cout << "错误描述信息: " << e.what() << endl;
			return false;

		}
	}
	//在DahengCamera使用完毕后调用
	static bool uninitCamera() {
		IGXFactory::GetInstance().Uninit();
		cout << "已释放相机驱动资源" << endl;
		return true;
	}

	//相机初始化
	virtual bool init(){
		try{
			GxIAPICPP::gxstring strSN = m_dinfovector[m_index].GetSN();
			GxIAPICPP::gxstring strUserID = m_dinfovector[m_index].GetUserID();
			m_device = IGXFactory::GetInstance().OpenDeviceBySN(strSN, GX_ACCESS_EXCLUSIVE);
			m_featureControl = m_device->GetRemoteFeatureControl();
			if (m_featureControl->IsImplemented("TriggerMode"))
			{
				if (m_featureControl->IsWritable("TriggerMode")) {//查询是否可写
					m_featureControl->GetEnumFeature("TriggerMode")->SetValue("Off");//设置当前值
				}
			}
			uint32_t nStreamNum = m_device->GetStreamCount();
#ifdef __debug__
			cout << "设备" << m_index << "流个数:" << nStreamNum << endl;
#endif
			if (nStreamNum > 0)
			{
				m_stream = m_device->OpenStream(0);
			}
			setState(State::Waiting);
			cout << "已完成相机"<< m_index <<"初始化" << endl;
			return true;
		}
		catch (CGalaxyException& e){
			cout << "错误码: " << e.GetErrorCode() << endl;
			cout << "错误描述信息: " << e.what() << endl;
			return false;
		}

	}

	//相机释放资源
	virtual bool uninit(){
		try{
			//关闭流通道
			m_stream->Close();
			m_device->Close();
			setState(State::Close);
			return true;
		}
		catch (CGalaxyException& e){
			cout << "错误码: " << e.GetErrorCode() << endl;
			cout << "错误描述信息: " << e.what() << endl;
			return false;
		}
	}

	//获取当前设备信息
	virtual void printDeviceInfo(){
		cout << m_dinfovector[m_index].GetVendorName() << endl;
		cout << m_dinfovector[m_index].GetModelName() << endl;
		cout << m_dinfovector[m_index].GetDeviceID() << endl;
		cout << m_dinfovector[m_index].GetSN() << endl;
			//更多设备信息详见IGXDeviceInfo接口
	}

	//单帧拍摄，堵塞
	virtual bool capture(void* &data){
		try{
			unique_lock<mutex> lk(m_stateMutex);
			if (!m_stateCv.wait_for(lk, chrono::milliseconds(1000), [&] {
				return (getState() == State::Waiting);
			})) {
				cout << "当前设备未就绪，请重新连接或稍等再试" << endl;
				return false;
			}
			setState(State::Blocking);
			//开启流通道采集
			m_stream->StartGrab();

			//给设备发送开采命令
			m_featureControl->GetCommandFeature("AcquisitionStart")->Execute();

			CImageDataPointer objImageDataPtr;
			objImageDataPtr = m_stream->GetImage(1000);//超时时间使用500ms，用户可以自行设定
			if (objImageDataPtr->GetStatus() == GX_FRAME_STATUS_SUCCESS)
			{
#ifdef __debug__
				cout << m_featureControl->GetEnumFeature("PixelFormat")->GetValue() << endl;
				cout << ("BayerRG8" == m_featureControl->GetEnumFeature("PixelFormat")->GetValue()) << endl;
#endif
				//采图成功而且是完整帧，可以进行图像处理，可以考虑自定义图像格式
				size_t size = objImageDataPtr->GetWidth() * objImageDataPtr->GetHeight() * 3;
				if (data) {
					free(data);
					data = nullptr;
				}
				data = malloc(size);
				if (0 == ("BayerRG8" == m_featureControl->GetEnumFeature("PixelFormat")->GetValue()))
					memcpy(data, objImageDataPtr->ConvertToRGB24(GX_BIT_0_7, GX_RAW2RGB_NEIGHBOUR, false), size);
				else if (0 == ("BayerRG12" == m_featureControl->GetEnumFeature("PixelFormat")->GetValue()))
					memcpy(data, objImageDataPtr->ConvertToRGB24(GX_BIT_4_11, GX_RAW2RGB_NEIGHBOUR, false), size);
			}

			//停采
			m_featureControl->GetCommandFeature("AcquisitionStop")->Execute();
			m_stream->StopGrab();
			setState(State::Waiting);
			m_stateCv.notify_all();
			return true;
		}
		catch (CGalaxyException& e){
			cout << "错误码: " << e.GetErrorCode() << endl;
			cout << "错误描述信息: " << e.what() << endl;
			return false;
		}
	}

	//回调采集
	virtual bool startCaptureCallback() {
		//注册采集回调函数
		unique_lock<mutex> lk(m_stateMutex);
		if (!m_stateCv.wait_for(lk, chrono::milliseconds(1000), [&] {
			return (State::Waiting == getState());
		})) {
#ifdef __debug__
			cout << "当前相机未处于等待状态，无法进入回调模式" << endl;
#endif
			return false;
		}
		if (m_featureControl->IsImplemented("TriggerMode"))
		{
			if (m_featureControl->IsWritable("TriggerMode")) {//查询是否可写
				m_featureControl->GetEnumFeature("TriggerMode")->SetValue("On");//设置当前值
			}
			
		}
		if (m_featureControl->IsImplemented("TriggerSource"))
		{
			if (m_featureControl->IsWritable("TriggerSource")) {//查询是否可写
				m_featureControl->GetEnumFeature("TriggerSource")->SetValue("Software");//设置当前值
			}
		}
		m_stream->RegisterCaptureCallback(&m_captureEventHandler, this);
		//开启流通道采集
		m_stream->StartGrab();
		//给设备发送开采命令
		m_featureControl->GetCommandFeature("AcquisitionStart")->Execute();
		dataProm = promise<void*>();
		dataFut = dataProm.get_future();
		setState(State::NonBlocking);
		m_stateCv.notify_all();
		return true;
	}
	virtual bool emitTriggerSoftware() {
		unique_lock<mutex> lk(m_stateMutex);
		if (!m_stateCv.wait_for(lk, chrono::milliseconds(1000), [&] {
			return (getState() == State::NonBlocking);
		})) {
#ifdef __debug__
			cout << "当前相机处于堵塞状态，无法触发" << endl;
#endif
			return false;
		}
		setState(State::Blocking);
		m_featureControl->GetCommandFeature("TriggerSoftware")->Execute();
		return true;
	}

	virtual bool getCallbackData(void* &data, int millisec) {
		chrono::milliseconds span(millisec);
		if (dataFut.wait_for(span) == std::future_status::timeout) {
#ifdef __debug__
			cout << "当前等待超时,已返回" << endl;
#endif
			return false;
		}
		else {
			data = dataFut.get();
#ifdef __debug__
			cout << "得到图像结果并返回" << endl;
#endif
			dataProm = promise<void*>();
			dataFut = dataProm.get_future();
			unique_lock<mutex> lk(m_stateMutex);
			setState(State::NonBlocking);
			m_stateCv.notify_all();
			return true;
		}
	}

	virtual bool stopCaptureCallback() {
		m_featureControl->GetCommandFeature("AcquisitionStop")->Execute();
		m_stream->StopGrab();
		m_stream->UnregisterCaptureCallback();
		if (m_featureControl->IsImplemented("TriggerMode"))
		{
			if (m_featureControl->IsWritable("TriggerMode")) {//查询是否可写
				m_featureControl->GetEnumFeature("TriggerMode")->SetValue("Off");//设置当前值
			}
		}
		dataProm = promise<void*>();
		dataFut = dataProm.get_future();
		unique_lock<mutex> lk(m_stateMutex);
		setState(State::Waiting);
		m_stateCv.notify_all();
		return true;
	}

	//设置图片分辨率
	virtual bool setPictureResolution(int& width, int& height){
		try{
			if (m_featureControl->IsImplemented("Width"))
			{
				if (m_featureControl->IsWritable("Width"))//查询是否可写
					m_featureControl->GetIntFeature("Width")->SetValue(width);//设置当前值
				if (m_featureControl->IsWritable("Height"))//查询是否可写
					m_featureControl->GetIntFeature("Height")->SetValue(height);//设置当前值
#ifdef __debug__
				cout << "设置相机" << m_index << "的分辨率为" << width << "x" << height << endl;
#endif
			}
			return true;
		}
		catch (CGalaxyException& e){
			cout << "错误码: " << e.GetErrorCode() << endl;
			cout << "错误描述信息: " << e.what() << endl;
			return false;
		}
	}
	
	//查看图片分辨率
	virtual bool getPictureResolution(int& width, int& height){
		try{
			if (m_featureControl->IsImplemented("Width"))
			{
				if (m_featureControl->IsWritable("Width"))//查询是否可写
					width = m_featureControl->GetIntFeature("Width")->GetValue();//设置当前值
				if (m_featureControl->IsWritable("Height"))//查询是否可写
					height = m_featureControl->GetIntFeature("Height")->GetValue();//设置当前值
#ifdef __debug__
				cout << "相机" << m_index << "的分辨率为" << width << "x" << height << endl;
#endif
			}
			return true;
		}
		catch (CGalaxyException& e){
			cout << "错误码: " << e.GetErrorCode() << endl;
			cout << "错误描述信息: " << e.what() << endl;
			return false;
		}
	}

	//设置图像格式
	//BayerRG8， BayerRG10
	virtual bool setPictureFormat(string mode){
		try{
			if (m_featureControl->IsImplemented("PixelFormat"))
			{
				if (m_featureControl->IsWritable("PixelFormat"))//查询是否可写
					m_featureControl->GetEnumFeature("PixelFormat")->SetValue(mode.c_str());//设置当前值
				cout << "设置相机" << m_index << "的像素格式为" << mode << endl;
			}
			return true;
		}
		catch (CGalaxyException& e){
			cout << "错误码: " << e.GetErrorCode() << endl;
			cout << "错误描述信息: " << e.what() << endl;
			return false;
		}
	}

	//设置曝光参数
	virtual bool setExposureTime(float t){//调用时默认关闭自动曝光
		try{
			if (m_featureControl->IsImplemented("ExposureTime"))
			{
				if (m_featureControl->IsWritable("ExposureAuto"))//查询是否可写
					m_featureControl->GetEnumFeature("ExposureAuto")->SetValue("Off");//关闭自动曝光
				if (m_featureControl->IsWritable("ExposureTime")){//查询是否可写
					m_featureControl->GetFloatFeature("ExposureTime")->SetValue(t);//设置当前值
					cout << "设置相机" << m_index << "的曝光时间为" << t << "us" << endl;
				}
			}
			return true;
		}
		catch (CGalaxyException& e){
			cout << "错误码: " << e.GetErrorCode() << endl;
			cout << "错误描述信息: " << e.what() << endl;
			return false;
		}
	}
	virtual bool setExposureAuto(string mode){//mode: Off, Continuous, Once
		try{
			if (m_featureControl->IsImplemented("ExposureAuto"))
			{
				if (m_featureControl->IsWritable("ExposureAuto")){//查询是否可写
					m_featureControl->GetEnumFeature("ExposureAuto")->SetValue(mode.c_str());//设置当前值
					cout << "设置相机" << m_index << "的自动曝光模式为" << mode << endl;
				}
			}
			return true;
		}
		catch (CGalaxyException& e){
			cout << "错误码: " << e.GetErrorCode() << endl;
			cout << "错误描述信息: " << e.what() << endl;
			return false;
		}
	}

	//设置白平衡
	virtual bool setBalanceWhiteAuto(string mode){//mode: Off, Continuous, Once
		try{
			if (m_featureControl->IsImplemented("BalanceWhiteAuto"))
			{
				if (m_featureControl->IsWritable("BalanceWhiteAuto")){//查询是否可写
					m_featureControl->GetEnumFeature("BalanceWhiteAuto")->SetValue(mode.c_str());//设置当前值
					cout << "设置相机" << m_index << "的自动白平衡模式为" << mode << endl;
				}
			}
			return true;
		}
		catch (CGalaxyException& e){
			cout << "错误码: " << e.GetErrorCode() << endl;
			cout << "错误描述信息: " << e.what() << endl;
			return false;
		}
	}
	virtual bool setBalanceRatio(float r, float g, float b){
		try{
			if (m_featureControl->IsImplemented("BalanceWhiteAuto"))
			{
				if (m_featureControl->IsWritable("BalanceWhiteAuto")){//查询是否可写
					m_featureControl->GetEnumFeature("BalanceWhiteAuto")->SetValue("Off");//设置当前值
				}
				if (m_featureControl->IsWritable("BalanceRatioSelector"))
					m_featureControl->GetEnumFeature("BalanceRatioSelector")->SetValue("Red");
				if (m_featureControl->IsWritable("BalanceRatio"))
					m_featureControl->GetFloatFeature("BalanceRatio")->SetValue(r);

				if (m_featureControl->IsWritable("BalanceRatioSelector"))
					m_featureControl->GetEnumFeature("BalanceRatioSelector")->SetValue("Green");
				if (m_featureControl->IsWritable("BalanceRatio"))
					m_featureControl->GetFloatFeature("BalanceRatio")->SetValue(g);

				if (m_featureControl->IsWritable("BalanceRatioSelector"))
					m_featureControl->GetEnumFeature("BalanceRatioSelector")->SetValue("Blue");
				if (m_featureControl->IsWritable("BalanceRatio"))
					m_featureControl->GetFloatFeature("BalanceRatio")->SetValue(b);

				cout << "设置相机" << m_index << "的白平衡系数为" << endl
					<< "r = " << r << endl
					<< "g = " << g << endl
					<< "b = " << b << endl;
			}
			return true;
		}
		catch (CGalaxyException& e){
			cout << "错误码: " << e.GetErrorCode() << endl;
			cout << "错误描述信息: " << e.what() << endl;
			return false;
		}
	}

	//设置增益
	virtual bool setGainAuto(string mode, float min = 0, float max = 23.9){
		try{
			if (m_featureControl->IsImplemented("GainAuto"))
			{
				if (m_featureControl->IsWritable("GainAuto")){//查询是否可写
					m_featureControl->GetEnumFeature("GainAuto")->SetValue(mode.c_str());//设置当前值
					cout << "设置相机" << m_index << "的自动自动模式为" << mode << endl;
				}
				if ("Continuous" == mode || "Once" == mode){
					if (m_featureControl->IsWritable("AutoGainMin"))
						m_featureControl->GetFloatFeature("AutoGainMin")->SetValue(min);
					if (m_featureControl->IsWritable("AutoGainMax"))
						m_featureControl->GetFloatFeature("AutoGainMax")->SetValue(max);
					cout << "相机自动增益最小值为" << min << "，最大值为" << max << endl;
				}
			}
			return true;
		}
		catch (CGalaxyException& e){
			cout << "错误码: " << e.GetErrorCode() << endl;
			cout << "错误描述信息: " << e.what() << endl;
			return false;
		}
	}
	virtual bool setGain(float g){
		try{
			if (m_featureControl->IsImplemented("Gain"))
			{
				if (m_featureControl->IsWritable("GainAuto"))//查询是否可写
					m_featureControl->GetEnumFeature("GainAuto")->SetValue("Off");//关闭自动曝光
				if (m_featureControl->IsWritable("Gain")){//查询是否可写
					m_featureControl->GetFloatFeature("Gain")->SetValue(g);//设置当前值
					cout << "设置相机" << m_index << "的增益为" << g << "dB" << endl;
				}
			}
			return true;
		}
		catch (CGalaxyException& e){
			cout << "错误码: " << e.GetErrorCode() << endl;
			cout << "错误描述信息: " << e.what() << endl;
			return false;
		}
	}


	//导出相机配置文件
	bool exportConfigFile(string filePathToName = "camera/default"){
		try{
			m_device->ExportConfigFile((filePathToName + "_" + to_string(m_index)).c_str());
		}
		catch (CGalaxyException& e){
			cout << "错误码: " << e.GetErrorCode() << endl;
			cout << "错误描述信息: " << e.what() << endl;
			return false;
		}
	}

	//导入相机配置文件
	bool importConfigFile(string filePathToName){
		try{
			m_device->ImportConfigFile(filePathToName.c_str());
			return true;
		}
		catch (CGalaxyException& e){
			cout << "错误码: " << e.GetErrorCode() << endl;
			cout << "错误描述信息: " << e.what() << endl;
			return false;
		}
	}

	static void getCameraID(vector<string>& ID) {
		for (int i = 0; i < m_dinfovector.size(); i++) {
			ID.push_back(m_dinfovector[i].GetDeviceID().c_str());
		}
	}

	//bool importConfigFile(string filePathToName = "camera/default"){
	//	try{
	//		for (uint32_t i = 0; i < m_dvector.size(); i++)
	//		{
	//			CGXStreamPointer objStreamPtr = m_dvector[i]->OpenStream(0);
	//			m_dvector[i]->ImportConfigFile((filePathToName + "_" + to_string(i)).c_str());
	//			//关闭流通道
	//			objStreamPtr->Close();
	//		}
	//		return true;
	//	}
	//	catch (CGalaxyException& e){
	//		cout << "错误码: " << e.GetErrorCode() << endl;
	//		cout << "错误描述信息: " << e.what() << endl;
	//		return false;
	//	}
	//}


private:
	//存储设备信息列表
	static const GxIAPICPP::gxdeviceinfo_vector m_dinfovector;
	static atomic<unsigned int> m_counter;
	const unsigned int m_index;
	CGXDevicePointer m_device;
	CGXStreamPointer m_stream;
	CGXFeatureControlPointer m_featureControl;

	CaptureEventHandler m_captureEventHandler;

	//用于异步调用中回传数据
	promise<void*> dataProm;
	future<void*> dataFut;

	//保护状态
	mutex m_stateMutex;
	condition_variable m_stateCv;
};
