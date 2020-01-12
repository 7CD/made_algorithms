/* 19. Поиск точек в прямоугольнике

На вход подаются точки и прямоугольники. Точка задается двумя координатами (x, y).
Прямоугольник - четверкой чисел [left, bottom, right, top].
Точка (left, bottom) принадлежит прямоугольнику, точка (right, top) - нет.
(left < right, bottom < top) Для каждого прямоугольника нужно вывести,
сколько добавленных точек он содержит.

X в диапазоне [-180, 180), Y - [-90, 90)
Количество точек <= 100000, Количество прямоугольников <= 1000

Формат входного файла
Количество точек N. Далее N строк с координатами точек (x пробел y). Далее
количество запросов M. Прямоугольники в виде 4 координат.

Формат выходного файла
Количество точек в каждом прямоугольнике
*/

#include<cassert>
#include<iostream>
#include<unordered_map>
#include<vector>

struct Point {
  double x;
  double y;
};


struct Rectangle {
  double left;
  double bottom;
  double right;
  double top;
};

// Для каждой точки весь range (заранее определенное возможное положение точек - прямоугольник на плоскости)
// разбивается рекурсивно до глубины depth на равные четверти (как в декартовой системе координат с началом в центре range).
// Каждая получившаяся клетка представляется строкой - последовательностью номеров четвертей, в которые попадает точка
// при рекурсивном спуске.
// В unordered_map<string, vector<Point>> для каждой клетки максимальной глубины хранится список попавших в нее точек.
// В unordered_map<string, size_t> для каждой составной клетки хранится число попавших в нее точек.
//
// Чтобы найти сколько точек содержится в заданном прямоугольнике, так же рекурсивно смотрим какие клетки накрывает прямоугольник.
// Для пустых клеток и клеток, целиком входящих в прямоугольник поиска, даем ответ за O(1).
// Для составных клеток, пересекаемых границей прямоугольника, спускаемся на уровень глубже.
// Для клеток последнего уровня, пересекаемых границей прямоугольника, проходим по списку лежащих в ней точек и считаем
// число попадающих в прямоугольник поиска. Такие клетки - только по периметру прямоугольника поиска.
class PointsQuadrupleDivisionHasher {
 public:
  PointsQuadrupleDivisionHasher(Rectangle range, size_t depth): depth(depth), RANGE(range) {
    assert(0 < depth && depth < 1000);
    assert(RANGE.left < RANGE.right && RANGE.bottom < RANGE.top);
  }

  void add_points(const std::vector<Point>& points) {
    for (const Point& point : points) {
      if (point.x < RANGE.left || point.x >= RANGE.right || point.y < RANGE.bottom || point.y >= RANGE.top)
        continue;
      Rectangle cell = RANGE;
      std::string cell_hash;

      for (int i = 0; i < depth; ++i) {
        char quarter_ = quarter(cell, point);
        cell_hash.push_back(quarter_);
        if (quarter_ == '1') {
          cell.bottom = 0.5 * (cell.bottom + cell.top);
          cell.left = 0.5 * (cell.left + cell.right);
        } else if (quarter_ == '2') {
          cell.bottom = 0.5 * (cell.bottom + cell.top);
          cell.right = 0.5 * (cell.left + cell.right);
        } else if (quarter_ == '3') {
          cell.top = 0.5 * (cell.bottom + cell.top);
          cell.right = 0.5 * (cell.left + cell.right);
        } else {
          cell.top = 0.5 * (cell.bottom + cell.top);
          cell.left = 0.5 * (cell.left + cell.right);
        }
        if (i < depth - 1) ++compound_cells[cell_hash];
        else last_level_cells[cell_hash].push_back(point);
      }
    }
  }

  size_t count_points(const Rectangle& search_rectangle) const {
    assert(search_rectangle.left < search_rectangle.right && search_rectangle.bottom < search_rectangle.top);
    return count_points(RANGE, std::string(), search_rectangle);
  }

