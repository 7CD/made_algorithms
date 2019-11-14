/* 6_3. Обход дерева в порядке post-order

Дано число N ≤ 10^4 и последовательность целых чисел из [-2^31..2^31] длиной N.
Требуется построить бинарное дерево, заданное наивным порядком вставки.
Т.е., при добавлении очередного числа K в дерево с корнем root, если root->Key ≤ K,
то узел K добавляется в правое поддерево root;
иначе в левое поддерево root. Выведите элементы в порядке post-order (снизу вверх).
Рекурсия запрещена.
*/

#include <iostream>
#include <cassert>

template<class T>
struct TreeNode {
    T key;
    TreeNode<T>* parent = nullptr;
    TreeNode<T>* left = nullptr;
    TreeNode<T>* right = nullptr;

    explicit TreeNode(T key): key(key) {};
    TreeNode(const TreeNode&) = delete;
    TreeNode(const TreeNode&&) = delete;
    TreeNode& operator= (const TreeNode&) = delete;
    TreeNode& operator= (const TreeNode&&) = delete;
};


template<class T, class Compare = std::less<T>>
class Tree {
public:
    TreeNode<T>* head = nullptr;
    Tree() = default;
    ~Tree() {
        for (auto it = begin(); it != end(); ++it) {
            delete it.pointer;
        }
    }

    void insert (T key) {
        TreeNode<T>* newNode = new TreeNode<T>(key);
        TreeNode<T>* x = head; // узел - место для вставки
        TreeNode<T>* y = nullptr; // его родитель
        while (x != nullptr) {
            y = x;
            if (comp(newNode->key, x->key))
                x = x->left;
            else
                x = x->right;
        }
        newNode->parent = y;
        if (y == nullptr)
            head = newNode;
        else if (comp(newNode->key, y->key))
            y->left = newNode;
        else
            y->right = newNode;
    }

private:
    class Iterator: public std::iterator<std::input_iterator_tag, T> {
        friend class Tree;
        const TreeNode<T>* pointer; // указатель на текущий узел
        const TreeNode<T>* pointerParent; // указатель на его родителя
        bool visited_left = false; // пройдено ли левое поддерево текущего узла
        bool visited_right = false; // пройдено ли правое поддерево текущего узла
    public:
        Iterator(TreeNode<T>* p): pointer(p) {};

        // Обход дерева в порядке post-order
        Iterator& operator++() {
            assert(pointer != nullptr);
            while (true) {
                // 4) Выполняется после того, как распечатали текущий узел. Меняем текущий на родителя.
                if(visited_left && visited_right) {
                    // если переходим к родителю слева, то visited_right устанавливаем false
                    if (pointerParent != nullptr && pointerParent->left == pointer)
                        visited_right = false;
                    pointer = pointerParent;
                }
                // 1) Идем влево до упора
                if (!visited_left)
                    while (pointer->left != nullptr)
                        pointer = pointer->left;
                visited_left = true;
                // 2) Делаем один шаг вправо, если возможно. В правом поддереве можно шагать влево.
                if (!(visited_right || pointer->right == nullptr)) {
                    pointer = pointer->right;
                    visited_left = false;
                } // если шагнули вправо, уходим на след итерацию. Иначе:
                // 3) Ни вправо, ни влево не надо, выводим этот узел и переходим к его родителю. Visited_right и left = true
                else {
                    if (pointer) pointerParent = pointer->parent; // Переменная pointerParent необходима для деструктора: после того как вернем и освободим pointer, не сможем узнать его parent
                    visited_right = true;
                    return *this;
                }
            }
        }
        const T& operator*() const {
            return pointer->key;
        }
        bool operator!=(const Iterator& other) const {
            return pointer != other.pointer;
        }
    };

public:
    Iterator begin() const {
        return ++Iterator(head);
    }
    Iterator end() const {
        return Iterator(nullptr);
    }
private:
    Compare comp;
    Tree(const Tree&) = delete;
    Tree(const Tree&&) = delete;
    Tree& operator= (const Tree&) = delete;
    Tree& operator= (const Tree&&) = delete;
};


int main() {
    int n = 0;
    std::cin >> n;
    assert(n > 0);
    assert(n <= 10000);

    Tree<int> tree;
    int k = 0;
    for (int i = 0; i < n; ++i) {
        std::cin >> k;
        tree.insert(k);
    }

    for (auto e : tree) {
        std::cout << e << " ";
    }

    return 0;
}
