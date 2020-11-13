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
    // �����ʼ���ؼ�
    ApiController m_ApiController;
    std::vector<std::string> m_cameras;

    // UI�������
    Ui::deflectometry_image_captureClass ui;
    QString image_path;

    //ͶӰ�ǲ���
    std::vector<int> fringe_sequence;
    int shift_step;
    int average_num;
    std::vector<std::shared_ptr<FringeSequence>> display_fringe;
    int fringe_width;	//���Ƶĳߴ�
    int fringe_height;

    // ͶӰ���߳�
    std::shared_ptr<projectorThread> m_projector_thread;
    std::unique_ptr<QThread> m_thread;

    // ��ǰʵ������
    int current_experient;

    // �ӽ���
    std::vector<std::shared_ptr<ImageShow>> m_image_ui;

    // �ɼ�ϵ��, ��־��ǰ������
    int current_fringe_num;
    int current_step;

    // ������ɼ��ɹ���־λ
    int m_save_success_num;


public slots:
    // �����ʼ���ۺ���
    void camera_init();
    void one_camera_init();

    // �����ļ��д���·���ۺ���
    void set_image_path();

    // ��ʾ�������Ʋۺ���
    void show_fringer();

    // ����ͼ��ۺ���
    void gather_image();
    void main_capture();

    // ͶӰ�ǲۺ���
    void projector_init();
    void on_init_success(bool success);
    void set_fringe_size(int width, int height);
    void on_all_save_success();

signals:
    //void capture_image();
    //void stop_capture_image();
    // ��ʼ�ɼ�һ��ͼƬ���ź�
    void start_save_image();

    // ��ʼ�ɼ�һ������ͼ���ź�
    void save_image_with_fringe(int, int, int, bool);

    // ͶӰ���߳��ź�
    void init_signal();
    void init_success(bool success);
    void show_fringe(std::shared_ptr<Fringe>);
    void all_save_success();
};
