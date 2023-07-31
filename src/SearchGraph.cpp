/**
*  @file SearchGraph.h
*
*  GraphicView object implementation
*
*  Author : Vladimir O. Ovchinnikov
**/

#include "QMouseEvent"
#include "QWheelEvent"
#include "QGraphicsSceneMouseEvent"

#include "SearchGraph.h"
#include "BFS.h"

#include <math.h>
#include <queue>
#include <chrono>

SearchGraph::SearchGraph(int width, int height, QWidget *parent)
    : QGraphicsView(parent), field_width(width), field_height(height)
{
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setAlignment(Qt::AlignCenter);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	setMouseTracking(true); // включаем отслеживание положения мыши

	setMinimumWidth(345);
	setMinimumHeight(250);

	srand(time(NULL)); // рандои для создания препятствий

	scene = new QGraphicsScene();
	setScene(scene);

	group = new QGraphicsItemGroup(); // группа с клетками поля
	group_lines = new QGraphicsItemGroup(); // группа с линиями пути

	scene->addItem(group);
	scene->addItem(group_lines);
	lines_pen = QPen(Qt::red);

	// Задержка на считывание положения мыши пользователя
	mouse_move_timer.setInterval(250);
	mouse_move_timer.setSingleShot(true);
	connect(&mouse_move_timer, &QTimer::timeout, this, &SearchGraph::paintEndPoint);

	// Задержка на изменение размера окна
	fiels_resize_timer.setInterval(1000);
	fiels_resize_timer.setSingleShot(true);
	connect(&fiels_resize_timer, &QTimer::timeout, this, [=](){
		paintField(field_width, field_height);
	});
}

void SearchGraph::paintField(uint16_t w, uint16_t h, bool isGen)
{
	if(w == 0 || h == 0) return;
	field_width = w;
	field_height = h;

	// Очищаем поле
	deleteItemsFromGroup(group);
	deleteItemsFromGroup(group_lines);

	last_start = nullptr;
	last_end = nullptr;

	if(thread) // Если поток существует, то завершаем
	{
		thread->requestInterruption();
		thread = nullptr;
	}
	if(bfs)
	{
		bfs = nullptr;
	}

	QPen pen_black(Qt::black);
	QBrush brush_blue(Qt::blue);

	auto curr_w = width();
	auto curr_h = height();

	auto w_side = curr_w / w;
	auto h_side = curr_h / h;

	square_side = w_side < h_side ? w_side : h_side;

	// Получение координат центра поля
	auto x_add = (curr_w - square_side * w) / 2;
	auto y_add = (curr_h - square_side * h) / 2;

	uint x_coord; // переменная координаты X клетки
	uint y_coord; // переменная координаты Y клетки

	// Если поле обновляется, то убираем масштабирование
	// и очищаем вектор точек для поиска пути
	graph.clear();
	if(isGen)
	{
		scaled = false;
		resetTransform();
		scale(1, 1);
	}

	for(auto i = 0u; i < h; i++)
	{
		for(auto j = 0u; j < w; j++)
		{
			x_coord = x_add + j * square_side;
			y_coord = y_add + i * square_side;
			int x_centr = x_coord + square_side / 2; // координата X середины клетки
			int y_centr = y_coord + square_side / 2; // координата Y середины клетки
			if(isGen)
			{
				auto r_value = 1 + rand() % 10;
				if(r_value < 2) // 20%
				{
					group->addToGroup(new RectItem(x_coord, y_coord, square_side, square_side, true));
				}
				else
				{

					group->addToGroup(new RectItem(x_coord, y_coord, square_side, square_side));
					graph.push_back(QPoint(x_centr, y_centr));
					unwall_graph.push_back(QPoint(j , i));
				}
			}
			else
			{
				if(!unwall_graph.empty() && unwall_graph.end() != std::find(unwall_graph.begin(), unwall_graph.end(), QPoint(j, i)))
				{
					group->addToGroup(new RectItem(x_coord, y_coord, square_side, square_side));
					graph.push_back(QPoint(x_centr, y_centr));
				}
				else
				{
					group->addToGroup(new RectItem(x_coord, y_coord, square_side, square_side, true));
				}
			}
		}
	}
}

/**
 * @brief Функция очистки QGraphicView от группы объектов
 * @param group - соответствующая группа объектов
 */
void SearchGraph::deleteItemsFromGroup(QGraphicsItemGroup *group)
{
	for(auto item: scene->items(group->boundingRect()))
	{
		if(item->group() == group )
		{
			delete item;
		}
	}
}

/**
 * @brief Функция установки конечной точки пути
 * и старта поиска этого пути в отдельном потоке
 */
