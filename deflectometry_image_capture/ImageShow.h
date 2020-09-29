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
	// 相机编号
	static int ui_counter;
	int m_index;

	// 线程对象
	std::shared_ptr<captureThread> m_camera;
	std::unique_ptr<QThread> m_thread;

	// 计时器对象
	std::shared_ptr<QTimer> m_timer;
	QString image_path;

public:
	void set_image_path(QString path_root, int current_num);
	// 所有子界面图像储存是否结束的标志位
	static int save_success_num;

public slots:
	// 图片显示槽函数
	void camera_init();
	void show_picture();
	void close_camera();

	//相机参数修改槽函数
	void on_set_exposition();
	void on_set_white_balance();
	void on_set_gain();
	void on_save_image(bool is_vectical, int fringe_num, int fringe_step, int average_num);
	void save_success();

signals:
	// 图像显示信号
	void camera_init_signal();
	// 相机参数设置
	void set_auto_exposition(QString);
	void set_exposition(float);
	void set_auto_white_balance(QString);
	void set_white_balance(float, float, float);
	void set_auto_gain(QString);
	void set_gain(float);

	// 图像储存信号
	void save_image(bool, int, int, int, QString);

	// 所有UI储存成功信号
	void all_ui_save_success();
};
