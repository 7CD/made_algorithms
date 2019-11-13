/*4_2. ���������� ����������

���� ��������������� ����� ����� n,k � ������ ����� ����� �� [0..10^9] ������� n.
��������� ����� k-� ���������� ����������. �.�. ���������� �����, ������� �� ������ �� ������� � �������� k (0..n-1)
� ��������������� �������. �������� ������������� ��������.

���������� � �������������� ������: O(n). ��������� ������� ����� ������: O(n).

���������� ��������� ������ �������� �������� �������� ����. ������� Partition ����������
������� ������� ����� ����������� �� ����� ������� � ������.
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


// �������� ����� �� ��������� "������� ����"
// ���������� ������, �� ������� ������� ����� ����� ����������.
// � ��������� ��������� ���������� ������������ ������
int Partition(int* a, int size) {
    assert(size > 0);
    if (size == 1) {
        return 0;
    }
    int* pivot = MedianOfThree(a[0], a[size / 2], a[size - 1]);
    // ������� ����� ����������� �� ����� ������� � ������
    // ����� - � ������ �������
    std::swap(a[0], *pivot);
    pivot = a;
    // i ��������� �� ����� ������ ��������� ������� ��� ������ ������
    // j ��������� �� ������ ��������������� ������� (������������ � �����)
    int i = size, j = size - 1;
    // ������������� ��� �������� �� ������
    while (j != 0) {
        // �������� ������� ������ ������������ � �� ������ � ����� �������
        if (a[j] > *pivot) {
            std::swap(a[j], a[--i]); // i ���������� ����� size, ��� �������������� �� 1 ��� ������
        }
        --j;
    }
    // ������ ����� �� ��� ���������� �������
    std::swap(a[--i], *pivot);
    // ���������� ���������� ������
    return i;
}


int OrderStatistic(int* a, int size, int order) {
    assert(size > 0);
    assert(order >= 0);
    assert(order < size);
    // ���� ���������� �� ���������� [begin, end)
    int begin = 0, end = size;
    while (true) {
        int pivotOrder = Partition(a + begin, end - begin);
        if (pivotOrder == order) {
            return a[begin + pivotOrder];
        } else if (order < pivotOrder) { // ����� ������ �����
            end = begin + pivotOrder;
        } else if (order > pivotOrder) { // ����� ������ ������
            int beginNew = begin + pivotOrder + 1; // ������� ����� ������ �� ���������������
            order -=  beginNew - begin;
            begin = beginNew;
        }
    }
}

/*
    ����� ������ - � ������� O(n):
  ����� ������ Partition - O(n); � �������, ����� �������, ����� ��������� ������ �������, ��� ���
  ������ ��� Partition ������������ ���� ������, ������ ��� - ��������, ������ - �������� � �.�. � ����� O(2n)
    ���. ������ ������������ O(1)
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
