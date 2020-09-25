#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_deflectometry_image_capture.h"
#include <QMessageBox>
#include <QFileDialog>
#include "projector.h"
#include "fringe.h"
#include <memory>
#include <DahengCamera.h>
#include "HighGraphicsView.h"
#include "captureThread.h"
#include <string>
#include <Qthread>
#include <QTimer>
#include <QtConcurrent>
#include <QtCore>

#if defined(_MSC_VER) && (_MSC_VER >= 1600)    
# pragma execution_character_set("utf-8")    
#endif

class deflectometry_image_capture : public QMainWindow
{
    Q_OBJECT

public:
    deflectometry_image_capture(QWidget *parent = Q_NULLPTR);
    ~deflectometry_image_capture();

private:
    Ui::deflectometry_image_captureClass ui;
    QString image_path;
    captureThread* m_camera_one;
    QThread m_thread_one;
    captureThread* m_camera_two;
    QThread m_thread_two;
    QTimer* m_timer;
    Projector m_projector;
    std::vector<int> fringe_sequence;
    std::vector<std::shared_ptr<FringeSequence>> display_fringe;
    int fringe_width;	//���Ƶĳߴ�
    int fringe_height;

public slots:
    void set_image_path();
    void camera_init();
    void show_picture();
    void close_camera();
    void show_fringer();
    void gather_image();
    void projector_init();
signals:
    //void capture_image();
    //void stop_capture_image();
    void start_save_image();
    void camera_init_signal();
};
