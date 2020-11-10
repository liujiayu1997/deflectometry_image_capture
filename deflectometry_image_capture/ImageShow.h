#pragma once

#include <QWidget>
#include "ui_ImageShow.h"
#include "HighGraphicsView.h"
#include <string>
#include <Qthread>
#include <QTimer>
#include <QtConcurrent>
#include <QtCore>
#include <QMessageBox>
#include <QFileDialog>
#include <memory>

#include "AVT/ApiController.h"


using namespace AVT::VmbAPI::Examples;
using AVT::VmbAPI::FramePtr;
using AVT::VmbAPI::CameraPtrVector;

class ImageShow : public QWidget
{
	Q_OBJECT

public:
	ImageShow(std::string ID, ApiController& root_controller,QWidget *parent = Q_NULLPTR);
	~ImageShow();

private:
	Ui::ImageShow ui;
	// ������
	static int ui_counter;
	std::string m_ID;

	// VIMBA ����������
	ApiController& m_ApiController;

	// ��ʾͼƬ
	QImage m_image;

	// �̶߳���
	//std::unique_ptr<QThread> m_thread;

public:
	// �����ӽ���ͼ�񴢴��Ƿ�����ı�־λ
	static int save_success_num;
	VmbErrorType CopyToImage(VmbUchar_t* pInBuffer, VmbPixelFormat_t ePixelFormat, QImage& pOutImage, const float* Matrix = NULL);

public slots:
	// ͼƬ��ʾ�ۺ���
	void show_picture();
	void close_camera();

	//��������޸Ĳۺ���
	void on_set_exposition();
	void on_set_white_balance();
	void on_set_gain();

	// ��ȡ֡
	void OnFrameReady(int status);

signals:
};
