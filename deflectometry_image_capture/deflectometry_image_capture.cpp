#include "deflectometry_image_capture.h"
#include <QMetaType>

deflectometry_image_capture::deflectometry_image_capture(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	is_connected = false;

	// 注册类型
	qRegisterMetaType<std::shared_ptr<Fringe>>("std::shared_ptr<Fringe>");
}

deflectometry_image_capture::~deflectometry_image_capture() {
	m_thread->wait();
	m_thread->quit();
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
}

void deflectometry_image_capture::camera_init()
{
	// Start Vimba
	VmbErrorType err = m_ApiController.StartUp();
	if (err == VmbErrorSuccess)
	{
		CameraPtrVector cameras = m_ApiController.GetCameraList();
		for (CameraPtrVector::const_iterator iter = cameras.begin();
			cameras.end() != iter;
			++iter)
		{
			std::string strCameraName;
			std::string strCameraID;
			if (VmbErrorSuccess != (*iter)->GetName(strCameraName))
			{
				strCameraName = "[NoName]";
			}
			// If for any reason we cannot get the ID of a camera we skip it
			if (VmbErrorSuccess == (*iter)->GetID(strCameraID))
			{
				ui.comboBox->addItem(QString::fromStdString(strCameraName + " " + strCameraID));
				m_cameras.push_back(strCameraID);
			}
		}
		QMessageBox::information(this, tr("相机列表初始化"), tr("相机初始化成功"));
	}
	else
	{
		QMessageBox::warning(this, tr("相机列表初始化"), tr("相机初始化失败"));
	}
}

void deflectometry_image_capture::show_fringer()
{
	int fringe_num = ui.comboBox_2->currentText().toInt();
	std::shared_ptr<Fringe> test_fringe(new Fringe(fringe_width, fringe_height, fringe_num, 0));
	test_fringe->generate();
	emit show_fringe(test_fringe);
}

void deflectometry_image_capture::gather_image()
{
}

void deflectometry_image_capture::main_capture()
{
}

void deflectometry_image_capture::projector_init()
{
    m_projector_thread = std::make_shared<projectorThread>();
	m_thread = std::make_unique<QThread>();
	m_projector_thread->moveToThread(m_thread.get());

	// 信号连接
	connect(this, SIGNAL(init_signal()), m_projector_thread.get(), SLOT(init_projector()));
	connect(m_projector_thread.get(), SIGNAL(init_success(bool)), this, SLOT(on_init_success(bool)));
	connect(m_projector_thread.get(), SIGNAL(fringe_size(int, int)), this, SLOT(set_fringe_size(int, int)));
	connect(this, SIGNAL(show_fringe(std::shared_ptr<Fringe>)), m_projector_thread.get(), SLOT(project_fringe(std::shared_ptr<Fringe>)));
	//connect(this, SIGNAL(show_fringe()), m_projector_thread.get(), SLOT(project_fringe()));

	
	// 开启投影仪线程
	m_thread->start();

	// 发送初始化信号
	emit init_signal();
}

void deflectometry_image_capture::on_init_success(bool success)
{
	if (success)
		QMessageBox::information(this, tr("投影仪初始化"), tr("投影仪初始化成功"));
	else
		QMessageBox::critical(this, tr("投影仪初始化"), tr("没有找到投影仪"));
}

void deflectometry_image_capture::set_fringe_size(int width, int height)
{
	fringe_width = width;
	fringe_height = height;
}
