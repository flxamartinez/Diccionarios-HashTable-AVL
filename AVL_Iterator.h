#ifndef AVL_ITERATOR_H
#define AVL_ITERATOR_H

#include "AVL_Node.h"

template <typename T>
class AVLIterator {
public:
    enum Type {
        PreOrder,
        InOrder,
        PostOrder,
        BFS
    };

private:
    NodeAVL<T>* current;
    Type type;

    // Pila personalizada
    struct StackNode {
        NodeAVL<T>* node;
        bool visited;
        StackNode* next;
        StackNode(NodeAVL<T>* n, bool v = false, StackNode* nx = nullptr) : node(n), visited(v), next(nx) {}
    };

    StackNode* stackTop;

    void stackPush(NodeAVL<T>* node, bool visited = false) {
        stackTop = new StackNode(node, visited, stackTop);
    }

    StackNode* stackPop() {
        if (!stackTop) return nullptr;
        StackNode* top = stackTop;
        stackTop = top->next;
        return top;
    }

    bool stackEmpty() const {
        return stackTop == nullptr;
    }

    void clearStack() {
        while (!stackEmpty()) {
            delete stackPop();
        }
    }

    void copyStack(StackNode* otherTop) {
        if (!otherTop) return;
        StackNode* nodes[1000];
        int n = 0;
        StackNode* temp = otherTop;
        while (temp) {
            nodes[n++] = temp;
            temp = temp->next;
        }
        for (int i = n - 1; i >= 0; --i) {
            stackPush(nodes[i]->node, nodes[i]->visited);
        }
    }

    // Cola personalizada
    struct QueueNode {
        NodeAVL<T>* node;
        QueueNode* next;
        QueueNode(NodeAVL<T>* n) : node(n), next(nullptr) {}
    };

    QueueNode* queueFront = nullptr;
    QueueNode* queueBack = nullptr;

    void enqueue(NodeAVL<T>* node) {
        QueueNode* newNode = new QueueNode(node);
        if (!queueBack) {
            queueFront = queueBack = newNode;
        } else {
            queueBack->next = newNode;
            queueBack = newNode;
        }
    }

    NodeAVL<T>* dequeue() {
        if (!queueFront) return nullptr;
        QueueNode* temp = queueFront;
        NodeAVL<T>* result = temp->node;
        queueFront = queueFront->next;
        if (!queueFront) queueBack = nullptr;
        delete temp;
        return result;
    }

    bool queueEmpty() const {
        return queueFront == nullptr;
    }

    void clearQueue() {
        while (!queueEmpty()) {
            dequeue();
        }
    }

    void copyQueue(QueueNode* otherFront) {
        queueFront = queueBack = nullptr;
        QueueNode* temp = otherFront;
        while (temp) {
            enqueue(temp->node);
            temp = temp->next;
        }
    }

    void initInOrder(NodeAVL<T>* root) {
        clearStack();
        current = nullptr;
        while (root) {
            stackPush(root);
            root = root->left;
        }
        ++(*this);
    }

    void initPreOrder(NodeAVL<T>* root) {
        clearStack();
        current = nullptr;
        if (root) stackPush(root);
        ++(*this);
    }

    void initPostOrder(NodeAVL<T>* root) {
        clearStack();
        current = nullptr;
        while (root) {
            stackPush(root, false);
            root = root->left;
        }
        ++(*this);
    }

    void initBFS(NodeAVL<T>* root) {
        clearQueue();
        current = nullptr;
        if (root) enqueue(root);
        ++(*this);
    }

public:
    AVLIterator() : current(nullptr), stackTop(nullptr), queueFront(nullptr), queueBack(nullptr), type(InOrder) {}

    AVLIterator(NodeAVL<T>* root, Type _type) : current(nullptr), stackTop(nullptr), queueFront(nullptr), queueBack(nullptr), type(_type) {
        switch (_type) {
            case InOrder: initInOrder(root); break;
            case PreOrder: initPreOrder(root); break;
            case PostOrder: initPostOrder(root); break;
            case BFS: initBFS(root); break;
        }
    }

    // Constructor de copia
    AVLIterator(const AVLIterator& other) {
        current = other.current;
        type = other.type;
        stackTop = nullptr;
        queueFront = queueBack = nullptr;

        if (type == BFS)
            copyQueue(other.queueFront);
        else
            copyStack(other.stackTop);
    }

    // Operador de asignación
    AVLIterator& operator=(const AVLIterator& other) {
        if (this != &other) {
            clearStack();
            clearQueue();
            current = other.current;
            type = other.type;
            stackTop = nullptr;
            queueFront = queueBack = nullptr;

            if (type == BFS)
                copyQueue(other.queueFront);
            else
                copyStack(other.stackTop);
        }
        return *this;
    }

    bool operator!=(const AVLIterator& other) const {
        return current != other.current;
    }

    T operator*() const {
        return current->data;
    }

    AVLIterator& operator++() {
        if (type == InOrder) {
            if (stackEmpty()) {
                current = nullptr;
                return *this;
            }
            StackNode* top = stackPop();
            current = top->node;
            delete top;
            NodeAVL<T>* temp = current->right;
            while (temp) {
                stackPush(temp);
                temp = temp->left;
            }
        } else if (type == PreOrder) {
            if (stackEmpty()) {
                current = nullptr;
                return *this;
            }
            StackNode* top = stackPop();
            current = top->node;
            delete top;
            if (current->right) stackPush(current->right);
            if (current->left) stackPush(current->left);
        } else if (type == PostOrder) {
            while (!stackEmpty()) {
                StackNode* top = stackPop();
                if (top->visited) {
                    current = top->node;
                    delete top;
                    return *this;
                } else {
                    top->visited = true;
                    stackPush(top->node, true);
                    NodeAVL<T>* temp = top->node->right;
                    while (temp) {
                        stackPush(temp);
                        temp = temp->left;
                    }
                    delete top;
                }
            }
            current = nullptr;
        } else if (type == BFS) {
            if (queueEmpty()) {
                current = nullptr;
                return *this;
            }
            current = dequeue();
            if (current->left) enqueue(current->left);
            if (current->right) enqueue(current->right);
        }
        return *this;
    }

    ~AVLIterator() {
        clearStack();
        clearQueue();
    }

};

#endif
