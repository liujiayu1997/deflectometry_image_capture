#include "HighGraphicsView.h"
#include "opencv2/core/mat.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"


HighGraphicsView::HighGraphicsView(QWidget* parent)
	: QGraphicsView(parent)
{
}


HighGraphicsView::~HighGraphicsView()
{
}

void HighGraphicsView::getImage(QImage& image)
{
	image = this->image;
}

// 在GraphicsView上显示图片
void HighGraphicsView::setImage(void *data)
{
	//cv::Mat test_image(im_height, im_width, CV_8UC3, data);
	QImage image(static_cast<uchar*>(data), im_width, im_height, QImage::Format_RGB888);
    this->image = image.rgbSwapped();
	current_width = this->width() - 10;
	current_height = int(double(im_height) / double(im_width) * double(current_width));
	scene->addPixmap(QPixmap::fromImage(this->image.scaled(current_width, current_height)));
	this->setScene(scene);
	qDebug() << "Graphics thread" << QThread::currentThread();
}

// 在GraphicsView上显示图片
void HighGraphicsView::setImage(QImage image)
{
	
	//cv::Mat test_image(im_height, im_width, CV_8UC3, data);
	this->image = image;
	current_width = this->width();
	current_height = int(double(im_height) / double(im_width) * double(current_width));
	const QSize s = this->size();
	scene->addPixmap(QPixmap::fromImage(this->image.scaled(s, Qt::KeepAspectRatio)));
	this->setScene(scene);
	qDebug() << "Graphics thread" << QThread::currentThread();
}

void HighGraphicsView::setImageSize(int width, int height)
{
	this->im_height = height;
	this->im_width = width;
}
void HighGraphicsView::onWheelScrollChanged(int step)
{
	current_width += current_width / 20 * step;
	current_height += current_height / 20 * step;
	scene->clear();
	scene->addPixmap(QPixmap::fromImage(image.scaled(current_width, current_height)));
	this->setScene(scene);

	int horizontal, vertical;
	horizontal = int(double(x) * double(current_width) / double(im_width) - viewX);
	vertical = int(double(y) * double(current_height) / double(im_height) - viewY);

	this->horizontalScrollBar()->setValue(horizontal);
	this->verticalScrollBar()->setValue(vertical);
}

void HighGraphicsView::onMousePositionChanged(int x, int y)
{
	viewX = x;
	viewY = y;

	QRectF rect = this->mapToScene(this->viewport()->geometry()).boundingRect();
	x += int(rect.x() - 1); // Rectangle begin with (1,1)
	y += int(rect.y() - 1);

	x = int(double(x) * (double(im_width) / double(current_width)));
	y = int(double(y) * (double(im_height) / double(current_height)));

	this->x = x;
	this->y = y;
}

void HighGraphicsView::wheelEvent(QWheelEvent* event)
{
	QPoint numPixels, numDegrees;
	numPixels = event->pixelDelta();
	numDegrees = event->angleDelta() / 8;

	int step = 0;
	if (!numPixels.isNull())
	{
		step = numPixels.y();
	}
	else if (!numDegrees.isNull())
	{
		QPoint numSteps = numDegrees / 15;
		step = numSteps.y();
	}

	// Enlarge: +; Shrink: -
	emit wheelScrollChanged(step);
}

void HighGraphicsView::mouseMoveEvent(QMouseEvent* event)
{
	QGraphicsView::mouseMoveEvent(event);

	int x = event->x();
	int y = event->y();

	emit mousePositionChanged(x, y);
}
