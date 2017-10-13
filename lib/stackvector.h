#ifndef STACKVECTOR_H
#define STACKVECTOR_H

#include <cstddef>
#include <stdexcept>

namespace parts
{

template<class Type>
class StackVector
{
public:
    StackVector(Type* buffer, size_t capacity, size_t start_offset, size_t size) :
        m_buffer(buffer),
        m_capacity(capacity),
        m_startOffset(start_offset),
        m_endOffset(start_offset + size)
    {}

    ~StackVector()
    {
        cleanup();
    }

    StackVector(const StackVector&) = delete;
    StackVector& operator=(const StackVector&) = delete;

    StackVector(StackVector&& right) :
        m_buffer(right.m_buffer),
        m_capacity(right.m_capacity),
        m_startOffset(right.m_startOffset),
        m_endOffset(right.m_endOffset)
    {
        right.cleanup();
    }

    StackVector& operator=(StackVector&& right)
    {
        m_buffer = right.m_buffer;
        m_capacity = right.m_capacity;
        m_startOffset = right.m_startOffset;
        m_endOffset = right.m_endOffset;

        right.cleanup();
    }

    typedef Type* iterator;
    typedef const Type* const_iterator;

    size_t size() const
    { return m_endOffset - m_startOffset; }
    size_t capacity() const
    { return m_capacity; }

    iterator begin()
    { return m_buffer + m_startOffset; }

    const_iterator begin() const
    { return m_buffer + m_startOffset; }

    iterator end()
    { return m_buffer + m_endOffset; }

    const iterator end() const
    { return m_buffer + m_endOffset; }

    Type& operator[](size_t pos)
    { return *(m_buffer + m_startOffset + pos); }

    const Type& operator[](size_t pos) const
    { return *(m_buffer + m_startOffset + pos); }

    void push_back(const Type& value)
    {
        checkEnd(1);
        m_buffer[m_endOffset] = value;
        ++m_endOffset;

    }
    void push_back(const Type&& value)
    {
        checkEnd(1);
        m_buffer[m_endOffset] = std::move(value);
        ++m_endOffset;
    }

    void push_front(const Type& value)
    {
        checkFront(1);
        --m_startOffset;
        m_buffer[m_startOffset] = value;
    }

    void push_front(const Type&& value)
    {
        checkFront(1);
        --m_startOffset;
        m_buffer[m_startOffset] = value;
    }

    void pop_front()
    {
        if (m_endOffset >= m_startOffset)
            throw std::runtime_error("vector is empty");
        ++m_startOffset;
    }

    void pop_back()
    {
        if (m_endOffset >= m_startOffset)
            throw std::runtime_error("vector is empty");
        --m_endOffset;
    }

    Type& front()
    { return m_buffer[m_startOffset]; }

    const Type& front() const
    { return m_buffer[m_startOffset]; }

    Type& back()
    { return m_buffer[m_endOffset - 1]; }
    const Type& back() const
    { return m_buffer[m_endOffset - 1]; }

    template<class Iterator>
    void append(Iterator begin, Iterator end)
    {
        size_t size = end - begin;
        checkEnd(size);

        for(;begin != end; ++begin)
            push_back(*begin);
    }

    template<class Iterator>
    void prepend(Iterator begin, Iterator end)
    {
        size_t size = end - begin;
        checkFront(size);

        for(end--, begin--; end != begin; --end)
            push_front(*end);
    }

protected:
    void checkEnd(size_t size)
    {
        if (m_endOffset + size > m_capacity)
            throw std::runtime_error("Inserted element is too large (at end)");
    }

    void checkFront(size_t size)
    {
        if (m_startOffset < size)
            throw std::runtime_error("Inserted element is too large (at front)");
    }

    void cleanup()
    {
        m_buffer = nullptr;
        m_startOffset = m_endOffset = m_capacity = 0;
    }

protected:
    Type* m_buffer;
    size_t m_capacity;
    size_t m_startOffset;
    size_t m_endOffset;
};

}

#endif // STACKVECTOR_H
