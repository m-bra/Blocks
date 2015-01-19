#ifndef GRAPHICS_CALLBACK_HPP_INCLUDED
#define GRAPHICS_CALLBACK_HPP_INCLUDED

#ifndef PRECOMPILED_HPP_INCLUDED
#warning #include "precompiled.hpp" at the beginning of the TU!
#include "precompiled.hpp"
#endif

namespace blocks
{

class GraphicsProvider
{
public:
    fvec3 camPos, camDir, camLeft, camUp;
    virtual ~GraphicsProvider() {}
    virtual void setFrameBufSize(int w, int h) {}
    virtual bool buildChunk(ivec3_c &c) {return false;}
    virtual void render() {}
};

}

#endif//GRAPHICS_CALLBACK_HPP_INCLUDED
