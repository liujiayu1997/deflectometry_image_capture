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
	// 相机编号
	static int ui_counter;
	std::string m_ID;

	// VIMBA 控制器对象
	ApiController& m_ApiController;

	// 显示图片
	QImage m_image;

	// 线程对象
	//std::unique_ptr<QThread> m_thread;

public:
	// 所有子界面图像储存是否结束的标志位
	static int save_success_num;
	VmbErrorType CopyToImage(VmbUchar_t* pInBuffer, VmbPixelFormat_t ePixelFormat, QImage& pOutImage, const float* Matrix = NULL);

public slots:
	// 图片显示槽函数
	void show_picture();
	void close_camera();

	//相机参数修改槽函数
	void on_set_exposition();
	void on_set_white_balance();
	void on_set_gain();

	// 获取帧
	void OnFrameReady(int status);

signals:
};
