#ifndef CHUNKFIELDARRAY_HPP_
#define CHUNKFIELDARRAY_HPP_

#include <cassert>
#include "vec.hpp"

template <int CCX, int CCY, int CCZ>
struct ChunkFieldArraySize
{
	static constexpr int CCOUNT_X = CCX;
	static constexpr int CCOUNT_Y = CCY;
	static constexpr int CCOUNT_Z = CCZ;
	static constexpr ivec3 CCOUNT = ivec3(CCX, CCY, CCZ);
};

template <typename ChunkFieldArraySize, typename Field>
class ChunkFieldArray
{
public:
	static auto constexpr CCOUNT_X = ChunkFieldArraySize::CCOUNT_X;
	static auto constexpr CCOUNT_Y = ChunkFieldArraySize::CCOUNT_Y;
	static auto constexpr CCOUNT_Z = ChunkFieldArraySize::CCOUNT_Z;
private:
	Field array[CCOUNT_X][CCOUNT_Y][CCOUNT_Z];

public:
	constexpr bool isValidChunkCoord(ivec3 const &c) const
	{
		return c.x >= 0 && c.x < CCOUNT_X
			&& c.y >= 0 && c.y < CCOUNT_Y
			&& c.z >= 0 && c.z < CCOUNT_Z;
	}

	Field &chunkAt(ivec3 const &c)
	{
		assert(isValidChunkCoord(c));
		return array[c.x][c.y][c.z];
	}

	Field const &chunkAt(ivec3 const &c) const
	{
		assert(isValidChunkCoord(c));
		return array[c.x][c.y][c.z];
	}

	template <typename F>
	bool iterate(F const &function)
	{
		ivec3 c;
		for (c.x = 0; c.x < CCOUNT_X; ++c.x)
		for (c.y = 0; c.y < CCOUNT_Y; ++c.y)
		for (c.z = 0; c.z < CCOUNT_Z; ++c.z)
		{
			if (!function(c, chunkAt(c)))
				return false;
		}
		return true;
	}

	template <typename F>
	bool iterate(F const &function) const
	{
		ivec3 c;
		for (c.x = 0; c.x < CCOUNT_X; ++c.x)
		for (c.y = 0; c.y < CCOUNT_Y; ++c.y)
		for (c.z = 0; c.z < CCOUNT_Z; ++c.z)
		{
			if (!function(c, chunkAt(c)))
				return false;
		}
		return true;
	}

	void fill(Field f = Field())
	{
		for (int cx = 0; cx < CCOUNT_X; ++cx)
		for (int cy = 0; cy < CCOUNT_Y; ++cy)
		for (int cz = 0; cz < CCOUNT_Z; ++cz)
			array[cx][cy][cz] = f;
	}

	void shift(ivec3 const &m)
	{
		shift(m, [](int,int,int){}, [](int,int,int){});
	}

	template <typename DestroyFunc, typename CreateFunc>
	void shift(ivec3 const &m,
		CreateFunc const &cfunc, DestroyFunc const &dfunc)
	{
#warning Please optimize algo to ivec3...
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
				dfunc(ivec3(cx, cy, cz));

			// if it has not a source
			if (!isValidChunkCoord(ivec3(fromCx, fromCy, fromCz)))
			{
				array[cx][cy][cz] = Field();
				cfunc(ivec3(cx, cy, cz));
			}
			// else take from source
			else
				array[cx][cy][cz] = array[fromCx][fromCy][fromCz];
		}
	}
};

#endif /* CHUNKFIELDARRAY_HPP_ */
