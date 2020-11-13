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

#include "AVT/ApiController.h"


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
	// ������
	static int ui_counter;
	std::string m_ID;

	// VIMBA ����������
	ApiController m_ApiController;

	// ��ʾͼƬ
	QImage m_image;

	// �Ƿ��ȡ
	bool whether_save;
	bool is_vertical;
	int m_fringe_num;
	int m_fringe_step;
	int m_average_num;
	int m_current_count;
	std::string m_saving_root;

	// �̶߳���
	//std::unique_ptr<QThread> m_thread;

public:
	// �����ӽ���ͼ�񴢴��Ƿ�����ı�־λ
	static int save_success_num;
	VmbErrorType CopyToImage(VmbUchar_t* pInBuffer, VmbPixelFormat_t ePixelFormat, QImage& pOutImage, const float* Matrix = NULL);
	void set_save_path(QString path_root, int current_experient_num);

public slots:
	// ͼƬ��ʾ�ۺ���
	void show_picture();
	void close_camera();

	// ��ȡ֡
	void OnFrameReady(int status);
	void start_save_image(int fringe_step, int fringe_num, int average_num, bool vertical);

signals:
	void save_success();
};
