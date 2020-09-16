#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_deflectometry_image_capture.h"
#include <QMessageBox>
#include <QFileDialog>
#include <memory>
#include <DahengCamera.h>
#include "HighGraphicsView.h"
#include <string>

#if defined(_MSC_VER) && (_MSC_VER >= 1600)    
# pragma execution_character_set("utf-8")    
#endif

class deflectometry_image_capture : public QMainWindow
{
    Q_OBJECT

public:
    deflectometry_image_capture(QWidget *parent = Q_NULLPTR);

private:
    Ui::deflectometry_image_captureClass ui;
    QString image_path;


public slots:
    void set_image_path();
    void camera_init();
    void show_picture();
    void close_camera();
    void show_fringer();
    void gather_image();
};
