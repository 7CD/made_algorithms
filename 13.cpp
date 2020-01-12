/* A. Пятнашки неточные

Ограничение времени:	6 секунд
Ограничение памяти:	1Gb
Ввод:	стандартный ввод или input.txt
Вывод:	стандартный вывод или output.txt

Написать алгоритм для решения игры в “пятнашки”. Решением задачи является приведение к виду:
[ 1 2 3 4 ] [ 5 6 7 8 ] [ 9 10 11 12] [ 13 14 15 0 ], где 0 задает пустую ячейку.
Достаточно найти хотя бы какое-то решение. Число перемещений костяшек не обязано быть минимальным.


Формат ввода
Начальная расстановка. Пример: 1 2 3 0 5 6 7 4 9 10 11 8 13 14 15 12

Формат вывода
Если вам удалось найти решение, то в первой строке файла выведите число перемещений, которое требуется сделать в вашем решении.
А во второй строке выведите соответствующую последовательность ходов: L означает, что в результате перемещения костяшка сдвинулась влево,
R – вправо, U – вверх, D – вниз. Если же выигрышная конфигурация недостижима, то выведите в выходной файл одно число −1.
*/

#include <algorithm>
#include<cassert>
#include <fstream>
#include<string>
#include<unordered_map>
#include<vector>

struct position {
  std::vector<char> chips;
  char zero_place = 15; // индекс пустой ячейки
  short estimated_moves = 255; // расстояние от начальной позиции + эвристика
  // cумма манхэттенских расстояний каждой ячейки (кроме 0) до своего правильного положения, умноженная на 2
  int heuristic() {
    int h = 0;
    for (int i = 0; i < 16; ++i) {
      if (chips[i] != 0)
        h += std::abs(i / 4 - (chips[i] - 1) / 4) + std::abs(i % 4 - (chips[i] - 1) % 4);
    }
    return 2 * h;
  }
  bool IsFinish() const; // является ли данная расстановка конечной
  std::vector<position> Siblings() const; // расстановки, соседние с данной
  bool operator==(const position& other) const;
};

bool position::operator==(const position& other) const {
  for (size_t i = 0; i < chips.size(); ++i) {
    if (chips[i] != other.chips[i]) return false;
  }
  return true;
}

namespace std {
  template <>
  struct hash<position> {
    size_t operator()(const position& key) const {
      size_t value = 0;
      int HashParameter = 31;
      for (auto chip : key.chips) {
        value = HashParameter * value + static_cast<size_t>(chip);
      }
      return value;
    }
  };
}

const position FinishPosition{{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0}, 15};

bool position::IsFinish() const {
  return operator==(FinishPosition);
}

std::vector<position> position::Siblings() const {
  std::vector<position> result;
  if (zero_place < 12) { // Down
    position sibling = *this;
    std::swap(sibling.chips[zero_place], sibling.chips[zero_place + 4]);
    sibling.zero_place += 4;
    result.emplace_back(sibling);
  }
  if (zero_place > 3) { // Up
    position sibling = *this;
    std::swap(sibling.chips[zero_place], sibling.chips[zero_place - 4]);
    sibling.zero_place -= 4;
    result.emplace_back(sibling);
  }
  if (zero_place % 4 != 0) { // Left
    position sibling = *this;
    std::swap(sibling.chips[zero_place], sibling.chips[zero_place - 1]);
    sibling.zero_place -= 1;
    result.emplace_back(sibling);
  }
  if (zero_place % 4 != 3) { // Right
    position sibling = *this;
    std::swap(sibling.chips[zero_place], sibling.chips[zero_place + 1]);
    sibling.zero_place += 1;
    result.emplace_back(sibling);
  }
  return result;
}

// решение существует тогда и только тогда когда
// число инверсий в перестановке(не считая нулевой пустой элемент) + номер строки пустого элемента(считая от 1) - четное число
bool IsSolvable(const position& start) {
  int sum = start.zero_place / 4 + 1;
  for (int i = 0; i < 16; ++i) {
    if (i == start.zero_place) continue;
    for (int j = i + 1; j < 16; ++j)
      if (start.chips[j] < start.chips[i] && j != start.zero_place)
        ++sum;
  }
  if (sum % 2 == 0) return true;
  return false;
}

// Куча из задания 2. С добавленным методом replace (вместо DecreaseKey),
// позволяющим менять элемент кучи на любой другой с ключом либо равным заменяемому, либо не встречающимся в куче
// Ключи в куче уникальны
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


void A_star(position& start, std::unordered_map<position, position>& parents) {

  auto cmp = [](const position& a, const position& b)->bool{ return a.estimated_moves  > b.estimated_moves; };
  std::vector<position> t_v;
	Heap<position, decltype(cmp)> min_heap(t_v, cmp);
  // расстояния до start
	std::unordered_map<position, unsigned char> position_to_distance; // из любой разрешимой позиции оптимальное число ходов <= 80

  position_to_distance[start] = 0;
  start.estimated_moves = start.heuristic();
  min_heap.push(start);

  while (!min_heap.empty()) {
    position u = min_heap.pop();
    if (u.IsFinish()) return;
    for (auto sibling : u.Siblings()) {
      if (position_to_distance.count(sibling) == 0) { // Если позиция рассматривается впервые
        int d = position_to_distance[u] + 1;
        position_to_distance[sibling] = d;
        parents[sibling] = u;
        sibling.estimated_moves = d + sibling.heuristic();
        min_heap.push(sibling);
      } else if (position_to_distance[sibling] > position_to_distance[u] + 1) {
        position_to_distance[sibling] = position_to_distance[u] + 1;
        parents[sibling] = u;
        if (min_heap.contains(sibling)) {
          sibling.estimated_moves = position_to_distance[u] + 1 + sibling.heuristic();
          min_heap.replace(sibling, sibling);
        }
      }
    }
  }
}


char GetMoveSymbol(const position& from, const position& to) {
  char zero_diff = to.zero_place - from.zero_place;
  switch (zero_diff) {
    case 1:
      return 'L';
    case -1:
      return 'R';
    case 4:
      return 'U';
    case -4:
      return 'D';
    default:
      assert(false);
  }
  return 0;
}


std::vector<char> GetPath(const position& start, const std::unordered_map<position, position>& parents) {
  std::vector<char> result;

  position current = FinishPosition;
  while (!(current == start)) {
    const position parent = parents.at(current);
    result.push_back(GetMoveSymbol(parent, current));
    current = parent;
  }
  // Развернем result, так как собирали его с конца.
  std::reverse(result.begin(), result.end());
  return result;
}


int main() {
  position start;
  start.chips = std::vector<char>(16, 0);

  std::ifstream input("input.txt");
  for (int i = 0; i < 16; ++i) {
    std::string number;
    std::getline(input, number, ' ');
    start.chips[i] = std::stoi(number);
    if (start.chips[i] == 0) start.zero_place = i;
  }
  input.close();

  std::ofstream output("output.txt");
  if (!IsSolvable(start)) {
    output << -1;
  } else {
    std::unordered_map<position, position> parents;
    A_star(start, parents);
    auto solution = GetPath(start, parents);
    output << solution.size() << std::endl;
    for (auto command : solution)
      output << command << " ";
  }
  output.close();

  return 0;
}
