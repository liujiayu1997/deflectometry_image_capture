#include "deflectometry_image_capture.h"

deflectometry_image_capture::deflectometry_image_capture(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	is_connected = false;
}

deflectometry_image_capture::~deflectometry_image_capture() {
}

void deflectometry_image_capture::set_image_path()
{
	QString image_path = QFileDialog::getExistingDirectory(this, tr("设置文件路径"), "E:/picture");
	this->image_path = image_path;
	QMessageBox::information(this, tr("已选择路径"), image_path);

}
void deflectometry_image_capture::one_camera_init()
{
	int current_select = ui.comboBox->currentIndex();

	std::shared_ptr<ImageShow> current_ui = std::make_shared<ImageShow>(current_select);
	//  创建子ui
	this->m_image_ui.push_back(current_ui);

	connect(this, SIGNAL(save_image_with_fringe(bool, int, int, int)), current_ui.get(), SLOT(on_save_image(bool, int, int, int)));
	connect(current_ui.get(), SIGNAL(all_ui_save_success()), this, SLOT(main_capture()));

	qDebug() << "Sub ui thread : " << QThread::currentThread();

	//显示子ui
	current_ui->show();


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

	qDebug() << "Main UI thread : " << QThread::currentThread();
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
	current_experient = ui.spinBox->value();
	int current_camera_num = m_image_ui.size();
	for (int i = 0; i < current_camera_num; i++)
	{
		m_image_ui[i]->set_image_path(image_path, current_experient);
	}

	// 首次点击信号连接
	if (!is_connected)
	{
		// 图像储存信号连接
		connect(this, SIGNAL(start_save_image()), this, SLOT(main_capture()));

		// 初始化条纹周期
		int fringe_max = ui.comboBox_3->currentText().toInt();
		fringe_sequence.push_back(fringe_max);
		fringe_sequence.push_back(fringe_max - 1);
		fringe_sequence.push_back(fringe_max - int(sqrt(fringe_max)));

		//初始化相移步数以及平均数
		shift_step = ui.comboBox_4->currentText().toInt();
		average_num = ui.comboBox_5->currentText().toInt();

		for (int i = 0; i < fringe_sequence.size(); i++)
		{
			std::shared_ptr<FringeSequence> fringe(new FringeSequence);
			fringe->init(fringe_sequence[i], fringe_width, fringe_height, shift_step, 1);
			display_fringe.push_back(fringe);
		}
		for (int i = 0; i < fringe_sequence.size(); i++)
		{
			std::shared_ptr<FringeSequence> fringe(new FringeSequence);
			fringe->init(fringe_sequence[i], fringe_width, fringe_height, shift_step, 0);
			display_fringe.push_back(fringe);
		}
		is_connected = true;
	}
	current_experient++;
	ui.spinBox->setValue(current_experient);

	current_fringe_num = 0;
	current_step = 0;
	emit start_save_image();
}

void deflectometry_image_capture::main_capture()
{
	ImageShow::save_success_num = 0;
	if (current_fringe_num < display_fringe.size())
	{
		m_projector.displayFringe(display_fringe[current_fringe_num]->getFringeList()[current_step]);
		emit save_image_with_fringe(display_fringe[current_fringe_num]->isVerticalFringe(), fringe_sequence[current_fringe_num], current_step, average_num);
		if (current_step == shift_step - 1)
		{
			current_step = 0;
			current_fringe_num++;
		}
		else
			current_step++;
	}
	else
	{
		QMessageBox::information(this, tr("图像储存"), tr("图像储存结束，可以开始下一组图像"));
	}
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