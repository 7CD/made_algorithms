/* 12. Мосты

Ребро неориентированного графа называется мостом, если удаление этого ребра из графа увеличивает число компонент связности.
Дан неориентированный граф, требуется найти в нем все мосты.

Формат ввода
Ввод	bridges.in. Первая строка входного файла содержит два целых числа n и m —
количество вершин и ребер графа соответственно (1 ≤ n ≤ 20000, 1 ≤ m ≤ 200000).
Следующие m строк содержат описание ребер по одному на строке.
Ребро номер i описывается двумя натуральными числами bi, ei — номерами концов ребра (1 ≤ bi, ei ≤ n).

Формат вывода
Вывод	bridges.out. Первая строка выходного файла должна содержать одно натуральное число b — количество мостов в заданном графе.
На следующей строке выведите b целых чисел — номера ребер, которые являются мостами, в возрастающем порядке.
Ребра нумеруются с единицы в том порядке, в котором они заданы во входном файл
*/

#include <algorithm>
#include<cassert>
#include <fstream>
#include <iostream>
#include<stack>
#include<vector>
#include<unordered_map>
#include<unordered_set>

struct edge {
  int v1;
  int v2;
  edge(int u1, int u2) {
    v1 = std::min(u1, u2);
    v2 = std::max(u1, u2);
  }
  bool operator==(const edge& other) const {
    return ((v1 == other.v1) && (v2 == other.v2));
  }
};

namespace std {
  template <>
  struct hash<edge> {
    size_t operator()(const edge& key) const {
      return std::hash<int>()(key.v1) + std::hash<int>()(key.v2);
    }
  };
}


void dfs_bridge_recursive(const std::vector<std::vector<int>>& graph, int v, int parent_v,
                          std::vector<edge>& bridges, std::vector<bool>& visited,
                          std::vector<int>& entry, std::vector<int>& lowest, int& time) {
  visited[v] = true;
  entry[v] = lowest[v] = time++;
  for (int to : graph[v]) {
    if (to == parent_v)
      continue;
    if (visited[to])
      lowest[v] = std::min(lowest[v], entry[to]);
    else {
      dfs_bridge_recursive(graph, to, v, bridges, visited, entry, lowest, time);
      lowest[v] = std::min(lowest[v], lowest[to]);
      if (lowest[to] > entry[v])
        bridges.push_back(edge(v, to));
    }
  }
}


void dfs_bridge(const std::vector<std::vector<int>>& graph, int v, int v_parent,
                std::vector<edge>& bridges, std::vector<bool>& visited,
                std::vector<int>& entry, std::vector<int>& lowest) {
  int time = 0;
  std::stack<int> v_stack;
  std::stack<int> v_parent_stack;
  std::stack<int> v_neighbour_index_stack;

  v_stack.push(v);
  v_parent_stack.push(v_parent);
  v_neighbour_index_stack.push(0);

  while (!v_stack.empty()) {
    int v = v_stack.top();
    int v_parent = v_parent_stack.top();
    int i = v_neighbour_index_stack.top();

    if (!visited[v]) {
      visited[v] = true;
      entry[v] = lowest[v] = time++;
    }

    if (i < graph[v].size()) {
      int to = graph[v][i];
      v_neighbour_index_stack.pop();
      v_neighbour_index_stack.push(i+1);

      if (to == v_parent)
        continue;
      if (visited[to])
        lowest[v] = std::min(lowest[v], entry[to]);
      else { // dfs(to, v);
        v_stack.push(to);
        v_parent_stack.push(v);
        v_neighbour_index_stack.push(0);
      }
    } else {
      if (v != 0) lowest[v_parent] = std::min(lowest[v_parent], lowest[v]);
      if (lowest[v] > entry[v_parent])
        bridges.push_back(edge(v_parent, v));
      // отработали, поднимаемся по дереву рекурсии наверх
      v_stack.pop();
      v_parent_stack.pop();
      v_neighbour_index_stack.pop();
    }
  }
}


void find_bridges(const std::vector<std::vector<int>>& graph, std::vector<edge>& bridges) {
  int n = graph.size();
  std::vector<bool> visited(n, false);
  std::vector<int> entry(n, 0);
  std::vector<int> lowest(n, 0);
  for (int  i = 0; i < n; ++i) {
    if (!visited[i])
      dfs_bridge(graph, i, -1, bridges, visited, entry, lowest);
  }
}


int main() {
  std::ifstream input("bridges.in");
  int n, m;
  input >> n >> m;
  assert(n > 0 && m > 0);
  assert(n < 20001 && m < 200001);

  std::vector<std::vector<int>> graph(n);

  std::unordered_map<edge, int> edge_to_number; // номера ребер во входном файле
  std::unordered_set<edge> multiple_edges;

  for (int i = 1; i < m + 1; ++i) {
    size_t v1, v2;
    input >> v1 >> v2;
    --v1; --v2;
    assert(v1 < n && v2 < n);
    edge edge_ = edge(v1, v2);
    if(edge_to_number.count(edge_) > 0) {
    	multiple_edges.insert(edge_);
    	continue;
  	}
    edge_to_number[edge_] = i;
    graph[v1].push_back(v2);
    graph[v2].push_back(v1);
  }
  input.close();

  std::vector<edge> bridges;
  find_bridges(graph, bridges);

  std::vector<int> bridge_numbers;
  for (const edge& bridge : bridges) {
   	if(multiple_edges.count(bridge) == 0)
    	bridge_numbers.push_back(edge_to_number[bridge]);
  }
  std::sort(bridge_numbers.begin(), bridge_numbers.end());

  std::ofstream output("bridges.out");
  output << bridge_numbers.size() << std::endl;
  for (int number : bridge_numbers) {
    output << number << " ";
  }
  output.close();

  return 0;
}
