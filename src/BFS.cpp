/**
*  @file BFS.h
*
*  Class for breadth first search implementation
*
*  Author : Vladimir O. Ovchinnikov
**/

#include "QThread"

#include "BFS.h"

#include <queue>
#include <math.h>

BFS::BFS(std::vector<QPoint> graph, QPoint start, QPoint end, int side_size)
    : point_graph(graph),
      point_start(start),
      point_end(end),
      mutex(),
      side_size(side_size)
{

}


/**
 * @brief Используется созданным потоком: преобразует координаты в узлы графа
 */
void BFS::pointToNode()
{
	mutex.lock();

	for(auto i = 0u; i < point_graph.size(); i++)
	{
		auto node = new Node(point_graph[i]);
		this->node_graph.push_back(node);
		if(point_graph[i] == point_start)
		{
			this->node_start = node;
		}
		if(point_graph[i] == point_end)
		{
			this->node_end = node;
		}
	}
}

/**
 * @brief Используется потоком GUI: устанавливает необходимые данные для поиска пути
 * @param graph - вектор доступных точек
 * @param start - начальная точка
 * @param end - конечная точка
 */
void BFS::setPointData(QPoint start, QPoint end)
{
	mutex.lock();

	change_pos = true;
	point_start = start;
	point_end = end;

	mutex.unlock();
}

/**
 * @brief Функция поиска соседей вершины
 * @param node - вершина графа
 * @return вектор указателей Node
 */
std::vector<Node*> BFS::getNeighborNodes(Node *node)
{
	std::vector<Node*> neighbors;

	for(auto i = 0u; i < node_graph.size(); i++)
	{
		auto n = node_graph[i];
		if(n->coords == node->coords || n->visited) continue;
		auto delta_x = std::abs(n->coords.x() - node->coords.x());
		auto delta_y = std::abs(n->coords.y() - node->coords.y());

		if((delta_x == side_size && delta_y == 0) || (delta_x == 0 && delta_y == side_size))
		{
			neighbors.push_back(n);
		}
	}

	return neighbors;
}

/**
 * @brief Основная функция выполнения поиска в ширину
 */
void BFS::run()
{
	while(true)
	{
		if(QThread::currentThread()->isInterruptionRequested())
		{
			emit finished();
			return;
		}
		if(change_pos)
		{
			std::vector<QPoint> path;

			auto compNodes = [](Node* first, Node* second) { return first->weight > second->weight; };
			std::priority_queue<Node*, std::vector<Node*>, decltype (compNodes)> pri_nodes(compNodes);

			pointToNode(); // points_graph в node_graph

			auto goal = node_end;

			if(!node_start) return;
			node_start->weight = 0;
			pri_nodes.push(node_start);

			// Поиск в ширину
			while(!pri_nodes.empty() && !goal->visited)
			{
				auto curr = pri_nodes.top();
				pri_nodes.pop();

				if(curr->visited)
				{
					continue;
				}

				curr->visited = true;
				if(goal->visited)
				{
					break;
				}

				auto neighbores = getNeighborNodes(curr);
				for(auto n: neighbores)
				{
					auto weight = curr->weight + sqrt(pow(curr->coords.x() - n->coords.x(), 2) +
					                                 pow(curr->coords.y() - n->coords.y(), 2));
					if(weight < n->weight)
					{
						n->weight = weight;
						n->prev_node = curr;

						pri_nodes.push(n);
					}
				}

			}

			// Формирование вектора пути по графу
			path.push_back(goal->coords);

			Node* prev = goal->prev_node;

			while(prev != nullptr)
			{
				path.push_back(prev->coords);
				if(prev == node_start)
					break;
				prev = prev->prev_node;
			}

			std::reverse(path.begin(), path.end());

			emit sendPath(path); // сигнал отрисовки пути

			mutex.unlock();

			change_pos = false;
		}
	}
}
