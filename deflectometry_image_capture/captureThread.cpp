#include "captureThread.h"

captureThread::captureThread() : QObject() {}


void captureThread::camera_init()
{
	m_camera = std::make_shared<DahengCamera>();
    bool flag = m_camera->init();
	int im_width = 0, im_height = 0;
	m_camera->getPictureResolution(im_width, im_height);
	m_camera->startCaptureCallback();

	emit image_size(im_width, im_height);
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

