/**
*  @file SearchGraph.h
*
*  GraphicView object
*
*  Author : Vladimir O. Ovchinnikov
**/

#pragma once

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItemGroup>
#include <QThread>

#include <QTimer>

#include "RectItem.h"
#include "BFS.h"

#include <float.h>

class SearchGraph: public QGraphicsView
{
	Q_OBJECT
public:
	explicit SearchGraph(int width, int height, QWidget *parent = nullptr);
	void paintField(uint16_t width, uint16_t height, bool isGen = false);
signals:
	void showMessage(QString text, int millisec);
protected:
	void resizeEvent(QResizeEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void wheelEvent(QWheelEvent *event);
private	slots:
	    void getPath(std::vector<QPoint> path);
private:
	QGraphicsScene      *scene;
	QGraphicsItemGroup  *group;
	QGraphicsItemGroup  *group_lines;

	RectItem *last_start { nullptr };
	RectItem *last_end { nullptr };

	QPoint start;
	QPoint end;

	// Ручка прорисовки линий пути
	QPen lines_pen;

	// Поток для выполнения алгоритма поиска в ширину
	QThread *thread { nullptr };
	// QObject для выполнения поиска по ширине
	BFS *bfs { nullptr };

	// Таймер для фиксации положения мыши при выборе конечной точки
	QTimer mouse_move_timer;
	// Таймер для фиксации изменения размера поля
	QTimer fiels_resize_timer;

	// Последнее положение мыши пользователя
	QPoint last_mouse_point;

	uint16_t field_width { 0 }; // ширина поля в клетках
	uint16_t field_height { 0 }; // высота поля в клетках
	uint16_t square_side { 0 };  // сторона квадрата клетки

	// Вектор хранения координат середин клеток поля без препятствий
	std::vector<QPoint> graph;
	// Вектор для хранения положений клеток без препятствий
	std::vector<QPoint> unwall_graph;

	// Флаг изменения масштаба
	bool scaled { false };


	void deleteItemsFromGroup(QGraphicsItemGroup *group);
	void paintEndPoint();

	RectItem* getRect(QPoint coord);
};
