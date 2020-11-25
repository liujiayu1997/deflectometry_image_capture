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
#include <QImageWriter>
#include <QMutex>
#include <QDebug>
#include "AVT/ApiController.h"
#include "saveThread.h"

using namespace AVT::VmbAPI::Examples;
using AVT::VmbAPI::FramePtr;
using AVT::VmbAPI::CameraPtrVector;

class ImageShow : public QWidget
{
	Q_OBJECT

public:
	ImageShow(std::string ID, QWidget *parent = Q_NULLPTR);
	~ImageShow();

private:
	Ui::ImageShow ui;
	// 相机编号
	static int ui_counter;
	std::string m_ID;

	// VIMBA 控制器对象
	ApiController m_ApiController;

	// 显示图片
	QImage m_image, m_image_save;

	// 线程对象
	//std::unique_ptr<QThread> m_thread;

	// 线程锁
	QMutex m_mutex;

	// 储存路径
	std::string m_saving_root;

	// 存取图片线程
	std::shared_ptr<saveThread> m_save_thread;
	std::unique_ptr<QThread> m_thread;

public:
	// 所有子界面图像储存是否结束的标志位
	static int save_success_num;
	VmbErrorType CopyToImage(VmbUchar_t* pInBuffer, VmbPixelFormat_t ePixelFormat, QImage& pOutImage, const float* Matrix = NULL);
	void set_save_path(QString path_root, int current_experient_num);

public slots:
	// 图片显示槽函数
	void show_picture();
	void close_camera();

	// 获取帧
	void OnFrameReady(int status);
	void start_save_image(int fringe_num, int fringe_step, int average_num, bool vertical);

	// 采集成功信号
	void on_save_success();

signals:
	void save_success();
	void save_image_data();
};
