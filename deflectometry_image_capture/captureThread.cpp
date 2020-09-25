#include "captureThread.h"

captureThread::captureThread() : QObject() {}


void captureThread::camera_init()
{
	m_camera = std::make_shared<DahengCamera>();
    bool flag = m_camera->init();
	m_camera->getPictureResolution(im_width, im_height);
	m_camera->startCaptureCallback();

	emit image_size(im_width, im_height);
}

void captureThread::save_image(bool is_vertival, int fringe_num, int fringe_step, int average_num, QString image_root)
{
	void* data = nullptr;
	for (int i = 0; i < average_num; i++)
	{
		m_camera->emitTriggerSoftware();
		while (true)
		{
			if (m_camera->getCallbackData(data, 100))
				break;
		}
		qDebug() << "Image capture thread" << QThread::currentThread();
		QString image_name;
		if (is_vertival)
			image_name = QString::fromStdString("phase_x") + QString::number(fringe_num) + QString::fromStdString("_") + QString::number(fringe_step) + QString::number(fringe_step) + QString::fromStdString("-") + QString::number(i) + QString::fromStdString(".bmp");
		else
			image_name = QString::fromStdString("phase_y") + QString::number(fringe_num) + QString::fromStdString("_") + QString::number(fringe_step) + QString::number(fringe_step) + QString::fromStdString("-") + QString::number(i) + QString::fromStdString(".bmp");

		QString image_path = image_root + image_name;
		QImage image_capture(static_cast<uchar*>(data), im_width, im_height, QImage::Format_RGB888);
		QImageWriter image_save(image_path);
		image_save.write(image_capture.rgbSwapped().convertToFormat(QImage::Format_Grayscale8));
		emit image(data);
	}
}

void captureThread::image_capture() {

		void* data = nullptr;
		m_camera->emitTriggerSoftware();
		while (true)
		{
			if (m_camera->getCallbackData(data, 100))
				break;
		}
		qDebug() << "Image capture thread" << QThread::currentThread();
		emit image(data);
}

