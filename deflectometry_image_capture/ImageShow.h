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
	// ������
	static int ui_counter;
	std::string m_ID;

	// VIMBA ����������
	ApiController m_ApiController;

	// ��ʾͼƬ
	QImage m_image, m_image_save;

	// �̶߳���
	//std::unique_ptr<QThread> m_thread;

	// �߳���
	QMutex m_mutex;

	// ����·��
	std::string m_saving_root;

	// ��ȡͼƬ�߳�
	std::shared_ptr<saveThread> m_save_thread;
	std::unique_ptr<QThread> m_thread;

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
	void start_save_image(int fringe_num, int fringe_step, int average_num, bool vertical);

	// �ɼ��ɹ��ź�
	void on_save_success();

signals:
	void save_success();
	void save_image_data();
};
