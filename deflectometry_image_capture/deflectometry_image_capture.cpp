#include "deflectometry_image_capture.h"

deflectometry_image_capture::deflectometry_image_capture(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	m_camera = new captureThread();
	m_camera->moveToThread(&m_thread);
	//初始化相机，发送图像大小信息
	connect(this, SIGNAL(camera_init_signal()), m_camera, SLOT(camera_init()));
	connect(m_camera, SIGNAL(image_size(int, int)), ui.graphicsView, SLOT(setImageSize(int, int)));

	//开始采图，发送图像指针
	connect(this, SIGNAL(capture_image()), m_camera, SLOT(image_capture()));
	connect(m_camera, SIGNAL(image(void*)), ui.graphicsView, SLOT(setImage(void*)));

	m_thread.start();
}

deflectometry_image_capture::~deflectometry_image_capture() {
	m_thread.quit();
	m_thread.wait();
}
void deflectometry_image_capture::set_image_path()
{
	QString image_path = QFileDialog::getExistingDirectory(this, tr("设置文件路径"), "E:/picture");
	this->image_path = image_path;
	QMessageBox::information(this, tr("已选择路径"), image_path);
}
void deflectometry_image_capture::camera_init()
{
	if (!DahengCamera::initCameraList())
	{
		QMessageBox::warning(this, tr("相机列表初始化"), tr("相机初始化失败"));
	}	
	else
	{
		std::vector<std::string> camera_ID;
		DahengCamera::getCameraID(camera_ID);
		int camera_num = camera_ID.size();
		for (int i = 0; i < camera_num; i++)
		{
			ui.comboBox->addItem(tr(camera_ID[i].c_str()));
		}
		QMessageBox::information(this, tr("相机列表初始化"), tr("相机初始化成功"));
	}
	emit camera_init_signal();
}

void deflectometry_image_capture::show_picture()
{
	//if (ui.comboBox->currentIndex() == 0)
	//	ui.graphicsView->setImage();
	//else
	//	ui.graphicsView_2->setImage();

	// 发送采图信息
	emit capture_image();
}

void deflectometry_image_capture::close_camera()
{
	emit stop_capture_image();
}

void deflectometry_image_capture::show_fringer()
{
}

void deflectometry_image_capture::gather_image()
{
}