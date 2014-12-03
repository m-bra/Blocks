/*
 * FieldArrays.hpp
 *
 *  Created on: Sep 14, 2014
 *      Author: merlin
 */

#ifndef FIELDARRAYS_HPP_
#define FIELDARRAYS_HPP_

#include <cassert>
#include "vec.hpp"

namespace blocks
{

template <int CCX, int CCY, int CCZ, int CSX, int CSY, int CSZ>
struct BlockFieldArraySize
{
	static constexpr int CCOUNT_X = CCX;
	static constexpr int CCOUNT_Y = CCY;
	static constexpr int CCOUNT_Z = CCZ;
	static constexpr ivec3 CCOUNT = ivec3(CCX, CCY, CCZ);
	static constexpr int CSIZE_X = CSX;
	static constexpr int CSIZE_Y = CSY;
	static constexpr int CSIZE_Z = CSZ;
	static constexpr ivec3 CSIZE = ivec3(CSX, CSY, CSZ);
};

template <typename BlockFieldArraySize, typename Field>
class BlockFieldArray
{
public:
	static constexpr auto CCOUNT_X = BlockFieldArraySize::CCOUNT_X;
	static constexpr auto CCOUNT_Y = BlockFieldArraySize::CCOUNT_Y;
	static constexpr auto CCOUNT_Z = BlockFieldArraySize::CCOUNT_Z;
	static constexpr auto CSIZE_X = BlockFieldArraySize::CSIZE_X;
	static constexpr auto CSIZE_Y = BlockFieldArraySize::CSIZE_Y;
	static constexpr auto CSIZE_Z = BlockFieldArraySize::CSIZE_Z;
private:
	Field (*array[CCOUNT_X][CCOUNT_Y][CCOUNT_Z])[CSIZE_X][CSIZE_Y][CSIZE_Z];

public:

	BlockFieldArray()
	{
		for (int cx = 0; cx < CCOUNT_X; ++cx)
		for (int cy = 0; cy < CCOUNT_Y; ++cy)
		for (int cz = 0; cz < CCOUNT_Z; ++cz)
		{
			array[cx][cy][cz] =
				(Field (*)[CSIZE_X][CSIZE_Y][CSIZE_Z]) new Field[CSIZE_X * CSIZE_Y * CSIZE_Z];
		}
	}

	~BlockFieldArray()
	{
		for (int cx = 0; cx < CCOUNT_X; ++cx)
		for (int cy = 0; cy < CCOUNT_Y; ++cy)
		for (int cz = 0; cz < CCOUNT_Z; ++cz)
		{
			delete[] (Field *) array[cx][cy][cz];
		}
	}

	constexpr bool isValidChunkCoord(ivec3 const &c) const
	{
		return c.x >= 0 && c.x < CCOUNT_X
			&& c.y >= 0 && c.y < CCOUNT_Y
			&& c.z >= 0 && c.z < CCOUNT_Z;
	}

	constexpr bool isValidBlockInChunkCoord(ivec3 const &b) const
	{
		return b.x >= 0 && b.x < CSIZE_X
			&& b.y >= 0 && b.y < CSIZE_Y
			&& b.z >= 0 && b.z < CSIZE_Z;
	}

	constexpr bool isValidBlockCoord(ivec3 const &b) const
	{
		return b.x >= 0 && b.x < CCOUNT_X * CSIZE_X
			&& b.y >= 0 && b.y < CCOUNT_Y * CSIZE_Y
			&& b.z >= 0 && b.z < CCOUNT_Z * CSIZE_Z;
	}

	Field &blockInChunk(ivec3 const &c, ivec3 const &b)
	{
		assert(isValidChunkCoord(c));
		assert(isValidBlockInChunkCoord(b));
		return (*array[c.x][c.y][c.z])[b.x][b.y][b.z];
	}

	Field const &blockInChunk(ivec3 const &c, ivec3 const &b) const
	{
		assert(isValidChunkCoord(c));
		assert(isValidBlockInChunkCoord(b));
		return (*array[c.x][c.y][c.z])[b.x][b.y][b.z];
	}

	Field &blockAt(ivec3 const &b)
	{
		assert(isValidBlockCoord(b));
		return (*array[b.x / CSIZE_X][b.y / CSIZE_Y][b.z / CSIZE_Z])
		            [b.x % CSIZE_X][b.y % CSIZE_Y][b.z % CSIZE_Z];
	}

	Field const &blockAt(ivec3 const &b) const
	{
		assert(isValidBlockCoord(b));
		return (*array[b.x / CSIZE_X][b.y / CSIZE_Y][b.z / CSIZE_Z])
		            [b.x % CSIZE_X][b.y % CSIZE_Y][b.z % CSIZE_Z];
	}

	template <typename F>
	bool iterateInChunk(ivec3 const &c, F const &function)
	{
		ivec3 const c_b(c.x * CSIZE_X, c.y * CSIZE_Z, c.z * CSIZE_Z);

		ivec3 b;
		for (b.x = 0; b.x < CSIZE_X; ++b.x)
		for (b.y = 0; b.y < CSIZE_Y; ++b.y)
		for (b.z = 0; b.z < CSIZE_Z; ++b.z)
		{
			if (!function(c_b + b, blockInChunk(c, b)))
				return false;
		}
		return true;
	}

