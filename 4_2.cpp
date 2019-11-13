/*4_2. ѕор€дкова€ статистика

ƒаны неотрицательные целые числа n,k и массив целых чисел из [0..10^9] размера n.
“ребуетс€ найти k-ю пор€дковую статистику. т.е. напечатать число, которое бы сто€ло на позиции с индексом k (0..n-1)
в отсортированном массиве. Ќапишите нерекурсивный алгоритм.

“ребовани€ к дополнительной пам€ти: O(n). “ребуемое среднее врем€ работы: O(n).

–еализуйте стратегию выбора опорного элемента Умедиана трЄхФ. ‘ункцию Partition реализуйте
методом прохода двум€ итераторами от конца массива к началу.
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


// ¬ыбирает пивот по стратегии "медиана трех"
// ¬озвращает индекс, на который встанет пивот после разделени€.
// » выполн€ет частичную сортировку относительно пивота
int Partition(int* a, int size) {
    assert(size > 0);
    if (size == 1) {
        return 0;
    }
    int* pivot = MedianOfThree(a[0], a[size / 2], a[size - 1]);
    // ѕройдем двум€ итераторами от конца массива к началу
    // ѕивот - в начале массива
    std::swap(a[0], *pivot);
    pivot = a;
    // i указывает на конец группы элементов меньших или равных пивоту
    // j указывает на первый нерассмотренный элемент (рассмотрение с конца)
    int i = size, j = size - 1;
    // –ассматриваем все элементы до пивота
    while (j != 0) {
        // Ёлементы большие пивота присоедин€ем к их группе в конце массива
        if (a[j] > *pivot) {
            std::swap(a[j], a[--i]); // i изначально равен size, его декреминтируем на 1 раз больше
        }
        --j;
    }
    // —тавим пивот на его пор€дковую позицию
    std::swap(a[--i], *pivot);
    // возвращаем статистику пивота
    return i;
}


int OrderStatistic(int* a, int size, int order) {
    assert(size > 0);
    assert(order >= 0);
    assert(order < size);
    // »щем статистику на промежутке [begin, end)
    int begin = 0, end = size;
    while (true) {
        int pivotOrder = Partition(a + begin, end - begin);
        if (pivotOrder == order) {
            return a[begin + pivotOrder];
        } else if (order < pivotOrder) { // нужно искать слева
            end = begin + pivotOrder;
        } else if (order > pivotOrder) { // нужно искать справа
            int beginNew = begin + pivotOrder + 1; // прежний пивот больше не рассматриваетс€
            order -=  beginNew - begin;
            begin = beginNew;
        }
    }
}

/*
    ¬рем€ работы - в среднем O(n):
  врем€ работы Partition - O(n); в среднем, можно считать, пивот разбивает массив пополам, так что
  первый раз Partition обрабатывает весь массив, второй раз - половину, третий - четверть и т.д. в сумме O(2n)
    ƒоп. пам€ти используетс€ O(1)
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
