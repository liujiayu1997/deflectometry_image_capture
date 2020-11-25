#pragma once
#include <qobject.h>
#include <QImageWriter>
#include <QImage>
#include <QMutex>

class saveThread :
    public QObject
{
    Q_OBJECT

public:
    saveThread();
    ~saveThread();
    void set_save_config(std::string image_path, int fringe_num, int fringe_step, int average_num, bool is_vertical);
    QImage m_image;

public slots:
    void save_image();

signals:
    void save_success();
private:
    std::string m_image_path;
    int m_fringe_num;
    int m_fringe_step;
    int m_average_num;
    int m_current_num;
    bool m_is_vertical;
    bool whether_save;

    // ¼ÓËø
    QMutex m_mutex;

    // ¶ªÆúÖ¡Êı
    int m_frame_throw;
};

