/* 11_1. Цикл минимальной длины

Дан невзвешенный неориентированный граф. Найдите цикл минимальной длины.

Формат ввода
В первой строке вводятся два натуральных числа N и M, не превосходящих 10000.
Далее идут M строк по 2 числа (от 0 до N-1) - индексы вершин между которыми есть ребро.

Формат вывода
Требуется вывести одно целое число равное длине минимального цикла.
Если цикла нет, то вывести -1.
*/

#include<iostream>
#include<cassert>
#include<vector>
#include<queue>

// Цикл детектируется, когда в bfs из вершины v происходит обращение к серой вершине u,
// которая находится не ближе от "начальной вершины", чем сама v. Длина цикла = distance[u] + distance[v] + 1.
// При этом distance[u] и distance[v] либо равны, либо distance[u] = distance[v] + 1
// Цикл длины L может быть найден при старте из вершины цикла на этапе bfs глубины L / 2,
// т.е. когда из очереди достаются вершины с distance = L / 2 - 1, и отмечаются вершины с L / 2.
// Поэтому если уже найден цикл длины L, то bfs можно прекратить по достижении глубины L / 2.
// Для этого служит параметр max_depth.
int bfs_cycle(size_t start_vertex, const std::vector<std::vector<size_t>>& graph,
              std::vector<bool>& visited, int max_depth) {
  const size_t N = graph.size();
  assert(start_vertex < N);
  assert(max_depth > 0);

  std::vector<size_t> distance(N, N); // N ~ inf
  std::queue<size_t> queue_;
  queue_.push(start_vertex);
  visited[start_vertex] = true;
  distance[start_vertex] = 0;

  while (!queue_.empty()) {
    size_t v = queue_.front(); queue_.pop();
    if (distance[v] >=  max_depth) return -1; // цикл не найден в max_depth окрестности start_vertex
    for (size_t u : graph[v]) {
      if (!visited[u]) {
        visited[u] = true;
        distance[u] = distance[v] + 1;
        queue_.push(u);
      } else if (!(distance[u] < distance[v])) { // Обращение из v к серой вершине u, которая не ближе
        return distance[u] + distance[v] + 1; // сразу возвращаем, т.к. циклов меньше уже не найдем
      }
    }
  }

  return -1; // цикл не найден
}


int min_cycle(const std::vector<std::vector<size_t>>& graph) {
  const size_t N = graph.size();
  // Оптимизация. Проверяем есть ли хоть один цикл, чтобы не запускать зря bfs от каждой вершины.
  int max_cycle_size = -1;
  std::vector<bool> visited(N, false);
  for (size_t i = 0; i < N; ++i) { // На случай если граф несвязный
      if (!visited[i]) {
        int cycle_size = bfs_cycle(i, graph, visited, N);
        if (cycle_size > max_cycle_size) { // цикл найден
          max_cycle_size = cycle_size;
          break;
        }
      }
  }
  if (max_cycle_size < 0) return -1;

  int min_cycle_size = N + 1;
  for (size_t i = 0; i < N; ++i) {
    std::vector<bool> visited(N, false);
    int cycle_size = bfs_cycle(i, graph, visited, min_cycle_size / 2);
    if (cycle_size == 3) return 3;
    if (cycle_size > 0 && cycle_size < min_cycle_size) min_cycle_size = cycle_size;
  }

  return min_cycle_size;
}

/*
  Сложность в худшем случае (цикл длины V): O((V + E)V)
*/
int main() {
  size_t N, M;
  std::cin >> N >> M;
  //assert(N < 10001 && M < 10001); // Из-за этого assert'а падал 32 тест

  std::vector<std::vector<size_t>> graph(N);

  for (size_t i = 0; i < M; ++i) {
    size_t v1, v2;
    std::cin >> v1 >> v2;
    assert(v1 < N && v2 < N);
    graph[v1].push_back(v2);
    graph[v2].push_back(v1);
  }

  std::cout << min_cycle(graph);

  return 0;
}