void SearchGraph::paintEndPoint()
{
	if(square_side == 0 || !last_start)
	{
		return;
	}

	if(auto rect = getRect(last_mouse_point); rect)
	{
		if(rect->pressed || rect == last_end)
		{
			return;
		}

		if(last_end)
		{
			deleteItemsFromGroup(group_lines);
			last_end->moved = false;
			last_end->update();
		}
		last_end = rect;
		end = *new QPoint(last_end->getX() + (last_end->getW() / 2),
		                 last_end->getY() + (last_end->getH() / 2));
		last_end->mouseMoveEvent(new QGraphicsSceneMouseEvent());
		if(field_height * field_width > 1)
		{
			if(!bfs)
			{
				bfs = new BFS(graph, start, end, square_side);

				// при получении сигнала удаляем QObject
				connect(bfs, &BFS::finished, bfs, &QObject::deleteLater);
				// соединение выполнения работы алгоритма и отрисовки этого пути
				connect(bfs, &BFS::sendPath, this, &SearchGraph::getPath, Qt::QueuedConnection);

				if(!thread)
				{
					thread = new QThread();
					// Основная функция работы потока
					QObject::connect(thread, &QThread::started, bfs, &BFS::run);
					// соединение удаления объекта потока при его завершении
					QObject::connect(thread, &QThread::finished, thread, &QObject::deleteLater);
				}
				bfs->moveToThread(thread); // передаём объект в другой поток
				thread->start();
			}
			else
			{
				bfs->setPointData(start, end); // устанавливаем новую конечную точку
			}
		}
	}
}

/**
 * @brief Функция поиска RectItem по координате
 * @param coord - координаты клетки поля
 * @return указатель на клетку поля
 */
RectItem* SearchGraph::getRect(QPoint coord)
{
	if(coord.x() <= 0 || coord.x() > 10000)
	{
		return nullptr;
	}

	for(auto i = 0u; i < graph.size(); i++)
	{
		auto curr = graph.at(i);
		auto radius = square_side * 0.5;
		if(abs(curr.x() - coord.x()) < radius &&
		        abs(curr.y() - coord.y()) < radius)
		{
			for(auto item: scene->items())
			{
				if(item->group() == group)
				{
					auto rect = qgraphicsitem_cast<RectItem*>(item);

					auto x_r = rect->getX() + square_side / 2;
					auto y_r = rect->getY() + square_side / 2;

					if(x_r == curr.x() && y_r == curr.y())
					{
						return rect;
					}
				}
			}
		}
	}
	return nullptr;
}

void SearchGraph::resizeEvent(QResizeEvent *event)
{
	fiels_resize_timer.start();
	QGraphicsView::resizeEvent(event);
}

/**
 * @brief Листенер нажатия мыши. Устанавливает значение координаты начала пути
 * @param event
 */
void SearchGraph::mousePressEvent(QMouseEvent *event)
{
	if(scaled) return;
	if(square_side == 0) return;

	auto rect = getRect(event->pos());
	if(!rect) return;

	if(last_start)
	{
		last_start->mousePressEvent(new QGraphicsSceneMouseEvent());
		last_start = nullptr;
	}

	deleteItemsFromGroup(group_lines);
	last_start = rect;
	start = *new QPoint(last_start->getX() + (last_start->getW() / 2),
	                   last_start->getY() + (last_start->getH() / 2));
	last_start->mousePressEvent(new QGraphicsSceneMouseEvent());
	QGraphicsView::mousePressEvent(event);
}

void SearchGraph::mouseMoveEvent(QMouseEvent *event)
{
	if(scaled) return;
	last_mouse_point = event->pos();
	mouse_move_timer.start();
	QGraphicsView::mouseMoveEvent(event);
}

void SearchGraph::wheelEvent(QWheelEvent *event)
{
	auto delta = 1.0f + event->angleDelta().y() / 1200.0f;
	scaled = true;
	scale(delta, delta);
}

/**
 * @brief Функция прорисовки пути
 * @param path - вектор с координатами центров клеток поля
 */
void SearchGraph::getPath(std::vector<QPoint> path)
{
	// Очищаем ранее нарисованный путь
	deleteItemsFromGroup(group_lines);

	// Если пути нет, то в переменной path будет одна точка
	if(path.size() < 2)
	{
		showMessage("Нет пути", 1000);
		return;
	}

	// В соответствии с количеством ребёр уменьшаем или увеличиваем размер линии
	if(path.size() > 20)
	{
		lines_pen.setWidth(2);
	}
	else
	{
		lines_pen.setWidth(4);
	}

	for(auto i = 0u; i < path.size() - 1; i++)
	{
		group_lines->addToGroup(scene->addLine(path[i].x(), path[i].y(), path[i + 1].x(), path[i + 1].y(), lines_pen));
	}
}


