#pragma once
//���õĴ������ӿڣ����ž��(jyzhang@sjtu.edu.cn)��д
//ʵ�����¹��ܣ�
//���������С��ع�ʱ�䡢��ƽ�⡢���㡢�����ж�...

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
//���߳�
#include <future>

using namespace std;

class DahengCamera final: public Camera{
private:
	class CaptureEventHandler : public ICaptureEventHandler
	{
	public:
		void DoOnImageCaptured(CImageDataPointer& objImageDataPointer, void* ths) {
#ifdef __debug__
			cout << "��ʼ��ͼ"<< (GX_FRAME_STATUS_SUCCESS == objImageDataPointer->GetStatus()) << endl;
#endif
			if (GX_FRAME_STATUS_SUCCESS == objImageDataPointer->GetStatus()) {
				if (GX_PIXEL_FORMAT_BAYER_RG8 == objImageDataPointer->GetPixelFormat() || GX_PIXEL_FORMAT_BAYER_GR8 == objImageDataPointer->GetPixelFormat())
					((DahengCamera*)ths) -> dataProm.set_value(objImageDataPointer->ConvertToRGB24(GX_BIT_0_7, GX_RAW2RGB_NEIGHBOUR, false));
				else if (GX_PIXEL_FORMAT_BAYER_RG12 == objImageDataPointer->GetPixelFormat())
					((DahengCamera*)ths) -> dataProm.set_value(objImageDataPointer->ConvertToRGB24(GX_BIT_4_11, GX_RAW2RGB_NEIGHBOUR, false));
				else
					//�����ﶨ��������ʽ��ͼ������
					((DahengCamera*)ths)->dataProm.set_value(nullptr);
#ifdef __debug__
				cout << "��ͼ�ɹ�" << endl;
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
		////TODO�����ƺ����̰߳�ȫ����
		//if (m_dinfovector.empty()) {
		//	initCameraList();
		//}
		assert(m_counter++ < m_dinfovector.size());
	};
	virtual ~DahengCamera() { m_counter--; };

	//������ʵ����DahengCamera֮ǰ����
	static bool initCameraList() {
		try
		{
			IGXFactory::GetInstance().Init();
			IGXFactory::GetInstance().UpdateDeviceList(1000, *(const_cast<GxIAPICPP::gxdeviceinfo_vector*>(&m_dinfovector)));
			cout << "���������б��ʼ�����������豸��Ϣ" << endl;
			cout << "����豸��Ϣ����Ϊ��" << m_dinfovector.size() << endl;
			return true;
		}

		catch (CGalaxyException& e)
		{
			cout << "������: " << e.GetErrorCode() << endl;
			cout << "����������Ϣ: " << e.what() << endl;
			return false;

		}
	}
	//��DahengCameraʹ����Ϻ����
	static bool uninitCamera() {
		IGXFactory::GetInstance().Uninit();
		cout << "���ͷ����������Դ" << endl;
		return true;
	}

	//�����ʼ��
	virtual bool init(){
		try{
			GxIAPICPP::gxstring strSN = m_dinfovector[m_index].GetSN();
			GxIAPICPP::gxstring strUserID = m_dinfovector[m_index].GetUserID();
			m_device = IGXFactory::GetInstance().OpenDeviceBySN(strSN, GX_ACCESS_EXCLUSIVE);
			m_featureControl = m_device->GetRemoteFeatureControl();
			if (m_featureControl->IsImplemented("TriggerMode"))
			{
				if (m_featureControl->IsWritable("TriggerMode")) {//��ѯ�Ƿ��д
					m_featureControl->GetEnumFeature("TriggerMode")->SetValue("Off");//���õ�ǰֵ
				}
			}
			uint32_t nStreamNum = m_device->GetStreamCount();
#ifdef __debug__
			cout << "�豸" << m_index << "������:" << nStreamNum << endl;
#endif
			if (nStreamNum > 0)
			{
				m_stream = m_device->OpenStream(0);
			}
			setState(State::Waiting);
			cout << "��������"<< m_index <<"��ʼ��" << endl;
			return true;
		}
		catch (CGalaxyException& e){
			cout << "������: " << e.GetErrorCode() << endl;
			cout << "����������Ϣ: " << e.what() << endl;
			return false;
		}

	}

	//����ͷ���Դ
	virtual bool uninit(){
		try{
			//�ر���ͨ��
			m_stream->Close();
			m_device->Close();
			setState(State::Close);
			return true;
		}
		catch (CGalaxyException& e){
			cout << "������: " << e.GetErrorCode() << endl;
			cout << "����������Ϣ: " << e.what() << endl;
			return false;
		}
	}

	//��ȡ��ǰ�豸��Ϣ
	virtual void printDeviceInfo(){
		cout << m_dinfovector[m_index].GetVendorName() << endl;
		cout << m_dinfovector[m_index].GetModelName() << endl;
		cout << m_dinfovector[m_index].GetDeviceID() << endl;
		cout << m_dinfovector[m_index].GetSN() << endl;
			//�����豸��Ϣ���IGXDeviceInfo�ӿ�
	}

	//��֡���㣬����
	virtual bool capture(void* &data){
		try{
			unique_lock<mutex> lk(m_stateMutex);
			if (!m_stateCv.wait_for(lk, chrono::milliseconds(1000), [&] {
				return (getState() == State::Waiting);
			})) {
				cout << "��ǰ�豸δ���������������ӻ��Ե�����" << endl;
				return false;
			}
			setState(State::Blocking);
			//������ͨ���ɼ�
			m_stream->StartGrab();

			//���豸���Ϳ�������
			m_featureControl->GetCommandFeature("AcquisitionStart")->Execute();

			CImageDataPointer objImageDataPtr;
			objImageDataPtr = m_stream->GetImage(1000);//��ʱʱ��ʹ��500ms���û����������趨
			if (objImageDataPtr->GetStatus() == GX_FRAME_STATUS_SUCCESS)
			{
#ifdef __debug__
				cout << m_featureControl->GetEnumFeature("PixelFormat")->GetValue() << endl;
				cout << ("BayerRG8" == m_featureControl->GetEnumFeature("PixelFormat")->GetValue()) << endl;
#endif
				//��ͼ�ɹ�����������֡�����Խ���ͼ�������Կ����Զ���ͼ���ʽ
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

			//ͣ��
			m_featureControl->GetCommandFeature("AcquisitionStop")->Execute();
			m_stream->StopGrab();
			setState(State::Waiting);
			m_stateCv.notify_all();
			return true;
		}
		catch (CGalaxyException& e){
			cout << "������: " << e.GetErrorCode() << endl;
			cout << "����������Ϣ: " << e.what() << endl;
			return false;
		}
	}

	//�ص��ɼ�
	virtual bool startCaptureCallback() {
		//ע��ɼ��ص�����
		unique_lock<mutex> lk(m_stateMutex);
		if (!m_stateCv.wait_for(lk, chrono::milliseconds(1000), [&] {
			return (State::Waiting == getState());
		})) {
#ifdef __debug__
			cout << "��ǰ���δ���ڵȴ�״̬���޷�����ص�ģʽ" << endl;
#endif
			return false;
		}
		if (m_featureControl->IsImplemented("TriggerMode"))
		{
			if (m_featureControl->IsWritable("TriggerMode")) {//��ѯ�Ƿ��д
				m_featureControl->GetEnumFeature("TriggerMode")->SetValue("On");//���õ�ǰֵ
			}
			
		}
		if (m_featureControl->IsImplemented("TriggerSource"))
		{
			if (m_featureControl->IsWritable("TriggerSource")) {//��ѯ�Ƿ��д
				m_featureControl->GetEnumFeature("TriggerSource")->SetValue("Software");//���õ�ǰֵ
			}
		}
		m_stream->RegisterCaptureCallback(&m_captureEventHandler, this);
		//������ͨ���ɼ�
		m_stream->StartGrab();
		//���豸���Ϳ�������
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
			cout << "��ǰ������ڶ���״̬���޷�����" << endl;
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
			cout << "��ǰ�ȴ���ʱ,�ѷ���" << endl;
#endif
			return false;
		}
		else {
			data = dataFut.get();
#ifdef __debug__
			cout << "�õ�ͼ����������" << endl;
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
			if (m_featureControl->IsWritable("TriggerMode")) {//��ѯ�Ƿ��д
				m_featureControl->GetEnumFeature("TriggerMode")->SetValue("Off");//���õ�ǰֵ
			}
		}
		dataProm = promise<void*>();
		dataFut = dataProm.get_future();
		unique_lock<mutex> lk(m_stateMutex);
		setState(State::Waiting);
		m_stateCv.notify_all();
		return true;
	}

	//����ͼƬ�ֱ���
	virtual bool setPictureResolution(int& width, int& height){
		try{
			if (m_featureControl->IsImplemented("Width"))
			{
				if (m_featureControl->IsWritable("Width"))//��ѯ�Ƿ��д
					m_featureControl->GetIntFeature("Width")->SetValue(width);//���õ�ǰֵ
				if (m_featureControl->IsWritable("Height"))//��ѯ�Ƿ��д
					m_featureControl->GetIntFeature("Height")->SetValue(height);//���õ�ǰֵ
#ifdef __debug__
				cout << "�������" << m_index << "�ķֱ���Ϊ" << width << "x" << height << endl;
#endif
			}
			return true;
		}
		catch (CGalaxyException& e){
			cout << "������: " << e.GetErrorCode() << endl;
			cout << "����������Ϣ: " << e.what() << endl;
			return false;
		}
	}
	
	//�鿴ͼƬ�ֱ���
	virtual bool getPictureResolution(int& width, int& height){
		try{
			if (m_featureControl->IsImplemented("Width"))
			{
				if (m_featureControl->IsWritable("Width"))//��ѯ�Ƿ��д
					width = m_featureControl->GetIntFeature("Width")->GetValue();//���õ�ǰֵ
				if (m_featureControl->IsWritable("Height"))//��ѯ�Ƿ��д
					height = m_featureControl->GetIntFeature("Height")->GetValue();//���õ�ǰֵ
#ifdef __debug__
				cout << "���" << m_index << "�ķֱ���Ϊ" << width << "x" << height << endl;
#endif
			}
			return true;
		}
		catch (CGalaxyException& e){
			cout << "������: " << e.GetErrorCode() << endl;
			cout << "����������Ϣ: " << e.what() << endl;
			return false;
		}
	}

	//����ͼ���ʽ
	//BayerRG8�� BayerRG10
	virtual bool setPictureFormat(string mode){
		try{
			if (m_featureControl->IsImplemented("PixelFormat"))
			{
				if (m_featureControl->IsWritable("PixelFormat"))//��ѯ�Ƿ��д
					m_featureControl->GetEnumFeature("PixelFormat")->SetValue(mode.c_str());//���õ�ǰֵ
				cout << "�������" << m_index << "�����ظ�ʽΪ" << mode << endl;
			}
			return true;
		}
		catch (CGalaxyException& e){
			cout << "������: " << e.GetErrorCode() << endl;
			cout << "����������Ϣ: " << e.what() << endl;
			return false;
		}
	}

	//�����ع����
	virtual bool setExposureTime(float t){//����ʱĬ�Ϲر��Զ��ع�
		try{
			if (m_featureControl->IsImplemented("ExposureTime"))
			{
				if (m_featureControl->IsWritable("ExposureAuto"))//��ѯ�Ƿ��д
					m_featureControl->GetEnumFeature("ExposureAuto")->SetValue("Off");//�ر��Զ��ع�
				if (m_featureControl->IsWritable("ExposureTime")){//��ѯ�Ƿ��д
					m_featureControl->GetFloatFeature("ExposureTime")->SetValue(t);//���õ�ǰֵ
					cout << "�������" << m_index << "���ع�ʱ��Ϊ" << t << "us" << endl;
				}
			}
			return true;
		}
		catch (CGalaxyException& e){
			cout << "������: " << e.GetErrorCode() << endl;
			cout << "����������Ϣ: " << e.what() << endl;
			return false;
		}
	}
	virtual bool setExposureAuto(string mode){//mode: Off, Continuous, Once
		try{
			if (m_featureControl->IsImplemented("ExposureAuto"))
			{
				if (m_featureControl->IsWritable("ExposureAuto")){//��ѯ�Ƿ��д
					m_featureControl->GetEnumFeature("ExposureAuto")->SetValue(mode.c_str());//���õ�ǰֵ
					cout << "�������" << m_index << "���Զ��ع�ģʽΪ" << mode << endl;
				}
			}
			return true;
		}
		catch (CGalaxyException& e){
			cout << "������: " << e.GetErrorCode() << endl;
			cout << "����������Ϣ: " << e.what() << endl;
			return false;
		}
	}

	//���ð�ƽ��
	virtual bool setBalanceWhiteAuto(string mode){//mode: Off, Continuous, Once
		try{
			if (m_featureControl->IsImplemented("BalanceWhiteAuto"))
			{
				if (m_featureControl->IsWritable("BalanceWhiteAuto")){//��ѯ�Ƿ��д
					m_featureControl->GetEnumFeature("BalanceWhiteAuto")->SetValue(mode.c_str());//���õ�ǰֵ
					cout << "�������" << m_index << "���Զ���ƽ��ģʽΪ" << mode << endl;
				}
			}
			return true;
		}
		catch (CGalaxyException& e){
			cout << "������: " << e.GetErrorCode() << endl;
			cout << "����������Ϣ: " << e.what() << endl;
			return false;
		}
	}
	virtual bool setBalanceRatio(float r, float g, float b){
		try{
			if (m_featureControl->IsImplemented("BalanceWhiteAuto"))
			{
				if (m_featureControl->IsWritable("BalanceWhiteAuto")){//��ѯ�Ƿ��д
					m_featureControl->GetEnumFeature("BalanceWhiteAuto")->SetValue("Off");//���õ�ǰֵ
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

				cout << "�������" << m_index << "�İ�ƽ��ϵ��Ϊ" << endl
					<< "r = " << r << endl
					<< "g = " << g << endl
					<< "b = " << b << endl;
			}
			return true;
		}
		catch (CGalaxyException& e){
			cout << "������: " << e.GetErrorCode() << endl;
			cout << "����������Ϣ: " << e.what() << endl;
			return false;
		}
	}

	//��������
	virtual bool setGainAuto(string mode, float min = 0, float max = 23.9){
		try{
			if (m_featureControl->IsImplemented("GainAuto"))
			{
				if (m_featureControl->IsWritable("GainAuto")){//��ѯ�Ƿ��д
					m_featureControl->GetEnumFeature("GainAuto")->SetValue(mode.c_str());//���õ�ǰֵ
					cout << "�������" << m_index << "���Զ��Զ�ģʽΪ" << mode << endl;
				}
				if ("Continuous" == mode || "Once" == mode){
					if (m_featureControl->IsWritable("AutoGainMin"))
						m_featureControl->GetFloatFeature("AutoGainMin")->SetValue(min);
					if (m_featureControl->IsWritable("AutoGainMax"))
						m_featureControl->GetFloatFeature("AutoGainMax")->SetValue(max);
					cout << "����Զ�������СֵΪ" << min << "�����ֵΪ" << max << endl;
				}
			}
			return true;
		}
		catch (CGalaxyException& e){
			cout << "������: " << e.GetErrorCode() << endl;
			cout << "����������Ϣ: " << e.what() << endl;
			return false;
		}
	}
	virtual bool setGain(float g){
		try{
			if (m_featureControl->IsImplemented("Gain"))
			{
				if (m_featureControl->IsWritable("GainAuto"))//��ѯ�Ƿ��д
					m_featureControl->GetEnumFeature("GainAuto")->SetValue("Off");//�ر��Զ��ع�
				if (m_featureControl->IsWritable("Gain")){//��ѯ�Ƿ��д
					m_featureControl->GetFloatFeature("Gain")->SetValue(g);//���õ�ǰֵ
					cout << "�������" << m_index << "������Ϊ" << g << "dB" << endl;
				}
			}
			return true;
		}
		catch (CGalaxyException& e){
			cout << "������: " << e.GetErrorCode() << endl;
			cout << "����������Ϣ: " << e.what() << endl;
			return false;
		}
	}


	//������������ļ�
	bool exportConfigFile(string filePathToName = "camera/default"){
		try{
			m_device->ExportConfigFile((filePathToName + "_" + to_string(m_index)).c_str());
		}
		catch (CGalaxyException& e){
			cout << "������: " << e.GetErrorCode() << endl;
			cout << "����������Ϣ: " << e.what() << endl;
			return false;
		}
	}

	//������������ļ�
	bool importConfigFile(string filePathToName){
		try{
			m_device->ImportConfigFile(filePathToName.c_str());
			return true;
		}
		catch (CGalaxyException& e){
			cout << "������: " << e.GetErrorCode() << endl;
			cout << "����������Ϣ: " << e.what() << endl;
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
	//			//�ر���ͨ��
	//			objStreamPtr->Close();
	//		}
	//		return true;
	//	}
	//	catch (CGalaxyException& e){
	//		cout << "������: " << e.GetErrorCode() << endl;
	//		cout << "����������Ϣ: " << e.what() << endl;
	//		return false;
	//	}
	//}


private:
	//�洢�豸��Ϣ�б�
	static const GxIAPICPP::gxdeviceinfo_vector m_dinfovector;
	static atomic<unsigned int> m_counter;
	const unsigned int m_index;
	CGXDevicePointer m_device;
	CGXStreamPointer m_stream;
	CGXFeatureControlPointer m_featureControl;

	CaptureEventHandler m_captureEventHandler;

	//�����첽�����лش�����
	promise<void*> dataProm;
	future<void*> dataFut;

	//����״̬
	mutex m_stateMutex;
	condition_variable m_stateCv;
};
