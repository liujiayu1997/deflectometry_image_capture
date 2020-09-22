#include "deflectometry_image_capture.h"

deflectometry_image_capture::deflectometry_image_capture(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	//初始化计时器
	m_timer = new QTimer();
}

deflectometry_image_capture::~deflectometry_image_capture() {
	m_thread_one.quit();
	m_thread_two.quit();
	m_thread_one.wait();
	m_thread_two.wait();
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

		//初始化相机

		//定义在子线程里的QObject
		m_camera_one = new captureThread();
		m_camera_one->moveToThread(&m_thread_one);

		m_camera_two = new captureThread();
		m_camera_two->moveToThread(&m_thread_two);

        // 信号连接
	    connect(this, SIGNAL(camera_init_signal()), m_camera_one, SLOT(camera_init()));
	    connect(m_camera_one, SIGNAL(image_size(int, int)), ui.graphicsView, SLOT(setImageSize(int, int)));

		connect(m_timer, SIGNAL(timeout()), m_camera_one, SLOT(image_capture()));
		connect(m_camera_one, SIGNAL(image(void*)), ui.graphicsView, SLOT(setImage(void*)));

		// 信号连接
		connect(this, SIGNAL(camera_init_signal()), m_camera_two, SLOT(camera_init()));
		connect(m_camera_two, SIGNAL(image_size(int, int)), ui.graphicsView_2, SLOT(setImageSize(int, int)));

		connect(m_timer, SIGNAL(timeout()), m_camera_two, SLOT(image_capture()));
		connect(m_camera_two, SIGNAL(image(void*)), ui.graphicsView_2, SLOT(setImage(void*)));

		m_thread_one.start();
		m_thread_two.start();
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

    //开始采图，发送图像指针
	m_timer->start(100);
}

void deflectometry_image_capture::close_camera()
{
	m_timer->stop();
}

void deflectometry_image_capture::show_fringer()
{
}

void deflectometry_image_capture::gather_image()
{

}