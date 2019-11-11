/*1_3. Очередь с помощью стека

Реализовать очередь с помощью двух стеков.
Использовать стек, реализованный с помощью динамического буфера.

Обрабатывать команды push back и pop front.

Формат ввода
В первой строке количество команд n. n ≤ 1000000.

Каждая команда задаётся как 2 целых числа: a b.

a = 2 - pop front
a = 3 - push back

Если дана команда pop front, то число b - ожидаемое значение.
Если команда pop front вызвана для пустой структуры данных, то ожидается “-1”.

Формат вывода
Требуется напечатать YES - если все ожидаемые значения совпали.
Иначе, если хотя бы одно ожидание не оправдалось, то напечатать NO.*/

#include <iostream>
#include <cassert>
#include <stdexcept>

template<class T>
class Stack {
public:
    Stack() = default;

    Stack(const Stack& x)
    : size(x.size)
    , capacity(x.capacity)
    , arr(new T[capacity]) {
        std::copy(x.arr, x.arr + size, arr);
    }

    Stack(Stack&& x)
    : size(x.size)
    , capacity(x.capacity)
    , arr(x.arr) {
        x.arr = nullptr;
    }

    Stack& operator= (const Stack& x) {
        if (this == &x) return *this;
        size = x.size;
        capacity = x.capacity;
        delete[] arr;
        arr = new T[capacity];
        std::copy(x.arr, x.arr + size, arr);
        return *this;
    }

    Stack& operator= (Stack&& x) {
        if (this == &x) return *this;
        size = x.size;
        capacity = x.capacity;
        delete[] arr;
        arr = x.arr;
        x.arr = nullptr;
        return *this;
    }

    ~Stack() {
        delete[] arr;
    };

    void push(const T& val) {
        if (size == capacity)
            grow();
        arr[size++] = val;
    }

    void push(T&& val) {
        if (size == capacity)
            grow();
        arr[size++] = val;
    }

    T pop() {
        if (empty())
            throw std::out_of_range("Stack is empty!");
        return arr[--size];
    }

    T top() const {
        if (empty())
            throw std::out_of_range("Stack is empty!");
        return arr[size - 1];
    }

    bool empty() const {
        return size == 0;
    }

private:
    int size = 0;
    int capacity = 10;
    T* arr = new T[capacity];

    void grow() {
        T* t = arr;
        capacity *= 1.5;
        arr = new T[capacity];
        std::copy(t, t + size, arr);
        delete[] t;
    }
};


template<class T>
class Queue {
public:
    Queue() = default;
    Queue(const Queue&) = default;
    Queue(const Queue&&) = default;
    Queue& operator= (const Queue&) = default;
    Queue& operator= (const Queue&&) = default;
    ~Queue() = default;

    void push_back(const T& value) {
        in.push(value);
    }

    void push_back(T&& value) {
        in.push(value);
    }

    T pop_front() {
        try {
            if (out.empty()) {
                while (!in.empty()) {
                    out.push(in.pop());
                }
            }
            return out.pop(); // если оба стека пусты - out будет пуст и выбросится исключение
        } catch (const std::exception& e) {
            throw e;
        }
    }

    bool empty() const {
        return in.empty() && out.empty();
    }

private:
    Stack<T> in;
    Stack<T> out;
};

/*
 по времени операция push_back() - O(1);
 pop_front() - в лучшем случае O(1), в худшем O(n), n - число элементов в стеке,
 но амортизационная стоимость pop_front() = O(1):
 если в стеке "in" K элементов, а стек "out" пуст (худший случай), то pop_front() выполнится за O(K);
 пусть когда первый раз произошел "долгий pop" в стеке "in" было K1 элементов, во второй - K2 и т.д.
 на n таких случаев уйдет O(K1) + O(K2) + ... + O(Kn),
 число осуществленных pop'ов за все это время будет K1 + K2 + ... + Kn => амортизационная стоимость = O(1)
*/
int main() {
    int n = 0;
    std::cin >> n;
    assert(n <= 1000000);

    int a = 0;
    int value = 0;
    Queue<int> queue;
    int i = 0;

    for (i = 0; i < n; ++i) {
        std::cin >> a;
        assert(a == 2 || a == 3);
        std::cin >> value;
        if (a == 3) {
            queue.push_back(value);
        } else {
            try {
                if (queue.pop_front() != value)
                    break;
            } catch (const std::exception& e) {
                if (value != -1)
                    break;
            }
        }
    }

    if (i == n)
        std::cout << "YES";
    else
        std::cout << "NO";

    return 0;
}
