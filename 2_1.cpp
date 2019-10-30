/*2_1. Жадина

Вовочка ест фрукты из бабушкиной корзины.
В корзине лежат фрукты разной массы. Вовочка может поднять не более K грамм.
Каждый фрукт весит не более K грамм. За раз он выбирает несколько самых тяжелых фруктов,
которые может поднять одновременно, откусывает от каждого половину и кладет огрызки обратно в корзину.
Если фрукт весит нечетное число грамм, он откусывает большую половину. Фрукт массы 1гр он съедает полностью.

Определить за сколько подходов Вовочка съест все фрукты в корзине.

Формат ввода
Вначале вводится n - количество фруктов и n строк с массами фруктов. n ≤ 50000.
Затем K - "грузоподъемность". K ≤ 1000.

Формат вывода
Неотрицательное число - количество подходов к корзине.*/

#include <iostream>
#include <cassert>
#include <stdexcept>
#include <vector>

template<typename T, typename Compare = std::less<T> >
class Heap {
public:
    Heap(const std::vector<T>& arr): arr(arr) {
        BuildHeap(this->arr);
    }

    T top() const {
        if (empty())
            throw std::out_of_range("Heap is empty!");
        return arr[0];
    }

    T pop() {
        if (empty())
            throw std::out_of_range("Heap is empty!");
        T t = arr[0];
        arr[0] = arr[arr.size() - 1];
        arr.pop_back();
        SiftDown(0);
        return t;
    }

    void push(T element) {
        arr.push_back(element);
        SiftUp(arr.size() - 1);
    }

    bool empty() const {
        return arr.empty();
    }

private:
    std::vector<T> arr;

    Compare comp;

    void BuildHeap(std::vector<T>& arr) {
        for (int i = arr.size() / 2 - 1; i >= 0; --i) {
            SiftDown(i);
        }
    }

    void SiftDown(int i) {
        int largest = i;
        if (2 * i + 1 < arr.size() && comp(arr[i], arr[2 * i + 1]))
            largest = 2 * i + 1;
        if (2 * i + 2 < arr.size() && comp(arr[largest], arr[2 * i + 2]))
            largest = 2 * i + 2;

        if (largest != i) {
            T t = arr[i];
            arr[i] = arr[largest];
            arr[largest] = t;
            SiftDown(largest);
        }
    }

    void SiftUp(int i) {
        if (i - 1 >= 0 && comp(arr[(i - 1) / 2], arr[i])) {
            T t = arr[i];
            arr[i] = arr[(i - 1) / 2];
            arr[(i - 1) / 2] = t;
            SiftUp((i - 1) / 2);
        }
    }
};

/* по времени: O(logK * n * logn)
для "откусывания" каждого фрукта потребуется вызов pop() стоимостью O(logN)
масса каждого фрукта <= K, следовательно, его нельзя съесть больше чем за logK откусываний
всего откусываний понадобится n * logK
*/
/* по памяти: O(n)
Потому что куча копирует передаваемый массив.
А если сохранять ссылку и менять внешний массив, то O(1) дополнительной памяти.
*/
int solution(int n, const std::vector<int>& v, int K) {
    Heap<int> heap(v);

    int answer = 0;

    while (!heap.empty()) {
        // складываем сюда веса фруктов, выбранных на каждом "подходе"
        std::vector<int> chosenFruits;
        int chosenFruitsWeight = 0;
        // достаем фрукты с максимальным весом, пока их общий вес не превышает K
        while (!heap.empty() && chosenFruitsWeight + heap.top() <= K) {
            chosenFruitsWeight += heap.top();
            chosenFruits.push_back(heap.pop());
        }
        // возвращаем обратно оставшиеся половинки фруктов
        for (int i = 0; i < chosenFruits.size(); ++i) {
            if (chosenFruits[i] > 1) {
                heap.push(chosenFruits[i] / 2);
            }
        }
        // считаем подходы
        ++answer;
    }
    return answer;
}


int main() {
    int n = 0;
    std::cin >> n;
    assert(n <= 50000);

    std::vector<int> v(n);
    for (int i = 0; i < n; ++i) {
        std::cin >> v[i];
    }

    int K = 0;
    std::cin >> K;
    assert(K <= 1000);

    for (int i = 0; i < n; ++i) {
        assert(v[i] <= K);
    }

    std::cout << solution(n, v, K);

    return 0;
}
