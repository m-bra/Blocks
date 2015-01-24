#ifndef TIMER_HPP_INCLUDED
#define TIMER_HPP_INCLUDED

#include <functional>
#include <vector>

class Timer
{
private:
    struct Entry
    {
        float offset, interval, next;
        std::function<void()> function;
    };
    std::vector<Entry> list;

    float time = 0;

public:
    void add(std::function<void()> const &f, float interval, float off = 0)
    {
        list.push_back({off, interval, off + interval, f});
    }

    void update(float delta);
};

#endif//TIMER_HPP_INCLUDED
