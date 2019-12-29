/* 16_2. Поиск подстроки

Найдите все вхождения шаблона в строку c помощью z-функции.
Длина шаблона – p, длина строки – n. Время O(n + p), доп. память – O(p).
p <= 30000, n <= 300000.

Формат ввода
Шаблон, символ перевода строки, строка.

Формат вывода
Позиции вхождения шаблона в строке.
*/

#include<iostream>
#include<string>
#include<vector>

// z[i] равен длине максимального префикса подстроки s[i:], являющегося префиксом самой s - т.е. длина отрезка совпадения
// время - O(|s|)
std::vector<size_t> z_function(const std::string& string_) {
  std::vector<size_t> z(string_.size(), 0);
  if (string_.empty()) return z;

  z[0] = string_.size(); // начнем процедуру с индекса 1
  size_t l = 1, r = 1; // начало и конец(не включительно) известного до текущего момента отрезка совпадения z[j] с cамым правым концом r

  for (size_t i = 1; i < string_.size(); ++i) {
    // отрезок совпадения с cамым правым концом вырожден,
    // "вручную" находим отрезок совпадения для s[i] и обновляем l и r
    if (i >= r) {
      for (size_t k = 0; i + k < string_.size() && string_[i + k] == string_[k]; ++k) {
        ++z[i];
      }
      l = i; r = i + z[i];
    }
    // при условии z[i - l] + i - l == r - l начало отрезка совпадения для s[i] гарантированно совпадает
    // с куском отрезка совпадения от i - l до r - l (не включительно)
    // и, возможно, его последующие символы совпадают с соответствующими следующими символами префикса s
    else if (z[i - l] + i == r) {
      z[i] = r - i;
      for (size_t k = 0; r + k < string_.size() && string_[r + k] == string_[r - i + k]; ++k) {
        ++z[i];
      }
      l = i; r = i + z[i];
    }
    // т.к. отрезок совпадения s[l:r] совпадает с префиксом s[0:r - l] и
    // при условии z[i - l] + i - l < r - l отрезок совпадения для s[i - l] целиком лежит в s[0:r - l],
    // то отрезок совпадения для s[i] совпадает с отрезком совпадения для s[i-l]
    else if (z[i - l] + i < r) {
      z[i] = z[i - l];
    }
    // при условии z[i - l] + i - l > r - l отрезок совпадения для s[i - l] выходит за пределы в s[0:r - l],
    // и отрезок совпадения для s[i] гарантировано равен s[i - l:r - l]
    else if (z[i - l] + i > r) {
      z[i] = r - i;
    }
  }
  return z;
}

// Аналогично z_function, только для text[i] ищем длину отрезка совпадения с pattern,
// и если она совпадает с |pattern|, то вхождение найдено.
// Гарантированно r - l <= |pattern| и значения z-функции для самого text не понадобятся.
// память - O(|pattern|), время - O(|pattern| + |text|)
void print_occurrence_positions(const std::string pattern, std::string text) {
  if (pattern.empty()) return;

  std::vector<size_t> z_pattern = z_function(pattern);

  size_t l = 0, r = 0;

  for (size_t i = 0; i < text.size(); ++i) {
    size_t z_i = 0; // длина отрезка совпадения для text[i]

    if (i >= r) {
      for (size_t k = 0; i + k < text.size() && text[i + k] == pattern[k]; ++k) {
        ++z_i;
      }
      l = i; r = i + z_i;
    }
    else if (z_pattern[i - l] + i == r) {
      z_i = r - i;
      for (size_t k = 0; r + k < text.size() && text[r + k] == pattern[r - i + k]; ++k) {
        ++z_i;
      }
      l = i; r = i + z_i;
    }
    else if (z_pattern[i - l] + i < r) {
      z_i = z_pattern[i - l];
    }
    else if (z_pattern[i - l] + i > r) {
      z_i = r - i;
    }

    if (z_i == pattern.size()) // text[z_i : z_i + |pattern|] == pattern
      std::cout << i << " ";
  }
}


int main() {
  std::string pattern;
  std::cin >> pattern;
  std::string text;
  std::cin >> text;

  print_occurrence_positions(pattern, text);

  return 0;
}
