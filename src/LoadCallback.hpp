#ifndef LOAD_CALLBACK_HPP_INCLUDED
#define LOAD_CALLBACK_HPP_INCLUDED

namespace blocks
{

class LoadCallback
{
public:
    virtual bool doneLoading() {return true;}
};

}

#endif/*LOAD_CALLBACK_HPP_INCLUDED*/
