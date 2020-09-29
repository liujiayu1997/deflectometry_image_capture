#include "ImageShow.h"

int ImageShow::ui_counter = 0;
int ImageShow::save_success_num = 0;


ImageShow::ImageShow(int index, QWidget *parent)
	: QWidget(parent),m_index(index)
{
	ui.setupUi(this);
	ui_counter++;
}

ImageShow::~ImageShow()
{
	ui_counter--;
	m_thread->wait();
	m_thread->quit();
}

void ImageShow::camera_init()
{
	// 初始化线程
	m_camera = std::make_shared<captureThread>(m_index);
	m_thread = std::make_unique<QThread>();
	m_camera->moveToThread(m_thread.get());
	m_timer = std::make_shared<QTimer>();

	// 信号连接,图像显示
	connect(this, SIGNAL(camera_init_signal()), m_camera.get(), SLOT(camera_init()));
	connect(m_camera.get(), SIGNAL(image_size(int, int)), ui.graphicsView, SLOT(setImageSize(int, int)));

	connect(m_timer.get(), SIGNAL(timeout()), m_camera.get(), SLOT(image_capture()));
	connect(m_camera.get(), SIGNAL(image(void*)), ui.graphicsView, SLOT(setImage(void*)));

	// 相机参数选择
	connect(this, SIGNAL(set_auto_exposition(QString)), m_camera.get(), SLOT(set_auto_exposition(QString)));
	connect(this, SIGNAL(set_exposition(float)), m_camera.get(), SLOT(set_exposition(float)));
	connect(this, SIGNAL(set_auto_white_balance(QString)), m_camera.get(), SLOT(set_auto_white_balance(QString)));
	connect(this, SIGNAL(set_white_balance(float, float, float)), m_camera.get(), SLOT(set_white_balance(float, float, float)));
	connect(this, SIGNAL(set_auto_gain(QString)), m_camera.get(), SLOT(set_auto_gain(QString)));
	connect(this, SIGNAL(set_gain(float)), m_camera.get(), SLOT(set_gain(float)));

	// 图像储存连接
	connect(this, SIGNAL(save_image(bool, int, int, int, QString)), m_camera.get(), SLOT(save_image(bool, int, int, int, QString)));
	connect(m_camera.get(), SIGNAL(save_image_success()), this, SLOT(save_success()));

	qDebug() << "Image UI thread : " << QThread::currentThread();

	m_thread->start();

	while (true)
	{
		if (m_thread->isRunning())
			break;
		else
			QThread::msleep(100);
	}

	QThread::msleep(100);
	emit camera_init_signal();
}

void ImageShow::set_image_path(QString path_root, int current_num)
{

	QString camera_path = path_root + QString::fromStdString("/") + QString::number(m_index);
	image_path = camera_path + QString::fromStdString("/") + QString::number(current_num);

	// 文件夹创建
	QDir dir_path_root(camera_path);
	if (!dir_path_root.exists())
		dir_path_root.mkpath(image_path);
	else
		dir_path_root.mkdir(QString::number(current_num));
}

void ImageShow::show_picture()
{
	m_timer->start(100);
}

void ImageShow::close_camera()
{
	m_timer->stop();
}

void ImageShow::on_set_exposition()
{
	QString exposition_mode = ui.expostion_combo_box->currentText();
	float exposition_time = ui.lineEdit->text().toFloat();
	if (exposition_mode == QString::fromStdString("Once") || exposition_mode == QString::fromStdString("Continuous"))
		emit set_auto_exposition(exposition_mode);
	else
	{
		emit set_exposition(exposition_time);
	}
}

void ImageShow::on_set_white_balance()
{
    QString white_balance_mode = ui.white_balance_combo_box->currentText();
	float white_balance_r = ui.lineEdit_2->text().toFloat();
	float white_balance_g = ui.lineEdit_3->text().toFloat();
	float white_balance_b = ui.lineEdit_4->text().toFloat();
	if (white_balance_mode == QString::fromStdString("Once") || white_balance_mode == QString::fromStdString("Continuous"))
		emit set_auto_white_balance(white_balance_mode);
	else
	{
		emit set_white_balance(white_balance_r, white_balance_g, white_balance_b);
	}

}

void ImageShow::on_set_gain()
{
	QString gain_mode = ui.gain_combo_box->currentText();
	float gain_num = ui.lineEdit_5->text().toFloat();
	if (gain_mode == QString::fromStdString("Once") || gain_mode == QString::fromStdString("Continuous"))
		emit set_auto_gain(gain_mode);
	else
	{
		emit set_gain(gain_num);
	}
}

void ImageShow::on_save_image(bool is_vectical, int fringe_num, int fringe_step, int average_num)
{
	emit save_image(is_vectical, fringe_num, fringe_step, average_num, image_path);
}

void ImageShow::save_success()
{
	save_success_num++;
	if (save_success_num == ui_counter)
		emit all_ui_save_success();
}


