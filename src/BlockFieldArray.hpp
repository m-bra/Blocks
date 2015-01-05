#ifndef BLOCK_FIELD_ARRAY_HPP_INCLUDED
#define BLOCK_FIELD_ARRAY_HPP_INCLUDED

#include <cassert>
#include "vec.hpp"

namespace blocks
{

template <typename T>
class BlockFieldArray
{
	T **array;
	ivec3 count, size;

	inline long getChunkIndex(ivec3_c &c)
	{
		return c.z * count.y * count.x + c.y * count.x + c.x;
	}

	inline long getBlockIndex(ivec3_c &b)
	{
		return b.z * size.y * size.x + b.y * size.x + b.x;
	}
public:

	void create(ivec3_c &a_count, ivec3_c &a_size)
	{
		count = a_count;
		size = a_size;

		array = new T *[count.x * count.y * count.z];

		for (long ci = 0; ci < count.x * count.y * count.z; ++ci)
			array[ci] = new T[size.x * size.y * size.z];
	}

	void destroy()
	{
		for (long ci = 0; ci < count.x * count.y * count.z; ++ci)
			delete[] array[ci];

		delete[] array;

		count = ivec3(0, 0, 0);
		size = ivec3(0, 0, 0);
	}

	bool isValidChunkCoord(ivec3_c &c) const
	{
		return c.x >= 0 && c.x < count.x
			&& c.y >= 0 && c.y < count.y
			&& c.z >= 0 && c.z < count.z;
	}

	bool isValidBlockInChunkCoord(ivec3_c &b) const
	{
		return b.x >= 0 && b.x < size.x
			&& b.y >= 0 && b.y < size.y
			&& b.z >= 0 && b.z < size.z;
	}

	bool isValidBlockCoord(ivec3_c &b) const
	{
		return b.x >= 0 && b.x < count.x * size.x
			&& b.y >= 0 && b.y < count.y * size.y
			&& b.z >= 0 && b.z < count.z * size.z;
	}

	T &blockInChunk(ivec3_c &c, ivec3_c &b)
	{
		assert(isValidChunkCoord(c));
		assert(isValidBlockInChunkCoord(b));
		return array[getChunkIndex(c)][getBlockIndex(b)];
	}

	T const &blockInChunk(ivec3_c &c, ivec3_c &b) const
	{
		assert(isValidChunkCoord(c));
		assert(isValidBlockInChunkCoord(b));
		return array[getChunkIndex(c)][getBlockIndex(b)];
	}

	T &blockAt (ivec3_c &b)
	{
		assert(isValidBlockCoord(b));
		return array[getChunkIndex(b / size)]
		            [getBlockIndex(b % size)];
	}

	T const &blockAt(ivec3_c &b) const
	{
		assert(isValidBlockCoord(b));
		return array[getChunkIndex(b / size)]
					[getBlockIndex(b % size)];
	}

	template <typename F>
	void iterateInChunk(ivec3_c &c, F const &function)
	{
		ivec3_c c_b = c * size;

		ivec3 b;
		for (b.x = 0; b.x < size.x; ++b.x)
		for (b.y = 0; b.y < size.y; ++b.y)
		for (b.z = 0; b.z < size.z; ++b.z)
			function(c_b + b, blockInChunk(c, b));
	}

	template <typename F>
	void iterateInChunk(ivec3_c &c, F const &function) const
	{
		ivec3_c c_b = c * size;

		ivec3 b;
		for (b.x = 0; b.x < size.x; ++b.x)
		for (b.y = 0; b.y < size.y; ++b.y)
		for (b.z = 0; b.z < size.z; ++b.z)
			function(c_b + b, blockInChunk(c, b));
	}

	template <typename F>
	bool iterateInChunk_cond(ivec3_c &c, F const &function)
	{
		ivec3_c c_b = c * size;

		ivec3 b;
		for (b.x = 0; b.x < size.x; ++b.x)
		for (b.y = 0; b.y < size.y; ++b.y)
		for (b.z = 0; b.z < size.z; ++b.z)
		{
			if (!function(c_b + b, blockInChunk(c, b)))
				return false;
		}
		return true;
	}

	template <typename F>
	bool iterateInChunk_cond(ivec3_c &c, F const &function) const
	{
		ivec3_c c_b = c * size;

		ivec3 b;
		for (b.x = 0; b.x < size.x; ++b.x)
		for (b.y = 0; b.y < size.y; ++b.y)
		for (b.z = 0; b.z < size.z; ++b.z)
		{
			if (!function(c_b + b, blockInChunk(c, b)))
				return false;
		}
		return true;
	}

