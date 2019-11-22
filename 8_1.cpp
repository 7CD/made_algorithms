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

template<class T, class Hash = std::hash<T>, class KeyEqual = std::equal_to<T>>
class HashTable {
 public:
  bool Has(const T& key) const;
  bool Add(const T& key);
  bool Remove(const T& key);
  HashTable(): size(0), capacity(8), loadFactor(0.75), table(capacity, nullptr), tombstone(new TableNode("")) {}
  ~HashTable() {
    for (size_t i = 0; i < capacity; ++i) {
      if (table[i] != nullptr && table[i] != tombstone)
        delete table[i] ;
    }
    delete tombstone;
  }
  HashTable(const HashTable&) = delete;
  HashTable(const HashTable&&) = delete;
  HashTable& operator= (const HashTable&) = delete;
  HashTable& operator= (const HashTable&&) = delete;
 private:
  struct TableNode {
    T key;
    TableNode(T key) : key(std::move(key)) {}
  };
  TableNode* tombstone;
  size_t size;
  size_t capacity;
  const double loadFactor;
  std::vector<TableNode*> table;
  Hash hash;
  KeyEqual equals;
  inline size_t probe(const size_t hash, const size_t i) const;
  void grow();
};

template<class T, class Hash, class KeyEqual>
bool HashTable<T, Hash, KeyEqual>::Has(const T& key) const {
  size_t hash = this->hash(key, capacity);
  for (size_t i = 0; i < capacity; ++i) {
    hash = probe(hash, i);
    if (table[hash] == nullptr) return false;
    if (table[hash] != tombstone && equals(table[hash]->key, key)) return true;
  }
  return false;
}

template<class T, class Hash, class KeyEqual>
bool HashTable<T, Hash, KeyEqual>::Add(const T& key) {
  size_t hash = this->hash(key, capacity);
  size_t placeToInsert = 0;
  for (size_t i = 0; i < capacity; ++i) {
    hash = probe(hash, i);
    if (table[hash] == nullptr) {
      placeToInsert = hash;
      break;
    }
    if (table[hash] == tombstone) {
      placeToInsert = hash;
    } else if (equals(table[hash]->key, key)) {
      return false;
    }
  }
  table[placeToInsert] =  new TableNode(key);
  ++size;
  if (size > capacity * loadFactor) grow();
  return true;
}

template<class T, class Hash, class KeyEqual>
void HashTable<T, Hash, KeyEqual>::grow() {
  std::vector<TableNode*> oldTable = std::move(table);
  capacity *= 2;
  table = std::vector<TableNode*>(capacity, nullptr);
  for (TableNode* e : oldTable) {
    if (e != nullptr && e != tombstone) {
      size_t hash = this->hash(e->key, capacity);
      for (size_t i = 0; table[hash] != nullptr && i < capacity; ++i) {
        hash = probe(hash, i);
      }
      table[hash] = e;
    }
  }
}

template<class T, class Hash, class KeyEqual>
bool HashTable<T, Hash, KeyEqual>::Remove(const T& key) {
  size_t hash = this->hash(key, capacity);
  for (size_t i = 0; i < capacity; ++i) {
    hash = probe(hash, i);
    if (table[hash] == nullptr) return false;
    if (table[hash] != tombstone && equals(table[hash]->key, key)) {
        delete table[hash];
        table[hash] = tombstone;
        --size;
        return true;
    }
  }
}

// Квадратичное пробирование
template<class T, class Hash, class KeyEqual>
inline size_t HashTable<T, Hash, KeyEqual>::probe(size_t hashPrev, size_t i) const {
  return (hashPrev + i) % capacity;
}


class StringPolynomialHash {
  const size_t a = 13;
public:
  size_t operator()(const std::string& key, size_t maxValue) const {
    size_t hash = 0;
    for(const char& c : key) {
      hash = (hash * a + c) % maxValue;
    }
    return hash;
  }
};


int main() {
  HashTable<std::string, StringPolynomialHash> table;
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
