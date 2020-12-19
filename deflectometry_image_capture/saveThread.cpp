#include "saveThread.h"

saveThread::saveThread()
{
	whether_save = false;
	m_fringe_num = 0;
	m_fringe_step = 0;
	m_average_num = 0;
	m_is_vertical = false;
	m_frame_throw = 3;
}

saveThread::~saveThread()
{
}

void saveThread::set_save_config(std::string image_path, int fringe_num, int fringe_step, int average_num, bool is_vertical)
{
	m_image_path = image_path;
	m_fringe_num = fringe_num;
	m_fringe_step = fringe_step;
	m_average_num = average_num;
	m_is_vertical = is_vertical;
	whether_save = true;
	m_current_num = 0;
}

void saveThread::save_image()
{
	if (whether_save)
	{
		if (m_current_num < m_frame_throw)
		{
			m_current_num++;
			return;
		}
		if (m_current_num == m_average_num + m_frame_throw)
		{
			whether_save = false;
			emit save_success();
		}
		else
		{
			// save image
			QString image_name;
			if (m_is_vertical)
				image_name = QString::fromStdString("phase_x") + QString::number(m_fringe_num) + QString::fromStdString("_") + QString::number(m_fringe_step) + QString::fromStdString("-") + QString::number(m_current_num - m_frame_throw) + QString::fromStdString(".bmp");
			else
				image_name = QString::fromStdString("phase_y") + QString::number(m_fringe_num) + QString::fromStdString("_") + QString::number(m_fringe_step) + QString::fromStdString("-") + QString::number(m_current_num - m_frame_throw) + QString::fromStdString(".bmp");
			QString image_path = QString::fromStdString(m_image_path) + QString::fromStdString("/") + image_name;
			QImageWriter image_save(image_path);
			m_mutex.lock();
			image_save.write(m_image);
			m_mutex.unlock();
			m_current_num++;
		}
	}
	else
		return;
}
