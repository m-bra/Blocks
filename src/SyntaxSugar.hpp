#ifndef SYNTAX_SUGAR_HPP_INCLUDED
#define SYNTAX_SUGAR_HPP_INCLUDED

#include <functional>

// The indexer is just syntactic sugar for a function taking one argument and returning something.
// instead of using your function like
//     int bar = obj.getSomeArray()[3]
// you can do:
//     Indexer<int, int> indexer = [](int i) {return obj.getSomeArray()[i];}; // implicit conversion from function
//     int bar = indexer[3];
// make the indexer const to forbid changing the function
template <typename R, typename A>
struct Indexer
{
    std::function<R(A)> function;

    Indexer(std::function<R(A)> const &a_function) : function(a_function) {}

    R const &operator[](A &arg) const
    {
        return function(arg);
    }
};

// Properties in C++ (?)
template <typename T>
class Property
{
    T t;
public:
    using GetFunc = std::function<T const &(T const &)>;
    using SetFunc = std::function<void(T &, T const &)>;
    using MutableGetFunc = std::function<T &(T &)>;
    mutable GetFunc get;
    mutable SetFunc set;
    mutable MutableGetFunc mutableGet;

    Property() :
        get([] (T const &a_t) -> T const &{return a_t;}),
        set([] (T &a_t, T const &a_new) {a_t = a_new;}),
        mutableGet([] (T &a_t) -> T &{return a_t;})
    {}

    void operator =(T const &val)
    {
        set(t, val);
    }

    operator T() const
    {
        return get(t);
    }

    T const *operator ->() const
    {
        return &get(t);
    }

    T *operator ->()
    {
        return &mutableGet(t);
    }
};

#endif
