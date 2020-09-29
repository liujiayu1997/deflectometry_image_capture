#include "captureThread.h"

captureThread::captureThread(int index) : QObject(),m_index(index) {}


void captureThread::camera_init()
{
	m_mutex.lock();
	m_camera = std::make_shared<DahengCamera>(m_index);
    bool flag = m_camera->init();
	m_camera->getPictureResolution(im_width, im_height);
	m_camera->startCaptureCallback();
	emit image_size(im_width, im_height);
	m_mutex.unlock();
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
			image_name = QString::fromStdString("phase_x") + QString::number(fringe_num) + QString::fromStdString("_") + QString::number(fringe_step) + QString::fromStdString("-") + QString::number(i) + QString::fromStdString(".bmp");
		else
			image_name = QString::fromStdString("phase_y") + QString::number(fringe_num) + QString::fromStdString("_") + QString::number(fringe_step) + QString::fromStdString("-") + QString::number(i) + QString::fromStdString(".bmp");

		QString image_path = image_root + QString::fromStdString("/") + image_name;
		QImage image_capture(static_cast<uchar*>(data), im_width, im_height, QImage::Format_RGB888);
		QImageWriter image_save(image_path);
		image_save.write(image_capture.rgbSwapped().convertToFormat(QImage::Format_Grayscale8));
		emit image(data);
	}

	emit save_image_success();
}

void captureThread::set_auto_exposition(QString exposition_mode)
{
	m_camera->setExposureAuto(exposition_mode.toStdString());
}

void captureThread::set_exposition(float exposition_time)
{
	m_camera->setExposureAuto("Off");
	m_camera->setExposureTime(exposition_time);
}

void captureThread::set_auto_white_balance(QString white_balance_mode)
{
	m_camera->setBalanceWhiteAuto(white_balance_mode.toStdString());
}

void captureThread::set_white_balance(float r, float g, float b)
{
	m_camera->setBalanceWhiteAuto("Off");
	m_camera->setBalanceRatio(r, g, b);
}

void captureThread::set_auto_gain(QString gain_mode)
{
	m_camera->setGainAuto(gain_mode.toStdString());
}

void captureThread::set_gain(float gain)
{
	m_camera->setGainAuto("Off");
	m_camera->setGain(gain);
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

