// circular-buffer.h

#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <cstddef> // for std::size_t
#include <algorithm> // for std::copy

template<typename T>
class CircularBuffer {
private:
    T* buffer;
    std::size_t capacity;
    std::size_t size;
    std::size_t head;
    std::size_t tail;

public:
    CircularBuffer(std::size_t capacity) : capacity(capacity), size(0), head(0), tail(0) {
        buffer = new T[capacity];
    }

    ~CircularBuffer() {
        delete[] buffer;
    }

    void put(const T& value) {
        // Add a new value
        buffer[head] = value;
        head = (head + 1) % capacity; // Move head circularly

        if (size < capacity) {
            size++;
        } else {
            tail = (tail + 1) % capacity; // Move tail circularly if buffer is full
        }
    }
    

    // get(index) returns the element at the given index, also supports negative index, index is for the last element, -1 for the second last element, and so on
    const T& get(int index) const {
        if (size == 0) {
            throw "Buffer is empty";
        }
        if (index >= (int)size || index < -(int)size) {
            throw "Index out of range";
        }
        if (index < 0) {
            index += (int)size;
        }
        return buffer[(tail + index) % capacity];
    }

    T* toArray() const {
        T* array = new T[size];
        if (size > 0) {
            std::size_t current = tail;
            for (std::size_t i = 0; i < size; ++i) {
                array[i] = buffer[current];
                current = (current + 1) % capacity;
            }
        }
        return array;
    }

	std::size_t getSize() const {
		return size;
	}

    const T& getLast() const {
        if (size == 0) {
            throw "Buffer is empty";
        }
        return buffer[tail];
    }
};

#endif // CIRCULAR_BUFFER_H
