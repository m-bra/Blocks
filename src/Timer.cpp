#include "Timer.hpp"

void Timer::update(float delta)
{
    time+= delta;
    for (auto entry = list.begin(); entry != list.end(); ++entry)
        if (time >= entry->next)
        {
            entry->function();
            entry->next = time + entry->interval;
        }
}
