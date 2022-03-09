#pragma once

namespace Uboat
{
    template <class T>
    struct Node
    {
        Node *next = nullptr;
        Node *prev = nullptr;
        T *data = nullptr;
    };

    template <class T>
    struct Pool
    {
        Node<T> *head = nullptr;
        Node<T> *tail = nullptr;

        void insert(Node<T> *node);
        void remove(Node<T> *node);
    };

    template <class T>
    void Pool<T>::insert(Node<T> *node)
    {
        if (tail)
        {
            tail->next = node;
            node->prev = tail;
            tail = node;
        }
        else
        {
            tail = head = node;
            node->prev = node->next = nullptr;
        }
    }

    template <class T>
    void Pool<T>::remove(Node<T> *node)
    {
        if (node->prev)
            node->prev->next = node->next;

        if (node->next)
            node->next->prev = node->prev;

        if (head == node)
            head = node->next;

        if (tail == node)
            tail = node->prev;

        node->next = node->prev = nullptr;
    }
}