 private:
  size_t count_points(const Rectangle& cell, const std::string& cell_hash, const Rectangle& search_rectangle) const {
    // если клетка последнего уровня пуста
    if (cell_hash.size() == depth && last_level_cells.count(cell_hash) == 0) return 0;
    // составная клетка пуста (но только не на нулевой итерации)
    if (0 < cell_hash.size() && cell_hash.size() < depth && compound_cells.count(cell_hash) == 0) return 0;
    // прямоугольник поиска целиком покрывает клетку
    if (search_rectangle.left <= cell.left && search_rectangle.bottom <= cell.bottom &&
        search_rectangle.right >= cell.right && search_rectangle.top >= cell.top) {
      if (cell_hash.size() == depth)
        return last_level_cells.at(cell_hash).size();
      return compound_cells.at(cell_hash);
    }
    // клетка последнего уровня пересекается с прямоугольником поиска частично - считаем попадающие точки "вручную"
    if (cell_hash.size() == depth) {
      size_t dots_num = 0;
      for (const Point& point : last_level_cells.at(cell_hash)) {
        if (point.x >= search_rectangle.left && point.y >= search_rectangle.bottom &&
            point.x < search_rectangle.right && point.y < search_rectangle.top)
          ++dots_num;
      }
      return dots_num;
    }

    // прямоугольник поиска пересекает составные клетки - вызываем процедуру рекурсивно на более мелких клетках

    char left_bottom_quarter = quarter(cell, Point{search_rectangle.left, search_rectangle.bottom});
    char right_top_quarter = quarter(cell, Point{search_rectangle.right, search_rectangle.top});
    // Части прямоугольника поиска в 1-4 четвертях
    Rectangle cell_1{0.5 * (cell.left + cell.right), 0.5 * (cell.bottom + cell.top), cell.right, cell.top};
    Rectangle cell_2{cell.left, 0.5 * (cell.bottom + cell.top), 0.5 * (cell.left + cell.right), cell.top};
    Rectangle cell_3{cell.left, cell.bottom, 0.5 * (cell.left + cell.right), 0.5 * (cell.bottom + cell.top)};
    Rectangle cell_4{0.5 * (cell.left + cell.right), cell.bottom, cell.right, 0.5 * (cell.bottom + cell.top)};

    if (left_bottom_quarter == '3') {
      if (right_top_quarter == '3') { // нижняя левая точка прямоугольника поиска в 3 четверти, верхняя правая - тоже
        return count_points(cell_3, cell_hash + '3', search_rectangle);
      }
      else if (right_top_quarter == '2') { // нижняя левая точка прямоугольника поиска в 3 четверти, верхняя правая - во 2
        Rectangle search_rectangle_3{search_rectangle.left, search_rectangle.bottom, search_rectangle.right, 0.5 * (cell.bottom + cell.top)};
        Rectangle search_rectangle_2{search_rectangle.left, 0.5 * (cell.bottom + cell.top), search_rectangle.right, search_rectangle.top};
        return count_points(cell_3, cell_hash + '3', search_rectangle_3) + count_points(cell_2, cell_hash + '2', search_rectangle_2);
      }
      else if (right_top_quarter == '4') { // нижняя левая точка прямоугольника поиска в 3 четверти, верхняя правая - в 4
        Rectangle search_rectangle_3{search_rectangle.left, search_rectangle.bottom, 0.5 * (cell.left + cell.right), search_rectangle.top};
        Rectangle search_rectangle_4{0.5 * (cell.left + cell.right), search_rectangle.bottom, search_rectangle.right, search_rectangle.top};
        return count_points(cell_3, cell_hash + '3', search_rectangle_3) + count_points(cell_4, cell_hash + '4', search_rectangle_4);
      }
      else if (right_top_quarter == '1') { // нижняя левая точка прямоугольника поиска в 3 четверти, верхняя правая - в 1
        Rectangle search_rectangle_3{search_rectangle.left, search_rectangle.bottom, 0.5 * (cell.left + cell.right), 0.5 * (cell.bottom + cell.top)};
        Rectangle search_rectangle_4{0.5 * (cell.left + cell.right), search_rectangle.bottom, search_rectangle.right, 0.5 * (cell.bottom + cell.top)};
        Rectangle search_rectangle_2{search_rectangle.left, 0.5 * (cell.bottom + cell.top), 0.5 * (cell.left + cell.right), search_rectangle.top};
        Rectangle search_rectangle_1{0.5 * (cell.left + cell.right), 0.5 * (cell.bottom + cell.top), search_rectangle.right, search_rectangle.top};
        return count_points(cell_3, cell_hash + '3', search_rectangle_3) + count_points(cell_4, cell_hash + '4', search_rectangle_4) +
               count_points(cell_2, cell_hash + '2', search_rectangle_2) + count_points(cell_1, cell_hash + '1', search_rectangle_1);
      }
    }
    else if (left_bottom_quarter == '2') {
      if (right_top_quarter == '2') { // нижняя левая точка прямоугольника поиска во 2 четверти, верхняя правая - тоже
        return count_points(cell_2, cell_hash + '2', search_rectangle);
      }
      else if (right_top_quarter == '1') { // нижняя левая точка прямоугольника поиска во 2 четверти, верхняя правая - в 1
        Rectangle search_rectangle_2{search_rectangle.left, search_rectangle.bottom, 0.5 * (cell.left + cell.right), search_rectangle.top};
        Rectangle search_rectangle_1{0.5 * (cell.left + cell.right), search_rectangle.bottom, search_rectangle.right, search_rectangle.top};
        return count_points(cell_2, cell_hash + '2', search_rectangle_2) + count_points(cell_1, cell_hash + '1', search_rectangle_1);
      }
    }
    else if (left_bottom_quarter == '1') {
      if (right_top_quarter == '1') { // нижняя левая точка прямоугольника поиска в 1 четверти, верхняя правая - тоже
        return count_points(cell_1, cell_hash + '1', search_rectangle);
      }
    }
    else if (left_bottom_quarter == '4') {
      if (right_top_quarter == '4') { // нижняя левая точка прямоугольника поиска в 4 четверти, верхняя правая - тоже
        return count_points(cell_4, cell_hash + '4', search_rectangle);
      }
      else if (right_top_quarter == '1') { // нижняя левая точка прямоугольника поиска в 4 четверти, верхняя правая - в 1
        Rectangle search_rectangle_4{search_rectangle.left, search_rectangle.bottom, search_rectangle.right, 0.5 * (cell.bottom + cell.top)};
        Rectangle search_rectangle_1{search_rectangle.left, 0.5 * (cell.bottom + cell.top), search_rectangle.right, search_rectangle.top};
        return count_points(cell_4, cell_hash + '4', search_rectangle_4) + count_points(cell_1, cell_hash + '1', search_rectangle_1);
      }
    }
  }

