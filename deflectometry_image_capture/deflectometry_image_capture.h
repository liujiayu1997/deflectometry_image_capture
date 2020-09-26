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
    // UI组件参数
    Ui::deflectometry_image_captureClass ui;
    QString image_path;
    captureThread* m_camera_one;
    QThread m_thread_one;
    captureThread* m_camera_two;
    QThread m_thread_two;
    QTimer* m_timer;

    //投影仪参数
    Projector m_projector;
    std::vector<int> fringe_sequence;
    std::vector<std::shared_ptr<FringeSequence>> display_fringe;
    int fringe_width;	//条纹的尺寸
    int fringe_height;

    // 当前实验组数
    static int current_experient;

public slots:
    void set_image_path();
    void camera_init();
    void show_picture();
    void close_camera();
    void show_fringer();
    void gather_image();
    void projector_init();

    // 相机参数设置
    void on_set_exposition_one();
    void on_set_white_balance_one();
    void on_set_gain_one();

    void on_set_exposition_two();
    void on_set_white_balance_two();
    void on_set_gain_two();

signals:
    //void capture_image();
    //void stop_capture_image();
    void start_save_image();
    void camera_init_signal();
    void save_image_with_fringe(bool, int, int, int, QString);

    // 相机参数设置
    void set_auto_exposition_one(QString);
    void set_exposition_one(float);
    void set_auto_white_balance_one(QString);
    void set_white_balance_one(float, float, float);
    void set_auto_gain_one(QString);
    void set_gain_one(float);

    void set_auto_exposition_two(QString);
    void set_exposition_two(float);
    void set_auto_white_balance_two(QString);
    void set_white_balance_two(float, float, float);
    void set_auto_gain_two(QString);
    void set_gain_two(float);
};
