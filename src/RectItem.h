/**
*  @file SearchGraph.h
*
*  Rect Item
*
*  Author : Vladimir O. Ovchinnikov
**/

#pragma once

#include <QPainter>
#include <QGraphicsItem>

class RectItem: public QGraphicsItem
{
public:
	RectItem();
	RectItem(qreal x, qreal y, uint16_t width, uint16_t height, bool isWall = false);

	qreal getX() { return x; }
	qreal getY() { return y; }
	uint16_t getW() { return w; }
	uint16_t getH() { return h; }

	QRectF boundingRect() const;

	void paint(QPainter * painter,
			   const QStyleOptionGraphicsItem * option,
			   QWidget * widget);

	// Состояния: объект является началом пути поиска
	bool pressed { false };
	// Состояния: объект является препятствием
	bool isWall { false };
	// Состояния: объект является концом пути поиска
	bool moved { false };

	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

private:
	qreal x { 0 }; // Координата X фигуры
	qreal y { 0 }; // Координата Y фигуры
	uint16_t w { 0 }; // ширина фигуры
	uint16_t h { 0 }; // высота фигуры
};

