#include <iostream>
#include <vector>
using namespace std;

const int maxColision = 3;

template <typename TK, typename TV>
class HashTable;

namespace std
{
    inline std::string to_string(const std::string &s)
    {
        return s;
    }
}

// itera sobre el hashtable manteniendo el orden de insercion
template <typename TK, typename TV>
class HashIterator
{
private:
    // TODO
    typename HashTable<TK, TV>::NodeHT *current;

public:
    HashIterator(typename HashTable<TK, TV>::NodeHT *ptr) : current(ptr) {}

    HashIterator<TK, TV> &operator=(HashIterator<TK, TV> other)
    {
        this->current = other.current;
        return *this;
    }

    bool operator!=(HashIterator<TK, TV> other)
    {
        return this->current != other.current;
    }

    HashIterator<TK, TV> &operator++()
    { //++it
        if (current)
            current = current->nextOrdered;
        return *this;
    }

    pair<TK, TV> operator*()
    {
        return {current->key, current->value};
    }
};

template <typename TK, typename TV>
class HashTable
{
public:
    typedef HashIterator<TK, TV> iterator;

    iterator begin() { return iterator(headOrdered); } // Retorna el inicio del iterador
    iterator end() { return iterator(nullptr); }       // Retorna el final del iterador

    struct NodeHT
    {
        TK key;
        TV value;
        NodeHT *nextBucket;
        NodeHT *prevOrdered;
        NodeHT *nextOrdered;

        NodeHT(const TK &k, const TV &v)
            : key(k), value(v),
              nextBucket(nullptr),
              prevOrdered(nullptr),
              nextOrdered(nullptr) {}
    };

private:
    int capacity; // capacidad del hash table
    int size;     // total de elementos
    // TODO: completar los demas atributo
    NodeHT **buckets;
    NodeHT *headOrdered;
    NodeHT *tailOrdered;

    int hash(TK key)
    {
        return std::hash<TK>()(key) % capacity;
    }

public:
    HashTable(int _cap = 5) : capacity(_cap), size(0)
    {
        // TODO
        buckets = new NodeHT *[capacity];
        for (int i = 0; i < capacity; ++i)
        {
            buckets[i] = nullptr;
        }
        headOrdered = nullptr;
        tailOrdered = nullptr;
    }

    void insert(TK key, TV value)
    {
        insert({key, value});
    }

    void insert(pair<TK, TV> item)
    {
        int idx = hash(item.first);
        NodeHT *current = buckets[idx];
        int count = 0;

        while (current)
        {
            if (current->key == item.first)
            {
                current->value = item.second;
                return;
            }
            current = current->nextBucket;
            count++;
        }

        if (count >= maxColision)
        {
            rehashing();
            insert(item);
            return;
        }

        NodeHT *newNode = new NodeHT(item.first, item.second);
        newNode->nextBucket = buckets[idx];
        buckets[idx] = newNode;

        if (!headOrdered)
        {
            headOrdered = tailOrdered = newNode;
        }
        else
        {
            tailOrdered->nextOrdered = newNode;
            newNode->prevOrdered = tailOrdered;
            tailOrdered = newNode;
        }

        size++;
    }

    TV &at(TK key)
    {
        int idx = hash(key);
        NodeHT *current = buckets[idx];
        while (current)
        {
            if (current->key == key)
                return current->value;
            current = current->nextBucket;
        }
        throw out_of_range("Key not found");
    }

    TV &operator[](TK key)
    {
        int idx = hash(key);
        NodeHT *current = buckets[idx];

        while (current)
        {
            if (current->key == key)
                return current->value;
            current = current->nextBucket;
        }

        insert(key, TV());
        return at(key);
    }

    bool find(TK key)
    {
        int idx = hash(key);
        NodeHT *current = buckets[idx];
        while (current)
        {
            if (current->key == key)
                return true;
            current = current->nextBucket;
        }
        return false;
    }

    bool remove(TK key)
    {
        int idx = hash(key);
        NodeHT *current = buckets[idx];
        NodeHT *prev = nullptr;

        while (current)
        {
            if (current->key == key)
            {
                if (prev)
                    prev->nextBucket = current->nextBucket;
                else
                    buckets[idx] = current->nextBucket;

                if (current->prevOrdered)
                    current->prevOrdered->nextOrdered = current->nextOrdered;
                else
                    headOrdered = current->nextOrdered;

                if (current->nextOrdered)
                    current->nextOrdered->prevOrdered = current->prevOrdered;
                else
                    tailOrdered = current->prevOrdered;

                delete current;
                size--;
                return true;
            }
            prev = current;
            current = current->nextBucket;
        }
        return false;
    }

    int getSize()
    {
        return size;
    }

    /*itera sobre el hashtable manteniendo el orden de insercion*/
    vector<TK> getAllKeys()
    {
        vector<TK> keys;
        NodeHT *current = headOrdered;
        while (current)
        {
            keys.push_back(current->key);
            current = current->nextOrdered;
        }
        return keys;
    }

    vector<pair<TK, TV>> getAllElements()
    {
        vector<pair<TK, TV>> elements;
        NodeHT *current = headOrdered;
        while (current)
        {
            elements.emplace_back(current->key, current->value);
            current = current->nextOrdered;
        }
        return elements;
    }

private:
    /*Si una lista colisionada excede maxColision, redimensionar el array*/
    void rehashing()
    {
        int oldCapacity = capacity;
        capacity *= 2;
        NodeHT **oldBuckets = buckets;
        buckets = new NodeHT *[capacity];

        for (int i = 0; i < capacity; ++i)
            buckets[i] = nullptr;

        NodeHT *current = headOrdered;
        headOrdered = nullptr;
        tailOrdered = nullptr;
        size = 0;

        while (current)
        {
            NodeHT *next = current->nextOrdered;
            current->nextBucket = nullptr;
            current->nextOrdered = nullptr;
            current->prevOrdered = nullptr;

            int idx = hash(current->key);
            current->nextBucket = buckets[idx];
            buckets[idx] = current;

            if (!headOrdered)
            {
                headOrdered = tailOrdered = current;
            }
            else
            {
                tailOrdered->nextOrdered = current;
                current->prevOrdered = tailOrdered;
                tailOrdered = current;
            }

            size++;
            current = next;
        }

        delete[] oldBuckets;
    }
};