	template <typename F>
	bool iterateInChunk(ivec3 const &c, F const &function) const
	{
		ivec3 const c_b(c.x * CSIZE_X, c.y * CSIZE_Z, c.z * CSIZE_Z);

		ivec3 b;
		for (b.x = 0; b.x < CSIZE_X; ++b.x)
		for (b.y = 0; b.y < CSIZE_Y; ++b.y)
		for (b.z = 0; b.z < CSIZE_Z; ++b.z)
		{
			if (!function(c_b + b, blockInChunk(c, b)))
				return false;
		}
		return true;
	}

	template <typename F>
	bool iterate(F const &function)
	{
		ivec3 c, c_b, b;
		// chunk coordinates suffixed with _b are measured in BLOCKS!
		for (c.x = 0, c_b.x = 0; c.x < CCOUNT_X; ++c.x, c_b.x+= CSIZE_X)
		for (c.y = 0, c_b.y = 0; c.y < CCOUNT_Y; ++c.y, c_b.y+= CSIZE_Y)
		for (c.z = 0, c_b.z = 0; c.z < CCOUNT_Z; ++c.z, c_b.z+= CSIZE_Z)
		{
			for (b.x = 0; b.x < CSIZE_X; ++b.x)
			for (b.y = 0; b.y < CSIZE_Y; ++b.y)
			for (b.z = 0; b.z < CSIZE_Z; ++b.z)
			{
				if (!function(c, c_b + b, blockInChunk(c, b)))
					return false;
			}
		}
		return true;
	}

	template <typename F>
	bool iterate(F const &function) const
	{
		ivec3 c, c_b, b;
		// chunk coordinates suffixed with _b are measured in BLOCKS!
		for (c.x = 0, c_b.x = 0; c.x < CCOUNT_X; ++c.x, c_b.x+= CSIZE_X)
		for (c.y = 0, c_b.y = 0; c.y < CCOUNT_Y; ++c.y, c_b.y+= CSIZE_Y)
		for (c.z = 0, c_b.z = 0; c.z < CCOUNT_Z; ++c.z, c_b.z+= CSIZE_Z)
		{
			for (b.x = 0; b.x < CSIZE_X; ++b.x)
			for (b.y = 0; b.y < CSIZE_Y; ++b.y)
			for (b.z = 0; b.z < CSIZE_Z; ++b.z)
			{
				if (!function(c, c_b + b, blockInChunk(c, b)))
					return false;
			}
		}
		return true;
	}

	void fill(Field f = Field())
	{
		for (int cx = 0, cx_b = 0; cx < CCOUNT_X; ++cx, cx_b+= CSIZE_X)
		for (int cy = 0, cy_b = 0; cy < CCOUNT_Y; ++cy, cy_b+= CSIZE_Y)
		for (int cz = 0, cz_b = 0; cz < CCOUNT_Z; ++cz, cz_b+= CSIZE_Z)
		{
			for (int bx = 0; bx < CSIZE_X; ++bx)
			for (int by = 0; by < CSIZE_Y; ++by)
			for (int bz = 0; bz < CSIZE_Z; ++bz)
			{
				(*array[cx][cy][cz])[bx][by][bz] = f;
			}
		}
	}

	void shift(ivec3 const &m)
	{
		shift(m, [](int,int,int){}, [](int,int,int){});
	}

	template <typename DestroyFunc, typename CreateFunc>
	void shift(ivec3 const &m,
		CreateFunc const &cfunc = [](int x, int y, int z){}, DestroyFunc const &dfunc = [](int x,int y,int z){})
	{
		int const &shiftX = m.x;
		int const &shiftY = m.y;
		int const &shiftZ = m.z;
		if (shiftX == 0 && shiftY == 0 && shiftZ == 0)
			return;

		// if negative, start low and go to end,
		// if positive, start big and go to beginning
		// if zero, doesn't matter...
		int const startX = shiftX < 0 ? 0 : CCOUNT_X-1;
		int const startY = shiftY < 0 ? 0 : CCOUNT_Y-1;
		int const startZ = shiftZ < 0 ? 0 : CCOUNT_Z-1;
		int const dirX = shiftX < 0   ? 1 : -1;
		int const dirY = shiftY < 0   ? 1 : -1;
		int const dirZ = shiftZ < 0   ? 1 : -1;
		// the EXCLUSIVE end
		int const endX = startX + CCOUNT_X*dirX;
		int const endY = startY + CCOUNT_Y*dirY;
		int const endZ = startZ + CCOUNT_Z*dirZ;

		for (int cx = startX; cx != endX; cx+= dirX)
		for (int cy = startY; cy != endY; cy+= dirY)
		for (int cz = startZ; cz != endZ; cz+= dirZ)
		{
			// from* has not been looped yet
			int const fromCx = cx - shiftX;
			int const fromCy = cy - shiftY;
			int const fromCz = cz - shiftZ;
			// to* has already been looped
			int const toCx = cx + shiftX;
			int const toCy = cy + shiftY;
			int const toCz = cz + shiftZ;

			// value will not be taken later!
			if (!isValidChunkCoord(ivec3(toCx, toCy, toCz)))
			{
				dfunc(cx, cy, cz);
				delete[] (Field *) array[cx][cy][cz];
			}

			// if it has not a source
			if (!isValidChunkCoord(ivec3(fromCx, fromCy, fromCz)))
			{
				array[cx][cy][cz] =
					(Field (*)[CSIZE_X][CSIZE_Y][CSIZE_Z]) new Field[CSIZE_X * CSIZE_Y * CSIZE_Z]();
				cfunc(cx, cy, cz);
			}
			// else take from source
			else
				array[cx][cy][cz] = array[fromCx][fromCy][fromCz];
		}
	}
};

}

#endif /* FIELDARRAYS_HPP_ */
