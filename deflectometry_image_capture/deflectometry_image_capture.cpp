#include "deflectometry_image_capture.h"

deflectometry_image_capture::deflectometry_image_capture(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
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
		QMessageBox::warning(this, tr("相机初始化"), tr("相机初始化失败"));
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

		if (!ui.graphicsView->initCamera() || !ui.graphicsView_2->initCamera())
		{
			QMessageBox::warning(this, tr("相机初始化"), tr("单个相机初始化失败"));
		}
		else
		{
			QMessageBox::information(this, tr("相机初始化"), tr("相机初始化成功"));
			ui.graphicsView->moveToThread(&graphics1_thread);
			ui.graphicsView_2->moveToThread(&graphics2_thread);
		}
	}
		
}

void deflectometry_image_capture::show_picture()
{
	if (ui.comboBox->currentIndex() == 0)
		ui.graphicsView->setImage();
	else
		ui.graphicsView_2->setImage();
}

void deflectometry_image_capture::close_camera()
{
}

void deflectometry_image_capture::show_fringer()
{
}

void deflectometry_image_capture::gather_image()
{
}