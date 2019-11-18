/* 7_4. Порядковые статистики Сплей

Формат ввода
Дано число N и N строк. (1 ≤ N ≤ 10000)
Каждая строка содержит команду добавления или удаления натуральных чисел (от 1 до 10^9),
а также запрос на получение k-ой порядковой статистики (0 ≤ k < N).
Команда добавления числа A задается положительным числом A,
команда удаления числа A задается отрицательным числом “-A”.

Формат вывода
N строк. В каждой строке - текущая k-ая порядковая статистика из запроса.

Требуемая скорость выполнения запроса - O(log n) амортизированно.
В реализации используйте сплей-деревья.
*/

#include <iostream>
#include <cassert>

template<class T, class Compare = std::less<T>>
class SplayTree {
private:
    struct TreeNode {
        T key;
        TreeNode* parent = nullptr;
        TreeNode* left = nullptr;
        TreeNode* right = nullptr;
        int leftSubtreeSize = 0; // Требуется для нахождения порядковой статистики за O(log(n))
        int rightSubtreeSize = 0; // Требуется для поддержания leftSubtreeSize при поворотах
        explicit TreeNode(const T& key): key(key) {};
        ~TreeNode() {
            delete left;
            delete right;
        }
    };
public:
    TreeNode* head = nullptr;
    int size = 0;
    SplayTree() = default;
    ~SplayTree() {
        delete head;
    }

    void insert(T key) {
        TreeNode* x = head; // узел - место для вставки
        TreeNode* y = nullptr; // его родитель
        while (x != nullptr) {
            y = x;
            if (comp(key, x->key)) {
                ++(x->leftSubtreeSize);
                x = x->left;
            } else if (comp(x->key, key)) {
                ++(x->rightSubtreeSize);
                x = x->right;
            } else { // В задаче предполагается, что все ключи уникальны
                assert(false);
            }
        }
        TreeNode* newNode = new TreeNode(key);
        newNode->parent = y;
        if (y == nullptr)
            head = newNode;
        else if (comp(newNode->key, y->key))
            y->left = newNode;
        else
            y->right = newNode;
        ++size;
        splay(newNode);
    }

    void remove(const T& key) {
        TreeNode* nodeToDelete = search(key);
        if(!nodeToDelete) return;
        merge(nodeToDelete->left, nodeToDelete->right);
        --size;
        nodeToDelete->left = nullptr;
        nodeToDelete->right = nullptr;
        delete nodeToDelete;
    }

    const T& orderStatistic(int order) const {
        assert(order >= 0);
        assert(order < size);
        TreeNode* x = head; // текущий узел поиска
        int otherLeftNodes = 0; // узлы, которые левее x, но не дочерние по отн. к x
        while (x != nullptr) {
            if (comp(order, x->leftSubtreeSize + otherLeftNodes)) {
                x = x->left;
            } else if (comp(x->leftSubtreeSize + otherLeftNodes, order)) {
                otherLeftNodes += x->leftSubtreeSize + 1;
                x = x->right;
            } else { // элемент найден в узле x
                return x->key;
            }
        }
    }
private:
    TreeNode* search(const T& key) {
        TreeNode* x = head; // текущий узел поиска
        TreeNode* y = nullptr; // родитель x
        while (x != nullptr) {
            y = x;
            if (comp(key, x->key))
                x = x->left;
            else if (comp(x->key, key))
                x = x->right;
            else // элемент найден в узле x. В этом случае y == x
                break;
        }
        splay(y);
        return x; // если элемент не найден, x = nullptr
    }

    void merge(TreeNode* leftTreeHead, TreeNode* rightTreeHead) {
        if (!leftTreeHead && !rightTreeHead) {
            this->head = nullptr;
        } else if (!leftTreeHead) {
            this->head = rightTreeHead;
            rightTreeHead->parent = nullptr;
        } else if (!rightTreeHead) {
            this->head = leftTreeHead;
            leftTreeHead->parent = nullptr;
        } else {
            TreeNode* leftTreeMax = leftTreeHead;
            while (leftTreeMax->right != nullptr) {
                leftTreeMax = leftTreeMax->right;
            }
            splay(leftTreeMax);
            leftTreeMax->right = rightTreeHead;
            rightTreeHead->parent = leftTreeMax;
            leftTreeMax->rightSubtreeSize = rightTreeHead->leftSubtreeSize + rightTreeHead->rightSubtreeSize + 1;
        }
    }

