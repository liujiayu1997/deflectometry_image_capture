#pragma once

#include <QWidget>
#include "ui_ImageShow.h"
#include <DahengCamera.h>
#include "HighGraphicsView.h"
#include "captureThread.h"
#include <string>
#include <Qthread>
#include <QTimer>
#include <QtConcurrent>
#include <QtCore>
#include <QMessageBox>
#include <QFileDialog>
#include <memory>

class ImageShow : public QWidget
{
	Q_OBJECT

public:
	ImageShow(int index, QWidget *parent = Q_NULLPTR);
	~ImageShow();

private:
	Ui::ImageShow ui;
	// ������
	static int ui_counter;
	int m_index;

	// �̶߳���
	std::shared_ptr<captureThread> m_camera;
	std::unique_ptr<QThread> m_thread;

	// ��ʱ������
	std::shared_ptr<QTimer> m_timer;
	QString image_path;

public:
	void set_image_path(QString path_root, int current_num);
	// �����ӽ���ͼ�񴢴��Ƿ�����ı�־λ
	static int save_success_num;

public slots:
	// ͼƬ��ʾ�ۺ���
	void camera_init();
	void show_picture();
	void close_camera();

	//��������޸Ĳۺ���
	void on_set_exposition();
	void on_set_white_balance();
	void on_set_gain();
	void on_save_image(bool is_vectical, int fringe_num, int fringe_step, int average_num);
	void save_success();

signals:
	// ͼ����ʾ�ź�
	void camera_init_signal();
	// �����������
	void set_auto_exposition(QString);
	void set_exposition(float);
	void set_auto_white_balance(QString);
	void set_white_balance(float, float, float);
	void set_auto_gain(QString);
	void set_gain(float);

	// ͼ�񴢴��ź�
	void save_image(bool, int, int, int, QString);

	// ����UI����ɹ��ź�
	void all_ui_save_success();
};
