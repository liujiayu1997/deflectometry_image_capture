#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_deflectometry_image_capture.h"
#include "projector.h"
#include "fringe.h"
#include "ImageShow.h"
#include <cmath>


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

    //投影仪参数
    Projector m_projector;
    std::vector<int> fringe_sequence;
    int shift_step;
    int average_num;
    std::vector<std::shared_ptr<FringeSequence>> display_fringe;
    int fringe_width;	//条纹的尺寸
    int fringe_height;
    bool is_connected;

    // 当前实验组数
    int current_experient;

    // 子界面
    std::vector<std::shared_ptr<ImageShow>> m_image_ui;

    // 采集系数
    int current_fringe_num;
    int current_step;

public slots:
    void camera_init();
    void one_camera_init();
    void set_image_path();
    void show_fringer();
    void gather_image();
    void projector_init();
    void main_capture();

signals:
    //void capture_image();
    //void stop_capture_image();
    void start_save_image();
    void save_image_with_fringe(bool, int, int, int);
};