    void splay(TreeNode* node) {
        if (node == head || !node) return;
        while (true) {
            if (node == head->left) {                            // левый сын корня
                zig(node);
                break;
            } else if (node == head->right) {                    // правый сын корня
                zag(node);
                break;
            } else if (node == node->parent->left) {
                if (node->parent == node->parent->parent->left) {// узел и его родитель - левые дети
                    zigZig(node);
                    if (head->parent && head->parent->parent == node) break;
                } else {                                         // узел - левый ребенок, его родитель - правый ребенок
                    zigZag(node);
                    if (head->parent == node) break;
                }
            } else {
                if (node->parent == node->parent->parent->right) {// узел и его родитель - правые дети
                    zagZag(node);
                    if (head->parent && head->parent->parent == node) break;
                } else {                                          // узел - правый ребенок, его родитель - левый ребенок
                    zagZig(node);
                    if (head->parent == node) break;
                }
            }
        }
        head = node;
    }

    // Правый поворот
    void zig(TreeNode* node) {
        assert(node);
        assert(node->parent);
        TreeNode* nodeFormerParent = node->parent;
        nodeFormerParent->left = node->right;
        if (node->right) node->right->parent = nodeFormerParent;
        node->right = nodeFormerParent;
        node->parent = nodeFormerParent->parent;
        nodeFormerParent->parent = node;
        if(node->parent) {
            if (node->parent->left == nodeFormerParent)
                node->parent->left = node;
            else
                node->parent->right = node;
        }
        nodeFormerParent->leftSubtreeSize = (nodeFormerParent->left) ? nodeFormerParent->left->leftSubtreeSize + nodeFormerParent->left->rightSubtreeSize + 1 : 0;
        node->rightSubtreeSize = nodeFormerParent->leftSubtreeSize + nodeFormerParent->rightSubtreeSize + 1;
    }
    // Левый поворот
    void zag(TreeNode* node) {
        assert(node);
        assert(node->parent);
        TreeNode* nodeFormerParent = node->parent;
        nodeFormerParent->right = node->left;
        if (node->left) node->left->parent = nodeFormerParent;
        node->left = nodeFormerParent;
        node->parent = nodeFormerParent->parent;
        nodeFormerParent->parent = node;
        if(node->parent) {
            if (node->parent->left == nodeFormerParent)
                node->parent->left = node;
            else
                node->parent->right = node;
        }
        nodeFormerParent->rightSubtreeSize = (nodeFormerParent->right) ? nodeFormerParent->right->leftSubtreeSize + nodeFormerParent->right->rightSubtreeSize + 1 : 0;
        node->leftSubtreeSize = nodeFormerParent->leftSubtreeSize + nodeFormerParent->rightSubtreeSize + 1;
    }

    void zigZig(TreeNode* node) {
        zig(node->parent);
        zig(node);
    }

    void zigZag(TreeNode* node) {
        zig(node);
        zag(node);
    }

    void zagZig(TreeNode* node) {
        zag(node);
        zig(node);
    }

    void zagZag(TreeNode* node) {
        zag(node->parent);
        zag(node);
    }

    Compare comp;
    SplayTree(const SplayTree&) = delete;
    SplayTree(const SplayTree&&) = delete;
    SplayTree& operator= (const SplayTree&) = delete;
    SplayTree& operator= (const SplayTree&&) = delete;
};

/*
  Время поиска порядковой статистики - O(log n) амортизированно, т.к.
  высота Splay дерева O(log n) амортизированно, и каждый узел содержит доп. информацию - число узлов в левом поддереве,
  которая позволяет найти ответ за проход от корня к листу стого вниз.
*/
int main() {
    int n = 0;
    std::cin >> n;
    assert(n > 0);
    assert(n <= 10000);

    SplayTree<int> splayTree;
    int e, k = 0;
    for (int i = 0; i < n; ++i) {
        std::cin >> e >> k;
        if (e > 0)
            splayTree.insert(e);
        else if (e < 0)
            splayTree.remove(-e);
        else
            assert(e != 0);
        std::cout << splayTree.orderStatistic(k) <<  " ";
    }
    return 0;
}
