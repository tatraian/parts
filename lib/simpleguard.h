#ifndef SIMPLEGUARD_H
#define SIMPLEGUARD_H

namespace parts
{

template<class Functor>
struct SimpleGuard {
public:
    SimpleGuard(Functor fn) : m_destructor(fn) {}
    ~SimpleGuard() { m_destructor(); }

    Functor m_destructor;
};

}

#endif // SIMPLEGUARD_H