	template <typename F>
	void iterate(F const &function)
	{
		ivec3 c, c_b, b;
		// chunk coordinates suffixed with _b are measured in BLOCKS!
		for (c.x = 0, c_b.x = 0; c.x < count.x; ++c.x, c_b.x+= size.x)
		for (c.y = 0, c_b.y = 0; c.y < count.y; ++c.y, c_b.y+= size.y)
		for (c.z = 0, c_b.z = 0; c.z < count.z; ++c.z, c_b.z+= size.z)
		{
			for (b.x = 0; b.x < size.x; ++b.x)
			for (b.y = 0; b.y < size.y; ++b.y)
			for (b.z = 0; b.z < size.z; ++b.z)
				function(c, c_b + b, blockInChunk(c, b));
		}
	}

	template <typename F>
	void iterate(F const &function) const
	{
		ivec3 c, c_b, b;
		// chunk coordinates suffixed with _b are measured in BLOCKS!
		for (c.x = 0, c_b.x = 0; c.x < count.x; ++c.x, c_b.x+= size.x)
		for (c.y = 0, c_b.y = 0; c.y < count.y; ++c.y, c_b.y+= size.y)
		for (c.z = 0, c_b.z = 0; c.z < count.z; ++c.z, c_b.z+= size.z)
		{
			for (b.x = 0; b.x < size.x; ++b.x)
			for (b.y = 0; b.y < size.y; ++b.y)
			for (b.z = 0; b.z < size.z; ++b.z)
				function(c, c_b + b, blockInChunk(c, b));
		}
	}

	template <typename F>
	bool iterate_cond(F const &function)
	{
		ivec3 c, c_b, b;
		// chunk coordinates suffixed with _b are measured in BLOCKS!
		for (c.x = 0, c_b.x = 0; c.x < count.x; ++c.x, c_b.x+= size.x)
		for (c.y = 0, c_b.y = 0; c.y < count.y; ++c.y, c_b.y+= size.y)
		for (c.z = 0, c_b.z = 0; c.z < count.z; ++c.z, c_b.z+= size.z)
		{
			for (b.x = 0; b.x < size.x; ++b.x)
			for (b.y = 0; b.y < size.y; ++b.y)
			for (b.z = 0; b.z < size.z; ++b.z)
			{
				if (!function(c, c_b + b, blockInChunk(c, b)))
					return false;
			}
		}
		return true;
	}

	template <typename F>
	bool iterate_cond(F const &function) const
	{
		ivec3 c, c_b, b;
		// chunk coordinates suffixed with _b are measured in BLOCKS!
		for (c.x = 0, c_b.x = 0; c.x < count.x; ++c.x, c_b.x+= size.x)
		for (c.y = 0, c_b.y = 0; c.y < count.y; ++c.y, c_b.y+= size.y)
		for (c.z = 0, c_b.z = 0; c.z < count.z; ++c.z, c_b.z+= size.z)
		{
			for (b.x = 0; b.x < size.x; ++b.x)
			for (b.y = 0; b.y < size.y; ++b.y)
			for (b.z = 0; b.z < size.z; ++b.z)
			{
				if (!function(c, c_b + b, blockInChunk(c, b)))
					return false;
			}
		}
		return true;
	}

	void fill(T v = T())
	{
		for (long ci = 0; ci < count.x * count.y * count.z; ++ci)
		{
			for (long bi = 0; bi < size.x * size.y * size.z; ++bi)
			{
				array[ci][bi] = v;
			}
		}
	}

	void shift(ivec3 const &m)
	{
		shift(m, [](ivec3_c&){}, [](ivec3_c&){});
	}

	template <typename DestroyFunc, typename CreateFunc>
	void shift(ivec3 const &m,
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
				shift.x < 0   ? 1 : -1,
				shift.y < 0   ? 1 : -1,
				shift.z < 0   ? 1 : -1);
		// the EXCLUSIVE end
		ivec3_c end = start + count * dir;

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
			{
				dfunc(c);
				delete[] array[getChunkIndex(c)];
			}

			// if it has not a source
			if (!isValidChunkCoord(from))
			{
				array[getChunkIndex(c)] = new T[size.x * size.y * size.z]();
				cfunc(c);
			}
			// else take from source
			else
				array[getChunkIndex(c)] = array[getChunkIndex(from)];
		}
	}
};

}

#endif /* FIELDARRAYS_HPP_ */
