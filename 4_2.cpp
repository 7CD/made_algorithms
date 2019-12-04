/* 4_2. Порядковая статистика

Даны неотрицательные целые числа n,k и массив целых чисел из [0..10^9] размера n.
Требуется найти k-ю порядковую статистику. т.е. напечатать число, которое бы стояло на позиции с индексом k (0..n-1)
в отсортированном массиве. Напишите нерекурсивный алгоритм.

Требования к дополнительной памяти: O(n). Требуемое среднее время работы: O(n).

Реализуйте стратегию выбора опорного элемента “медиана трёх”. Функцию Partition реализуйте
методом прохода двумя итераторами от конца массива к началу.
*/

#include <iostream>
#include <cassert>

int* MedianOfThree(int& a, int& b, int& c) {
    if (a <= b) {
        if (b <= c)
            return &b;
        return (a <= c) ? &c : &a;
    } else {
        if (a <= c)
            return &a;
        return (b <= c) ? &c : &b;
    }
}


// Выбирает пивот по стратегии "медиана трех"
// Возвращает индекс, на который встанет пивот после разделения.
// И выполняет частичную сортировку относительно пивота
int Partition(int* a, int size) {
    assert(size > 0);
    if (size == 1) {
        return 0;
    }
    int* pivot = MedianOfThree(a[0], a[size / 2], a[size - 1]);
    // Пройдем двумя итераторами от конца массива к началу
    // Пивот - в начале массива
    std::swap(a[0], *pivot);
    pivot = a;
    // i указывает на конец группы элементов меньших или равных пивоту
    // j указывает на первый нерассмотренный элемент (рассмотрение с конца)
    int i = size, j = size - 1;
    // Рассматриваем все элементы до пивота
    while (j != 0) {
        // Элементы большие пивота присоединяем к их группе в конце массива
        if (a[j] > *pivot) {
            std::swap(a[j], a[--i]); // i изначально равен size, его декреминтируем на 1 раз больше
        }
        --j;
    }
    // Ставим пивот на его порядковую позицию
    std::swap(a[--i], *pivot);
    // возвращаем статистику пивота
    return i;
}


int OrderStatistic(int* a, int size, int order) {
    assert(size > 0);
    assert(order >= 0);
    assert(order < size);
    // Ищем статистику на промежутке [begin, end)
    int begin = 0, end = size;
    while (true) {
        int pivotOrder = Partition(a + begin, end - begin);
        if (pivotOrder == order) {
            return a[begin + pivotOrder];
        } else if (order < pivotOrder) { // нужно искать слева
            end = begin + pivotOrder;
        } else if (order > pivotOrder) { // нужно искать справа
            int beginNew = begin + pivotOrder + 1; // прежний пивот больше не рассматривается
            order -=  beginNew - begin;
            begin = beginNew;
        }
    }
}

/*
    Время работы - в среднем O(n):
  время работы Partition - O(n); в среднем, можно считать, пивот разбивает массив пополам, так что
  первый раз Partition обрабатывает весь массив, второй раз - половину, третий - четверть и т.д. в сумме O(2n)
    Доп. памяти используется O(1)
*/

int main() {
    int n;
    std::cin >> n;
    assert(n > 0);

    int k;
    std::cin >> k;
    assert(k >= 0);
    assert(k < n);

    int* a = new int[n];
    for (int i = 0; i < n; ++i) {
        std::cin >> a[i];
    }

    std::cout << OrderStatistic(a, n, k);

    delete[] a;
    return 0;
}
