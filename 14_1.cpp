/* 14_1. Минимальное остовное дерево

Дан неориентированный связный граф.
Требуется найти вес минимального остовного дерева в этом графе с помощью алгоритма Прима.

  Формат ввода
Первая строка содержит два натуральных числа n и m — количество вершин и ребер графа соответственно (1 ≤ n ≤ 20000, 0 ≤ m ≤ 100000).
Следующие m строк содержат описание ребер по одному на строке.
Ребро номер i описывается тремя натуральными числами bi, ei и wi — номера концов ребра и его вес соответственно (1 ≤ bi, ei ≤ n, 0 ≤ wi ≤ 100000).

  Формат вывода
Выведите единственное целое число - вес минимального остовного дерева.
*/

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <vector>

// Куча из задачи 2. С добавленным методом replace (вместо DecreaseKey),
// позволяющим менять элемент кучи на любой другой с ключом либо равным заменяемому, либо не встречающимся в куче
// Ключи в куче уникальны (в соответствии с operator==) (для Compare ключи могут быть одинаковы)
template<typename T, typename Compare = std::less<T> >
class Heap {
public:
  Heap(const std::vector<T>& arr, Compare cmp = Compare()): arr(arr), comp(cmp) {
    BuildHeap(this->arr);
  }

  T top() const {
    if (empty()) throw std::out_of_range("Heap is empty!");
    return arr[0];
  }

  T pop() {
    if (empty()) throw std::out_of_range("Heap is empty!");
    T t = arr[0];
    arr[0] = arr[arr.size() - 1];
    arr.pop_back();
    key_to_arr_index.erase(t);
    SiftDown(0);
    return t;
  }

  void push(T element) {
    assert(key_to_arr_index.count(element) == 0);
    arr.push_back(element);
    key_to_arr_index[element] = arr.size() - 1;
    SiftUp(arr.size() - 1);
  }

  void replace(const T& old, T new_) { // new_ и old могут иметь одинаковые ключи, иначе new_ должен быть новым
    assert(contains(old));
    int index = key_to_arr_index[old];
    key_to_arr_index.erase(old);
    assert(!contains(new_));
    key_to_arr_index[new_] = index;
    arr[index] = new_;
    if (comp(new_, old)) {
      SiftDown(index);
    } else {
      SiftUp(index);
    }
  }

  T get(const T& key) {
    return arr[key_to_arr_index[key]];
  }

  bool empty() const {
    return arr.empty();
  }

  bool contains(const T& key) const {
    return key_to_arr_index.count(key) > 0;
  }
private:
  std::vector<T> arr;
  Compare comp;
  std::unordered_map<T, int> key_to_arr_index;

  void BuildHeap(std::vector<T>& arr) {
    for (int i = 0; i < arr.size(); ++i) {
      key_to_arr_index[arr[i]] = i;
    }
    assert(arr.size() == key_to_arr_index.size()); // все ключи должны быть разными
    for (int i = arr.size() / 2 - 1; i >= 0; --i)
      SiftDown(i);
  }

  void SiftDown(int i) {
    int largest = i;
    if (2 * i + 1 < arr.size() && comp(arr[i], arr[2 * i + 1]))
      largest = 2 * i + 1;
    if (2 * i + 2 < arr.size() && comp(arr[largest], arr[2 * i + 2]))
      largest = 2 * i + 2;
    if (largest != i) {
      key_to_arr_index[arr[i]] = largest;
      key_to_arr_index[arr[largest]] = i;
      T t = arr[i];
      arr[i] = arr[largest];
      arr[largest] = t;
      SiftDown(largest);
    }
  }

  void SiftUp(int i) {
    if (i - 1 >= 0 && comp(arr[(i - 1) / 2], arr[i])) {
      key_to_arr_index[arr[i]] = (i - 1) / 2;
      key_to_arr_index[arr[(i - 1) / 2]] = i;
      T t = arr[i];
      arr[i] = arr[(i - 1) / 2];
      arr[(i - 1) / 2] = t;
      SiftUp((i - 1) / 2);
    }
  }
};


struct vertex {
  int value;
  int distance;
  vertex(int v, int d): value(v), distance(d) {}

  bool operator==(const vertex& other) const { // для хранения в map в Heap по ключу value
    return (value == other.value);
  }
};

namespace std {
  template <>
  struct hash<vertex> {
    size_t operator()(const vertex& v) const {
      return std::hash<int>()(v.value);
    }
  };
}


long long int MST_weight(std::vector<std::vector<std::pair<int, int>>>& graph) { // graph[u] = Vector<(v, w(u,v))>
  long long int tree_weight = 0;

  std::vector<vertex> V;
  V.reserve(graph.size());

  V.push_back(vertex(0, 0));

  int INF = 2147483647;
  for (int i = 1; i < graph.size(); ++i) {
    V.push_back(vertex(i, INF));
  }

  auto cmp = [](const vertex& a, const vertex& b)->bool{ return a.distance > b.distance; };
  Heap<vertex, decltype(cmp)> min_heap(V, cmp);

  int no_matter;

  while (!min_heap.empty()) {
    vertex u = min_heap.pop();
    tree_weight += u.distance;

    for (auto v_w : graph[u.value]) {
      if (min_heap.contains(vertex(v_w.first, no_matter)) ) {
        vertex v = min_heap.get(vertex(v_w.first, no_matter));
        if (v_w.second < v.distance) {
          v.distance = v_w.second;
          min_heap.replace(v, v);
        }
      }
    }
  }

  return tree_weight;
}


int main() {
  int n, m;
  std::cin >> n >> m;
  assert(n > 0 && m >= 0);
  assert(n < 20001 && m < 100001);

  std::vector<std::vector<std::pair<int, int>>> graph(n); // graph[u] = Vector<(v, w(u,v))>

  for (int i = 0; i < m; ++i) {
    int b, e, w;
    std::cin >> b >> e >> w;
    --b; --e;

    graph[b].push_back(std::make_pair(e, w));
    graph[e].push_back(std::make_pair(b, w));
  }

  std::cout << MST_weight(graph);

  return 0;
}
