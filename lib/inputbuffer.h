#ifndef INPUTBUFFER_H
#define INPUTBUFFER_H

#include <vector>

namespace parts {

class InputBuffer
{
public:
    typedef std::vector<uint8_t>::iterator iterator;
    typedef std::vector<uint8_t>::const_iterator const_iterator;

    typedef std::vector<uint8_t>::reference reference;
    typedef std::vector<uint8_t>::const_reference const_reference;

    InputBuffer() : m_start(0)
    {}

    InputBuffer(std::initializer_list<uint8_t> values) : m_buffer(values), m_start(0)
    {}

    InputBuffer(const InputBuffer& rhs) = default;
    InputBuffer& operator=(const InputBuffer& rhs) = default;

    InputBuffer(InputBuffer&& rhs) = default;
    InputBuffer& operator=(InputBuffer&& rhs) = default;

    ~InputBuffer() = default;

    iterator begin()
    { return m_buffer.begin() + m_start; }

    const_iterator begin() const
    { return m_buffer.begin() + m_start; }

    iterator end()
    { return m_buffer.end(); }

    const_iterator end() const
    { return m_buffer.end(); }

    reference front()
    { return m_buffer[m_start]; }

    const_reference front() const
    { return m_buffer[m_start]; }

    reference back()
    { return m_buffer.back(); }

    const_reference back() const
    { return m_buffer.back(); }

    size_t size() const {
        return m_buffer.size() - m_start;
    }

    bool empty() const {
        return size() == 0;
    }

    template<class T>
    iterator insert(iterator pos, T first, T last) {
        return m_buffer.insert(pos, first, last);
    }

    iterator erase(iterator first, iterator last) {
        if (first == begin() + m_start)
            m_start += last-first;
        else
            m_buffer.erase(first, last);

        return last;
    }

protected:
    std::vector<uint8_t> m_buffer;

    size_t m_start;
};

}
#endif // INPUTBUFFER_H
