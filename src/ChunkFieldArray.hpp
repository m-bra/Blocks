#ifndef CHUNKFIELDARRAY_HPP_INCLUDED
#define CHUNKFIELDARRAY_HPP_INCLUDED

#include <cassert>
#include "vec.hpp"

namespace blocks
{

template <typename T>
class ChunkFieldArray
{
	T *array;
	ivec3 count;

	inline long getIndex(ivec3_c &c) const
	{
		return c.z * count.y * count.x + c.y * count.x + c.x;
	}

public:
	void create(ivec3_c &count)
	{
		this->count = count;
		array = new T[count.x * count.y * count.z];
	}

	void destroy()
	{
		delete[] array;
		count = ivec3(0, 0, 0);
	}

	bool isValidChunkCoord(ivec3_c &c) const
	{
		return c.x >= 0 && c.x < count.x
			&& c.y >= 0 && c.y < count.y
			&& c.z >= 0 && c.z < count.z;
	}

	T &operator [](ivec3_c &c)
	{
		assert(isValidChunkCoord(c));
		return array[getIndex(c)];
	}

	T const &operator [](ivec3_c &c) const
	{
		assert(isValidChunkCoord(c));
		return array[getIndex(c)];
	}

	template <typename F>
	bool iterate_cond(F const &function)
	{
		ivec3 c;
		for (c.x = 0; c.x < count.x; ++c.x)
		for (c.y = 0; c.y < count.y; ++c.y)
		for (c.z = 0; c.z < count.z; ++c.z)
		{
			if (!function(c, array[getIndex(c)]))
				return false;
		}
		return true;
	}

	template <typename F>
	bool iterate_cond(F const &function) const
	{
		ivec3 c;
		for (c.x = 0; c.x < count.x; ++c.x)
		for (c.y = 0; c.y < count.y; ++c.y)
		for (c.z = 0; c.z < count.z; ++c.z)
		{
			if (!function(c, array[getIndex(c)]))
				return false;
		}
		return true;
	}

	template <typename F>
	void iterate(F const &function)
	{
		ivec3 c;
		for (c.x = 0; c.x < count.x; ++c.x)
		for (c.y = 0; c.y < count.y; ++c.y)
		for (c.z = 0; c.z < count.z; ++c.z)
			function(c, array[getIndex(c)]);
	}

	template <typename F>
	void iterate(F const &function) const
	{
		ivec3 c;
		for (c.x = 0; c.x < count.x; ++c.x)
		for (c.y = 0; c.y < count.y; ++c.y)
		for (c.z = 0; c.z < count.z; ++c.z)
			function(c, array[getIndex(c)]);
	}

	void fill(T v = T())
	{
		for (long ci = 0; ci < count.x * count.y * count.z; ++ci)
			array[ci] = v;
	}

	void shift(ivec3 const &m)
	{
		shift(m, [](ivec3_c&){}, [](ivec3_c&){});
	}

	template <typename DestroyFunc, typename CreateFunc>
	void shift(ivec3_c &m,
		CreateFunc const &cfunc, DestroyFunc const &dfunc)
	{
		ivec3_c shift = m;
		if (shift.x == 0 && shift.y == 0 && shift.z == 0)
			return;

		// if negative, start low and go to end,
		// if positive, start big and go to beginning
		// if zero, doesn't matter...
		ivec3_c start(
			shift.x < 0 ? 0 : count.x-1,
			shift.y < 0 ? 0 : count.y-1,
			shift.z < 0 ? 0 : count.z-1);
		ivec3_c dir(
			shift.x < 0 ? 1 : -1,
			shift.y < 0 ? 1 : -1,
			shift.z < 0 ? 1 : -1);
		// the EXCLUSIVE end
		ivec3_c end = start + count*dir;

		ivec3 c;
		for (c.x = start.x; c.x != end.x; c.x+= dir.x)
		for (c.y = start.y; c.y != end.y; c.y+= dir.y)
		for (c.z = start.z; c.z != end.z; c.z+= dir.z)
		{
			// from has not been looped yet
			ivec3_c from = c - shift;
			// to has already been looped
			ivec3_c to = c + shift;

			// value will not be taken later!
			if (!isValidChunkCoord(to))
				dfunc(c);

			// if it has not a source
			if (!isValidChunkCoord(from))
			{
				array[getIndex(c)] = T();
				cfunc(c);
			}
			// else take from source
			else
				array[getIndex(c)] = array[getIndex(from)];
		}
	}
};

}

#endif /* CHUNKFIELDARRAY_HPP_ */