  const Rectangle RANGE; // область возможного расположения точек
  const size_t depth; // "глубина" разбиения RANGE на 4 части
  std::unordered_map<std::string, std::vector<Point>> last_level_cells; // клетки последнего уровня, в которых есть точки, -> точки
  std::unordered_map<std::string, size_t> compound_cells; // составные клетки, в которых есть точки, -> число лежащих в них точек

  // в какую четверть cell (как в декартовой системе координат с началом в центре cell) попадает point
  inline char quarter(const Rectangle& cell, const Point& point) const {
    if (point.x < 0.5 * (cell.left + cell.right)) {
      if (point.y < 0.5 * (cell.bottom + cell.top)) return '3';
      else return '2';
    } else {
      if (point.y < 0.5 * (cell.bottom + cell.top)) return '4';
      else return '1';
    }
  }
};


int main() {
  size_t N;
  std::cin >> N;
  assert(N < 100001);

  PointsQuadrupleDivisionHasher pointsHasher(Rectangle{-180, -90, 180, 90}, 8);

  std::vector<Point> points;

  for (size_t i = 0; i < N; ++i) {
    Point point;
    std::cin >> point.x >> point.y;
    points.push_back(point);
  }

  pointsHasher.add_points(points);

  size_t M;
  std::cin >> M;
  assert(M < 1001);

  for (size_t i = 0; i < M; ++i) {
    Rectangle search_rectangle;
    std::cin >> search_rectangle.left >> search_rectangle.bottom >> search_rectangle.right >> search_rectangle.top;
    std::cout << pointsHasher.count_points(search_rectangle) << std::endl;
  }

  return 0;
}
