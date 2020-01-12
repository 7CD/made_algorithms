/*18_2. Выпуклая оболочка 2D - с помощью алгоритма Джарвиса

Дано множество точек на плоскости (x, y). Постройте выпуклую оболочку этого множества и вычислите ее периметр.

Формат ввода
Количество точек, далее на каждой строке координаты точек x, y.
Общее количество точек не превосходит 100000.
Координаты точек - числа с плавающей точкой в диапазоне [-1000, 1000].

Формат вывода
Выведите периметр выпуклой оболочки.
*/

#include<iostream>
#include<cassert>
#include<vector>
#include<cmath>
#include<algorithm>


struct Point {
  double x;
  double y;
  bool operator==(const Point& other) const {
    return x == other.x && y == other.y;
  }
};


inline double cross_product(double x1, double y1, double x2, double y2) {
  return x1 * y2 - y1 * x2;
}

inline double distance(const Point& p1, const Point& p2) {
  return sqrt((p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y));
}


void JarvisConvexHull(std::vector<Point>& points, std::vector<Point>& convex_hull) {
  assert(convex_hull.empty());
  if (points.empty()) return;
  // в качестве стартовой точки выбираем самую левую из самых нижних
  const Point start_point = *(std::min_element(points.begin(), points.end(), [](const Point& a, const Point& b) -> bool {
                                               return a.y < b.y || (a.y == b.y && a.x < b.x); }));

  convex_hull.push_back(start_point);
  Point latest_hull_point = start_point; // последняя точка, добавленная в выпуклую оболочку

  do  {
    Point rightmost = latest_hull_point; // точка, такая что для ее радиус-вектора относительно latest_hull_point -> все точки расположены слева
    size_t rightmost_index = 0;

    for (size_t i = 0; i < points.size(); ++i) {
      // удаляем повторные вхождения latest_hull_point
      if (points[i] == latest_hull_point && !(points[i] == start_point)) {
        while (points[points.size() - 1] == latest_hull_point)
          points.pop_back();
        if (i < points.size()) {
          std::swap(points[i], points[points.size() - 1]);
          points.pop_back();
        }
        continue;
      }
      // векторное произведение радиус-вектора rightmost на радиус-вектор points[i]
      double cross_product_ = cross_product(rightmost.x - latest_hull_point.x, rightmost.y - latest_hull_point.y,
                                            points[i].x - latest_hull_point.x, points[i].y - latest_hull_point.y);

      if (cross_product_ < 0) { // поворот от радиус-вектора rightmost к points[i] по часовой стрелке
        rightmost = points[i];
        rightmost_index = i;
      }
      // либо rightmost и points[i] совпадают, либо rightmost равна latest_hull_point, либо rightmost, points[i] и last_hull_point лежат на одной прямой
      else if (cross_product_ == 0) {
        if (points[i] == rightmost)
          continue;
        else if (rightmost == latest_hull_point) {
          rightmost = points[i];
          rightmost_index = i;
        } else { // rightmost, points[i] и latest_hull_point не равны и лежат на одной прямой - выбираем ближайшую к latest_hull_point
          // start_point не удаляется и может оказаться ближайшей к latest_hull_point, тогда как есть другие, еще не добавленные канддидаты
          if (points[i] == start_point)
            continue;
          if (distance(points[i], latest_hull_point) < distance(rightmost, latest_hull_point) || rightmost == start_point) {
            rightmost = points[i];
            rightmost_index = i;
          }
        }
      }
    }
    convex_hull.push_back(rightmost);
    latest_hull_point = rightmost;
    std::swap(points[rightmost_index], points[points.size() - 1]);
    points.pop_back();

  } while (!(latest_hull_point == start_point));
}


double perimeter(const std::vector<Point>& points) {
  double perimeter = 0;
  for (int i = 0; i < static_cast<int>(points.size()) - 1; ++i) {
    perimeter += distance(points[i], points[i+1]);
  }
  if (points.size() > 1) {
    size_t i = points.size() - 1;
    perimeter += distance(points[0], points[i]);
  }
  return perimeter;
}


int main() {
  size_t N;
  std::cin >> N;
  assert(N < 100001);

  std::vector<Point> points(N);

  for (size_t i = 0; i < N; ++i) {
    std::cin >> points[i].x >> points[i].y;
  }

  std::vector<Point> convex_hull;
  JarvisConvexHull(points, convex_hull);

  std::cout.precision(16);
  std::cout << perimeter(convex_hull);

  return 0;
}
