#pragma once
#include <qgraphicsview.h>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QImage>
#include <memory>
#include <QScrollBar>
#include "DahengCamera.h"
#include <QMutex>

class HighGraphicsView :
	public QGraphicsView
{
	Q_OBJECT

public:
	HighGraphicsView(QWidget* parent = nullptr);
	~HighGraphicsView();
	void getImage(QImage& image); 
	void setImage();
	bool initCamera();

signals:
	void mousePositionChanged(int x, int y);
	void wheelScrollChanged(int step);
public slots:
	void onMousePositionChanged(int x, int y);
	void onWheelScrollChanged(int step);


protected:
	void wheelEvent(QWheelEvent* event);
	void mouseMoveEvent(QMouseEvent* event);

private:
	// Mouse Position
	int x, y;         // x0,y0
	int viewX, viewY; // x,y
	int im_width, im_height;
	int current_width, current_height;
	// image scene
	QImage image;
	QGraphicsScene* scene = new QGraphicsScene;
	std::shared_ptr<Camera> m_camera;
	QMutex mutex;
};