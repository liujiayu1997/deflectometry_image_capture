#pragma once

#include <QThread>
#include "Camera.h"
#include "DahengCamera.h"
#include <memory>
#include <QMessageBox>
#include <QDebug>
class captureThread : public QObject
{
	Q_OBJECT
	
public:
	captureThread();
public slots:
	void image_capture();
	void camera_init();

signals:
	void image(void* data);  //����ͼ���ź�
	void image_size(int im_width, int im_height);  //����ͼ���С

private:
	std::shared_ptr<Camera> m_camera;
};