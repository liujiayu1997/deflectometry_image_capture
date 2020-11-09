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
}

void ImageShow::camera_init()
{
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
}

void ImageShow::close_camera()
{
}

void ImageShow::on_set_exposition()
{
}

void ImageShow::on_set_white_balance()
{
}

void ImageShow::on_set_gain()
{
}

void ImageShow::on_save_image(bool is_vectical, int fringe_num, int fringe_step, int average_num)
{
}

void ImageShow::save_success()
{
}


