/**
*  @file BFS.h
*
*  Class for breadth first search
*
*  Author : Vladimir O. Ovchinnikov
**/

#pragma once

#include "QDebug"
#include <QObject>
#include <QPoint>
#include <QMutex>

#include <float.h>

// Вершина графа
struct Node
{
	Node(QPoint coords)
	{
		this->coords = coords;
	}
	QPoint coords;
	Node* prev_node { nullptr };
	double weight { DBL_MAX };
	bool visited { false };

	bool operator ==(const Node &other) const
	{
		return this->coords == other.coords;
	}
};

class BFS : public QObject
{
	Q_OBJECT
public:
	BFS(std::vector<QPoint> graph, QPoint start, QPoint end, int side_size);

	void pointToNode();
	void setPointData(QPoint start, QPoint end);
signals:
	void finished();
	void sendPath(std::vector<QPoint> path);
public slots:
	void run();
private:

	std::vector<Node*> node_graph;
	Node *node_start;
	Node *node_end;

	std::vector<QPoint> point_graph;
	QPoint point_start;
	QPoint point_end;

	QMutex mutex; // блокировщик доступа к данным начала и конца пути

	int side_size { 0 }; // текущая стороны клетки
	bool change_pos { true };  // флаг изменения положения конечной точки

	std::vector<Node*> getNeighborNodes(Node *node);
};

