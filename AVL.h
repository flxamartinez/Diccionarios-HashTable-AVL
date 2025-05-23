#ifndef AVLTree_H
#define AVLTree_H
#include <iostream>
#include <sstream>
#include "AVL_Node.h"
#include "AVL_Iterator.h"

using namespace std;

template <typename T>
class AVLTree
{
public:
    typedef AVLIterator<T> iterator;
    iterator begin(AVLIterator<int>::Type _)
    {
        return iterator(root, _);
    } // Retorna el inicio del iterador

    iterator end()
    {
        return iterator(nullptr, AVLIterator<int>::InOrder);
    } // Retorna el final del iterador

private:
    NodeAVL<T> *root;

public:
    AVLTree() : root(nullptr) {}

    void insert(T value) // O(log n)
    {
        insert(root, value);
    }

    bool find(T value) // O(log n)
    {
        NodeAVL<T> *current = root;
        while (current != nullptr)
        {
            if (value < current->data)
                current = current->left;
            else if (value > current->data)
                current = current->right;
            else
                return true;
        }
        return false;
    }

    string getInOrder()
    {
        stringstream ss;
        getInOrder(root, ss);
        return ss.str();
    }

    string getPreOrder()
    {
        stringstream ss;
        getPreOrder(root, ss);
        return ss.str();
    }

    string getPostOrder()
    {
        stringstream ss;
        getPostOrder(root, ss);
        return ss.str();
    }

    int height()
    {
        return height(root);
    }

    T minValue() // O(log n)
    {
        NodeAVL<T> *current = root;
        while (current && current->left != nullptr)
            current = current->left;
        return current->data;
    }

    T maxValue() // O(log n)
    {
        NodeAVL<T> *current = root;
        while (current && current->right != nullptr)
            current = current->right;
        return current->data;
    }

    bool isBalanced() // O(n)
    {
        return isBalanced(root);
    }

    int size()
    {
        return size(root);
    }

    void remove(T value) // Use el predecesor para cuando el nodo a eliminar tiene dos hijos
    {
        remove(root, value);
    }

    /*Adicionales*/
    T successor(T value) // Retornar el valor siguiente de "value" en el arbol
    {
        NodeAVL<T> *current = root;
        NodeAVL<T> *succ = nullptr;
        while (current)
        {
            if (value < current->data)
            {
                succ = current;
                current = current->left;
            }
            else
            {
                current = current->right;
            }
        }
        return succ ? succ->data : T();
    }

    T predecessor(T value) // Retornar el valor anterior de "value" en el arbol
    {
        NodeAVL<T> *current = root;
        NodeAVL<T> *pred = nullptr;
        while (current)
        {
            if (value > current->data)
            {
                pred = current;
                current = current->right;
            }
            else
            {
                current = current->left;
            }
        }
        return pred ? pred->data : T();
    }

    void clear() // Liberar todos los nodos (usar root->KillSelf)
    {
        if (root)
        {
            root->killSelf();
            root = nullptr;
        }
    }

    void displayPretty() // Muestra el arbol visualmente atractivo
    {
        displayPretty(root, 0);
    }

    ~AVLTree()
    {
        if (this->root != nullptr)
        {
            this->root->killSelf();
        }
    }

private:
    void insert(NodeAVL<T> *&node, T value)
    {
        if (node == nullptr)
        {
            node = new NodeAVL<T>(value);
            return;
        }

        if (value < node->data)
        {
            insert(node->left, value);
        }
        else if (value > node->data)
        {
            insert(node->right, value);
        }
        else
        {
            return;
        }

        balance(node);
    }

    void getInOrder(NodeAVL<T> *node, stringstream &ss)
    {
        if (!node)
            return;
        getInOrder(node->left, ss);
        ss << node->data << " ";
        getInOrder(node->right, ss);
    }

    void getPreOrder(NodeAVL<T> *node, stringstream &ss)
    {
        if (!node)
            return;
        ss << node->data << " ";
        getPreOrder(node->left, ss);
        getPreOrder(node->right, ss);
    }

    void getPostOrder(NodeAVL<T> *node, stringstream &ss)
    {
        if (!node)
            return;
        getPostOrder(node->left, ss);
        getPostOrder(node->right, ss);
        ss << node->data << " ";
    }

    int height(NodeAVL<T> *node)
    {
        if (!node)
            return -1;
        return node->height;
    }

    bool isBalanced(NodeAVL<T> *node)
    {
        if (!node)
            return true;
        int bf = balancingFactor(node);
        if (bf > 1 || bf < -1)
            return false;
        return isBalanced(node->left) && isBalanced(node->right);
    }

    int size(NodeAVL<T> *node)
    {
        if (!node)
            return 0;
        return 1 + size(node->left) + size(node->right);
    }

    void remove(NodeAVL<T> *&node, T value)
    {
        if (!node)
            return;

        if (value < node->data)
        {
            remove(node->left, value);
        }
        else if (value > node->data)
        {
            remove(node->right, value);
        }
        else
        {
            if (!node->left && !node->right)
            {
                delete node;
                node = nullptr;
            }
            else if (node->left && !node->right)
            {
                NodeAVL<T> *temp = node;
                node = node->left;
                delete temp;
            }
            else if (!node->left && node->right)
            {
                NodeAVL<T> *temp = node;
                node = node->right;
                delete temp;
            }
            else
            {
                NodeAVL<T> *pred = node->left;
                while (pred->right)
                    pred = pred->right;
                node->data = pred->data;
                remove(node->left, pred->data);
            }
        }

        if (node)
            balance(node);
    }

    void displayPretty(NodeAVL<T> *node, int depth)
    {
        if (!node)
            return;
        displayPretty(node->right, depth + 1);
        for (int i = 0; i < depth; ++i)
            cout << "   ";
        cout << node->data << endl;
        displayPretty(node->left, depth + 1);
    }

    /*Rotaciones del AVL*/
    int balancingFactor(NodeAVL<T> *node)
    {
        int leftHeight = (node->left) ? node->left->height : -1;
        int rightHeight = (node->right) ? node->right->height : -1;
        return leftHeight - rightHeight;
    } // Obtiene el factor de balanceo O(1)

    void updateHeight(NodeAVL<T> *node)
    {
        int leftHeight = (node->left) ? node->left->height : -1;
        int rightHeight = (node->right) ? node->right->height : -1;
        node->height = 1 + std::max(leftHeight, rightHeight);
    } // Actualiza la altura de un nodo O(1)

    void balance(NodeAVL<T> *&node)
    {
        updateHeight(node);
        int bf = balancingFactor(node);

        if (bf > 1)
        {
            if (balancingFactor(node->left) < 0)
            {
                left_rota(node->left);
            }
            right_rota(node);
        }
        else if (bf < -1)
        {
            if (balancingFactor(node->right) > 0)
            {
                right_rota(node->right);
            }
            left_rota(node);
        }
    } // Agoritmo principal que verifica el balanceo del nodo y aplica las rotaciones O(1)

    void left_rota(NodeAVL<T> *&node)
    {
        NodeAVL<T> *newRoot = node->right;
        node->right = newRoot->left;
        newRoot->left = node;
        updateHeight(node);
        updateHeight(newRoot);
        node = newRoot;
    } // Rotación a la izquier O(1)

    void right_rota(NodeAVL<T> *&node)
    {
        NodeAVL<T> *newRoot = node->left;
        node->left = newRoot->right;
        newRoot->right = node;
        updateHeight(node);
        updateHeight(newRoot);
        node = newRoot;
    } // Rotación a la derecha O(1)
};

#endif
