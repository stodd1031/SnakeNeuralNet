#pragma once

template <typename T>
class MyArr
{
#define ALLOC_SIZE 100
    T *arr = nullptr;
    unsigned long _size = 0;
    unsigned long _allocated = 0;

public:
    T &operator[](const unsigned long index)
    {
        // if (index < size)
        {
            return arr[index];
        }
    }

    void operator=(const MyArr &ref)
    {
        if (ref._allocated)
        {
            _allocated = ref._allocated;
            arr = new T[_allocated];
        }
        if (ref._size)
        {
            _size = ref._size;
            for (int index = 0; index < _size; index++)
            {
                arr[index] = ref.arr[index];
            }
        }
    }

    void setSize(const unsigned long size)
    {
        if (size > _allocated)
        {
            _allocated = (size / ALLOC_SIZE + 1) * ALLOC_SIZE;
            if (arr)
            {
                T *backup = arr;
                arr = new T[_allocated];
                memcpy(arr, backup, sizeof(T) * _size);
                delete[] backup;
            }
            else
            {
                arr = new T[_allocated];
            }
        }
        _size = size;
    }

    MyArr(const unsigned long sizeInit)
    {
        _size = sizeInit;
        _allocated = (_size / ALLOC_SIZE + 1) * ALLOC_SIZE;
        arr = new T[_allocated];
    }

    MyArr(MyArr &ref)
    {
        if (ref._allocated)
        {
            _allocated = ref._allocated;
            arr = new T[_allocated];
        }
        if (ref._size)
        {
            _size = ref._size;
            for (int index = 0; index < _size; index++)
            {
                arr[index] = ref.arr[index];
            }
        }
    }

    MyArr()
    {
    }

    ~MyArr()
    {
        if (arr)
        {
            delete[] arr;
        }
    }

    unsigned long size()
    {
        return _size;
    }

    unsigned long allocated()
    {
        return _allocated;
    }

    void push_back(T item)
    {
        if (_size == _allocated)
        {
            if (arr)
            {
                T *backup = arr;
                _allocated = _allocated + ALLOC_SIZE;
                arr = new T[_allocated];
                memcpy(arr, backup, sizeof(T) * _size);
                delete[] backup;
            }
            else
            {
                _allocated = _allocated + ALLOC_SIZE;
                arr = new T[_allocated];
            }
        }
        arr[_size++] = item;
    }
};