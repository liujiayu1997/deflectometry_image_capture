#pragma once
#include <qobject.h>
#include "projector.h"
#include <QThread>
#include <QDebug>

class projectorThread :
    public QObject
{
    Q_OBJECT

public:
    projectorThread();
    ~projectorThread();
public slots:
    void init_projector();
    void project_fringe(std::shared_ptr<Fringe> show_fringe);

signals:
    void init_success(bool);
    void fringe_size(int, int);

private:
    std::shared_ptr<Projector> m_projector;

};

