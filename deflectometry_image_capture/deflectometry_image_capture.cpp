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

        // 信号连接,相机一
		// 图像显示
	    connect(this, SIGNAL(camera_init_signal()), m_camera_one, SLOT(camera_init()));
	    connect(m_camera_one, SIGNAL(image_size(int, int)), ui.graphicsView, SLOT(setImageSize(int, int)));

		connect(m_timer, SIGNAL(timeout()), m_camera_one, SLOT(image_capture()));
		connect(m_camera_one, SIGNAL(image(void*)), ui.graphicsView, SLOT(setImage(void*)));

		// 相机参数选择
		connect(this, SIGNAL(set_auto_exposition_one(QString)), m_camera_one, SLOT(set_auto_exposition(QString)));
		connect(this, SIGNAL(set_exposition_one(float)), m_camera_one, SLOT(set_exposition(float)));
		connect(this, SIGNAL(set_auto_white_balance_one(QString)), m_camera_one, SLOT(set_auto_white_balance(QString)));
		connect(this, SIGNAL(set_white_balance_one(float, float, float)), m_camera_one, SLOT(set_white_balance(float, float, float)));
		connect(this, SIGNAL(set_auto_gain_one(QString)), m_camera_one, SLOT(set_auto_gain(QString)));
		connect(this, SIGNAL(set_gain_one(float)), m_camera_one, SLOT(set_gain(float)));

		// 信号连接，相机二
		connect(this, SIGNAL(camera_init_signal()), m_camera_two, SLOT(camera_init()));
		connect(m_camera_two, SIGNAL(image_size(int, int)), ui.graphicsView_2, SLOT(setImageSize(int, int)));

		connect(m_timer, SIGNAL(timeout()), m_camera_two, SLOT(image_capture()));
		connect(m_camera_two, SIGNAL(image(void*)), ui.graphicsView_2, SLOT(setImage(void*)));

		// 相机参数选择
		connect(this, SIGNAL(set_auto_exposition_two(QString)), m_camera_two, SLOT(set_auto_exposition(QString)));
		connect(this, SIGNAL(set_exposition_two(float)), m_camera_two, SLOT(set_exposition(float)));
		connect(this, SIGNAL(set_auto_white_balance_two(QString)), m_camera_two, SLOT(set_auto_white_balance(QString)));
		connect(this, SIGNAL(set_white_balance_two(float, float, float)), m_camera_two, SLOT(set_white_balance(float, float, float)));
		connect(this, SIGNAL(set_auto_gain_two(QString)), m_camera_two, SLOT(set_auto_gain(QString)));
		connect(this, SIGNAL(set_gain_two(float)), m_camera_two, SLOT(set_gain(float)));

		m_thread_one.start();
		m_thread_two.start();
		while (true)
		{
			if (m_thread_one.isRunning() && m_thread_two.isRunning())
				break;
			else
				QThread::msleep(100);
		}
		QThread::msleep(100);
		emit camera_init_signal();
	}
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
	int fringe_num = ui.comboBox_2->currentText().toInt();
	std::shared_ptr<Fringe> test_fringe(new Fringe(fringe_width, fringe_height, fringe_num, 0));
	test_fringe->generate();
	m_projector.displayFringe(test_fringe);
}

void deflectometry_image_capture::gather_image()
{

}

void deflectometry_image_capture::projector_init()
{
	if (!m_projector.init())
		QMessageBox::critical(this, tr("投影仪初始化"), tr("没有找到投影仪"));
	else
	{
		m_projector.getProjectorResolution(fringe_width, fringe_height);
	}
}

void deflectometry_image_capture::on_set_exposition_one()
{
	QString exposition_mode = ui.comboBox_3->currentText();
	float exposition_time = ui.lineEdit->text().toFloat();
	if (exposition_mode == QString::fromStdString("Once") || exposition_mode == QString::fromStdString("Continuous"))
		emit set_auto_exposition_one(exposition_mode);
	else
	{
		emit set_exposition_one(exposition_time);
	}
}

void deflectometry_image_capture::on_set_white_balance_one()
{
	QString white_balance_mode = ui.comboBox_4->currentText();
	float white_balance_r = ui.lineEdit_2->text().toFloat();
	float white_balance_g = ui.lineEdit_3->text().toFloat();
	float white_balance_b = ui.lineEdit_4->text().toFloat();
	if (white_balance_mode == QString::fromStdString("Once") || white_balance_mode == QString::fromStdString("Continuous"))
		emit set_auto_white_balance_one(white_balance_mode);
	else
	{
		emit set_white_balance_one(white_balance_r, white_balance_g, white_balance_b);
	}
}

void deflectometry_image_capture::on_set_gain_one()
{
	QString gain_mode = ui.comboBox_5->currentText();
	float gain_num = ui.lineEdit_5->text().toFloat();
	if (gain_mode == QString::fromStdString("Once") || gain_mode == QString::fromStdString("Continuous"))
		emit set_auto_gain_one(gain_mode);
	else
	{
		emit set_gain_one(gain_num);
	}
}

void deflectometry_image_capture::on_set_exposition_two()
{
	QString exposition_mode = ui.comboBox_11->currentText();
	float exposition_time = ui.lineEdit_12->text().toFloat();
	if (exposition_mode == QString::fromStdString("Once") || exposition_mode == QString::fromStdString("Continuous"))
		emit set_auto_exposition_one(exposition_mode);
	else
	{
		emit set_exposition_one(exposition_time);
	}
}

void deflectometry_image_capture::on_set_white_balance_two()
{
	QString white_balance_mode = ui.comboBox_9->currentText();
	float white_balance_r = ui.lineEdit_13->text().toFloat();
	float white_balance_g = ui.lineEdit_14->text().toFloat();
	float white_balance_b = ui.lineEdit_15->text().toFloat();
	if (white_balance_mode == QString::fromStdString("Once") || white_balance_mode == QString::fromStdString("Continuous"))
		emit set_auto_white_balance_one(white_balance_mode);
	else
	{
		emit set_white_balance_one(white_balance_r, white_balance_g, white_balance_b);
	}
}

void deflectometry_image_capture::on_set_gain_two()
{
	QString gain_mode = ui.comboBox_10->currentText();
	float gain_num = ui.lineEdit_11->text().toFloat();
	if (gain_mode == QString::fromStdString("Once") || gain_mode == QString::fromStdString("Continuous"))
		emit set_auto_gain_one(gain_mode);
	else
	{
		emit set_gain_one(gain_num);
	}
}
