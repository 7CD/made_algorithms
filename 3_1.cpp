/* 3_1. Первые k элементов длинной последовательности

Дана очень длинная последовательность целых чисел длины n.
Требуется вывести в отсортированном виде её наименьшие k элементов.
Последовательность может не помещаться в память. Время работы O(n * log(k)).
Доп. память O(k). Использовать слияние.
*/

#include <iostream>
#include <cassert>

template <class InputIt, class OutputIt, class Compare = std::less<>>
void Merge(InputIt first1, InputIt last1, InputIt first2, InputIt last2, OutputIt outIt, Compare comp = Compare()) {
    assert(last1 >= first1);
    assert(last2 >= first2);
    while (true) {
        if (first1 == last1) {
            std::copy(first2, last2, outIt);
            return;
        } else if(first2 == last2) {
            std::copy(first1, last1, outIt);
            return;
        }

        if (comp(*first2, *first1)) {
            *(outIt++) = *(first2++);
        } else { // если элемент из первого массива >= элемента из второго
            *(outIt++) = *(first1++);
        }
    }
}


template <class RandomAccessIterator, class Compare = std::less<>>
void MergeSort(RandomAccessIterator first, RandomAccessIterator last, Compare comp = Compare()) {
    assert(last >= first);
    int len = last - first;
    if (len <= 1) {
        return;
    }
    MergeSort(first, first + len / 2, comp); // сортируем первую половину
    MergeSort(first + len / 2, last, comp); // сортируем вторую половину
    // Заводим массив, куда будем сливать
    auto tmp = new typename std::iterator_traits<RandomAccessIterator>::value_type [len];
    // Сливаем первую половину и вторую половину в tmp
    Merge(first, first + len / 2, first + len / 2, last, tmp, comp);
    std::copy(tmp, tmp + len, first); // копируем в first его отсортированное содержимое
    delete[] tmp;
}

// Для задачи нужно сливать два массива размера k в один массив размера k - первые k элементов
// Можно было и в сортировке использовать эту функцию, но для наглядности, и чтобы был MergeSort почище...
template <class InputIt, class OutputIt, class Compare = std::less<>>
void MergeFirstK(InputIt first1, InputIt last1, InputIt first2, InputIt last2, OutputIt outIt, int K,  Compare comp = Compare()) {
    assert(last1 >= first1);
    assert(last2 >= first2);
    int k = 0;
    while (k < K) {
        if (first1 == last1) {
            std::copy(first2, last2, outIt);
            return;
        } else if(first2 == last2) {
            std::copy(first1, last1, outIt);
            return;
        }

        if (comp(*first2, *first1)) {
            *(outIt++) = *(first2++);
            ++k;
        } else { // если элемент из первого массива >= элемента из второго
            *(outIt++) = *(first1++);
            ++k;
        }
    }
}

/*
    Время работы O(n * log(k)):
  n / k раз выполняется сортировка k элементов и слияние - k * log(k) + k. Итого, (n / k) * k * log(k) = n * log(k)
    Доп. памяти используется O(k) - 4 массива по k элементов: first, second, tmp, и при сортировке MergSort
*/

int main() {
    int n;
    std::cin >> n;
    assert(n > 0);

    int k;
    std::cin >> k;
    assert(k > 0);
    assert(k <= n);

    // Считаем первый массив из k элементов
    int* const first = new int[k];
    for (int i = 0; i < k; ++i) {
        std::cin >> first[i];
    }
    // Сортируем его
    MergeSort(first, first + k);

    // Далее будем считывать по k элементов
    // и сливать их с first
    int* const second = new int[k];
    int* const tmp = new int[k];

    for (int i = 1; i < n / k; ++i) {
        for (int j = 0; j < k; ++j) {
            std::cin >> second[j];
        }
        MergeSort(second, second + k);
        // Сливаем first и second в first, для этого копируем first в tmp
        std::copy(first, first + k, tmp);
        MergeFirstK(tmp, tmp + k, second, second + k, first, k);
    }

    // Делаем то же с оставшимися элементами
    int remainder = n % k;
    if (remainder > 0) {
        for (int j = 0; j < remainder; ++j) {
            std::cin >> second[j];
        }
        MergeSort(second, second + remainder);
        std::copy(first, first + k, tmp);
        MergeFirstK(tmp, tmp + k, second, second + remainder, first, k);
    }


    for (int i = 0; i < k; ++i) {
        std::cout << first[i] << " ";
    }

    delete[] first;
    delete[] second;
    delete[] tmp;
    return 0;
}
