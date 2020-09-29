#include "projectorThread.h"

projectorThread::projectorThread() {}
projectorThread::~projectorThread() {}


void projectorThread::init_projector()
{
	m_projector = std::make_shared<Projector>();
	if (!m_projector->init())
		emit init_success(false);
	else
	{
		int width, height;
		m_projector->getProjectorResolution(width, height);
		emit fringe_size(width, height);
		emit init_success(true);
	}
}

void projectorThread::project_fringe(std::shared_ptr<Fringe> show_fringe)
{
	qDebug() << "Projector thread : " << QThread::currentThread();
    m_projector->displayFringe(show_fringe);
}
