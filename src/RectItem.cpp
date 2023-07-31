/**
*  @file SearchGraph.h
*
*  Rect Item implementation
*
*  Author : Vladimir O. Ovchinnikov
**/

#include "RectItem.h"

RectItem::RectItem()
{

}

RectItem::RectItem(qreal x, qreal y, uint16_t width, uint16_t height, bool isWall)
    : isWall(isWall), x(x),  y(y), w(width), h(height)
{
}

QRectF RectItem::boundingRect() const
{
	return QRectF(x, y, w, h);
}

void RectItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	QRectF rect = boundingRect();
	QBrush w_brush(Qt::blue);
	QBrush s_brush(Qt::green);
	QPen pen(Qt::black, 2);
	painter->setPen(pen);

	(void)option;
	(void)widget;

	if(isWall)
	{
		painter->setBrush(w_brush);
		painter->drawRect(rect);
	}
	else
	{
		QFont font("Segoe UI", w * 0.5, w * 0.5, 1);

		QTextOption letter_options;
		letter_options.setAlignment(Qt::AlignCenter);

		painter->setFont(font);

		if(pressed)
		{
			painter->setBrush(s_brush);
			painter->drawRect(rect);
			painter->drawText(rect, "A", letter_options);
		}
		else if(moved)
		{
			painter->drawRect(rect);
			painter->drawText(rect, "B", letter_options);
		}
		else
		{
			painter->setPen(pen);
			painter->drawRect(rect);
		}
	}
}

void RectItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	pressed = !pressed;
	update();
	QGraphicsItem::mousePressEvent(event);
}

void RectItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	moved = true;
	update();
	QGraphicsItem::mouseMoveEvent(event);
}
