#pragma once

#include <QWidget>
#include "ui_ImageShow.h"

class ImageShow : public QWidget
{
	Q_OBJECT

public:
	ImageShow(QWidget *parent = Q_NULLPTR);
	~ImageShow();

private:
	Ui::ImageShow ui;
};
