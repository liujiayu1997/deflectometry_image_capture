#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_deflectometry_image_capture.h"
#include "projector.h"
#include "fringe.h"
#include "ImageShow.h"
#include <cmath>
#include "projectorThread.h"
#include "AVT/ApiController.h"


using namespace AVT::VmbAPI::Examples;
using AVT::VmbAPI::FramePtr;
using AVT::VmbAPI::CameraPtrVector;

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
    // 相机初始化控件
    ApiController m_ApiController;
    std::vector<std::string> m_cameras;

    // UI组件参数
    Ui::deflectometry_image_captureClass ui;
    QString image_path;

    //投影仪参数
    std::vector<int> fringe_sequence;
    int shift_step;
    int average_num;
    std::vector<std::shared_ptr<FringeSequence>> display_fringe;
    int fringe_width;	//条纹的尺寸
    int fringe_height;

    // 投影仪线程
    std::shared_ptr<projectorThread> m_projector_thread;
    std::unique_ptr<QThread> m_thread;

    // 当前实验组数
    int current_experient;

    // 子界面
    std::vector<std::shared_ptr<ImageShow>> m_image_ui;

    // 采集系数, 标志当前的条纹
    int current_fringe_num;
    int current_step;

    // 各相机采集成功标志位
    int m_save_success_num;


public slots:
    // 相机初始化槽函数
    void camera_init();
    void one_camera_init();

    // 设置文件夹储存路径槽函数
    void set_image_path();

    // 显示测试条纹槽函数
    void show_fringer();

    // 储存图像槽函数
    void gather_image();
    void main_capture();

    // 投影仪槽函数
    void projector_init();
    void on_init_success(bool success);
    void set_fringe_size(int width, int height);
    void on_all_save_success();

signals:
    //void capture_image();
    //void stop_capture_image();
    // 开始采集一组图片的信号
    void start_save_image();

    // 开始采集一次条纹图的信号
    void save_image_with_fringe(int, int, int, bool);

    // 投影仪线程信号
    void init_signal();
    void init_success(bool success);
    void show_fringe(std::shared_ptr<Fringe>);
    void all_save_success();
};
