#pragma once

#include <QThread>
#include <QMutex>
#include "Camera.h"
#include "DahengCamera.h"
#include <memory>
#include <QMessageBox>
#include <QDebug>
#include <QImageWriter>
class captureThread : public QObject
{
	Q_OBJECT
	
public:
	captureThread(int index);
public slots:
	void image_capture();
	void camera_init();
	void save_image(bool is_vertival, int fringe_num, int fringe_step, int average_num, QString image_root);

	void set_auto_exposition(QString exposition_mode);
	void set_exposition(float exposition_time);
	void set_auto_white_balance(QString white_balance_mode);
	void set_white_balance(float r, float g, float b);
	void set_auto_gain(QString gain_mode);
	void set_gain(float gain);

signals:
	void image(void* data);  //返回图像信号
	void image_size(int im_width, int im_height);  //返回图像大小

	// 标志着次线程采集数据完毕
	void save_image_success();

private:
	int m_index;
	std::shared_ptr<Camera> m_camera;
	int im_width;
	int im_height;
	QMutex m_mutex;
};