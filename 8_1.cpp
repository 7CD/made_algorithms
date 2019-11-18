/*8_1. Хеш-таблица

Реализуйте структуру данных типа “множество строк” на основе динамической хеш-таблицы с открытой адресацией.
Хранимые строки непустые и состоят из строчных латинских букв.
Хеш-функция строки должна быть реализована с помощью вычисления значения многочлена методом Горнера.
Начальный размер таблицы должен быть равным 8-ми.
Перехеширование выполняйте при добавлении элементов в случае, когда коэффициент заполнения таблицы достигает 3/4.
Структура данных должна поддерживать операции добавления строки в множество, удаления строки из множества и проверки
принадлежности данной строки множеству.

1_1. Для разрешения коллизий используйте квадратичное пробирование. i-ая проба
g(k, i)=g(k, i-1) + i (mod m). m - степень двойки.

Формат входных данных
Каждая строка входных данных задает одну операцию над множеством.
Запись операции состоит из типа операции и следующей за ним через пробел строки, над которой проводится операция.
Тип операции  – один из трех символов:
    +  означает добавление данной строки в множество;
    -  означает удаление  строки из множества;
    ?  означает проверку принадлежности данной строки множеству.
При добавлении элемента в множество НЕ ГАРАНТИРУЕТСЯ, что он отсутствует в этом множестве.
При удалении элемента из множества НЕ ГАРАНТИРУЕТСЯ, что он присутствует в этом множестве.
Формат выходных данных
Программа должна вывести для каждой операции одну из двух строк OK или FAIL, в зависимости от того,
встречается ли данное слово в нашем множестве.
*/
#include <assert.h>
#include <iostream>
#include <string>
#include <vector>

class HashTable {
 public:
  bool Has(const std::string& key) const;
  bool Add(const std::string& key);
  bool Remove(const std::string& key);
 private:
  size_t size = 0;
  size_t capacity = 8;
  size_t maxSize = capacity / 4 * 3;
  size_t a = 13; // константа для вычисления полиномиального кэша
  std::vector<std::string> table =  std::vector<std::string>(capacity);
  const std::string tombstone = "_DELETED_"; // метка удаленного элемена
  size_t hash(const std::string& key) const;
  inline bool Has(const std::string& key, const size_t hash) const;
  inline size_t probe(const size_t hash, const size_t i) const;
  void grow();
};

bool HashTable::Has(const std::string& key) const {
  assert(!key.empty());
  size_t hash = this->hash(key);
  return Has(key, hash);
}

inline bool HashTable::Has(const std::string& key, size_t hash) const {
  for (size_t i = 0; i < capacity; ++i) {
    hash = probe(hash, i);
    if (table[hash].empty()) return false;
    if (table[hash] == key) return true;
  }
}

bool HashTable::Add(const std::string& key) {
  assert(!key.empty());
  size_t hash = this->hash(key);
  if (Has(key, hash)) return false;
  size_t x = 0;
  for (size_t i = 0; i < capacity; ++i) {
    hash = probe(hash, i);
    if (table[hash].empty() || table[hash] == tombstone) {
      table[hash] = key;
      ++size;
      if (size > maxSize) grow();
      return true;
    }
    if (table[hash] == key) return false;
  }
}

void HashTable::grow() {
  std::vector<std::string> oldTable = std::move(table);
  capacity *= 2;
  maxSize = capacity / 4 * 3;
  table = std::vector<std::string>(capacity);
  for (auto e : oldTable) {
    if (!e.empty() && e != tombstone) {
      size_t hash = this->hash(e);
      for (size_t i = 0; !table[hash].empty() && i < capacity; ++i) {
        hash = probe(hash, i);
      }
      table[hash] = e;
    }
  }
}

bool HashTable::Remove(const std::string& key) {
  assert(!key.empty());
  size_t hash = this->hash(key);
  for (size_t i = 0; i < capacity; ++i) {
    hash = probe(hash, i);
    if (table[hash].empty()) return false;
    if (table[hash] == key) {
        table[hash] = tombstone;
        --size;
        return true;
    }
  }
}

// Полиномиальный хэш строки по схеме Горнера
size_t HashTable::hash(const std::string& key) const {
  size_t hash = 0;
  for(const char& c : key) {
    hash = (hash * a + c) % capacity;
  }
  return hash;
}

// Квадратичное пробирование
inline size_t HashTable::probe(size_t hashPrev, size_t i) const {
  return (hashPrev + i) % capacity;
}


int main() {
  HashTable table;
  char command = ' ';
  std::string value;
  while (std::cin >> command >> value) {
    switch (command) {
      case '?':
        std::cout << (table.Has(value) ? "OK" : "FAIL") << std::endl;
        break;
      case '+':
        std::cout << (table.Add(value) ? "OK" : "FAIL") << std::endl;
        break;
      case '-':
        std::cout << (table.Remove(value) ? "OK" : "FAIL") << std::endl;
        break;
    }
  }
  return 0;
}
