#pragma once

#include <QThread>
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
	captureThread();
public slots:
	void image_capture();
	void camera_init();
	void save_image(bool is_vertival, int fringe_num, int fringe_step, int average_num, QString image_root);

signals:
	void image(void* data);  //����ͼ���ź�
	void image_size(int im_width, int im_height);  //����ͼ���С

private:
	std::shared_ptr<Camera> m_camera;
	int im_width;
	int im_height;
};