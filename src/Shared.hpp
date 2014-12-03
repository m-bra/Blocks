#ifndef WORLD_HPP_
#define WORLD_HPP_

#include <iostream>
#include <atomic>
#include <mutex>
#include <glm/glm.hpp>

#include "vec.hpp"
#include "EntityFieldArray.hpp"
#include "ChunkFieldArray.hpp"
#include "BlockFieldArray.hpp"

#include "logic/Module.hpp"
#include "graphics/Module.hpp"
#include "physics/Module.hpp"

#include "SharedTypes.hpp"

namespace blocks
{

class Shared
{
public:
	static int constexpr CCOUNT_X = 6;
	static int constexpr CCOUNT_Y = 2;
	static int constexpr CCOUNT_Z = CCOUNT_X;
	static int constexpr CSIZE_X = 32;
	static int constexpr CSIZE_Y = 32;
	static int constexpr CSIZE_Z = CSIZE_X;
	static ivec3 constexpr CCOUNT = ivec3(CCOUNT_X, CCOUNT_Y, CCOUNT_Z);
	static ivec3 constexpr CSIZE = ivec3(CSIZE_X, CSIZE_Y, CSIZE_Z);

	using BlockFieldArraySize = ::blocks::BlockFieldArraySize<CCOUNT_X, CCOUNT_Y, CCOUNT_Z,
													  CSIZE_X, CSIZE_Y, CSIZE_Z>;
	using ChunkFieldArraySize = ::blocks::ChunkFieldArraySize<CCOUNT_X, CCOUNT_Y, CCOUNT_Z>;

	template <typename Field>
	using BlockFieldArray = ::blocks::BlockFieldArray<BlockFieldArraySize, Field>;
	template <typename Field>
	using ChunkFieldArray = ::blocks::ChunkFieldArray<ChunkFieldArraySize, Field>;
	template <typename Field>
	using EntityFieldArray = ::blocks::EntityFieldArray<Field>;

	std::mutex moveLock;
	struct
	{
		std::mutex _mutex[CCOUNT_X][CCOUNT_Y][CCOUNT_Z];

		void lock(ivec3_c &c)
		{
			_mutex[c.x][c.y][c.z].lock();
		}

		bool try_lock(ivec3_c &c)
		{
			return _mutex[c.x][c.y][c.z].try_lock();
		}

		void unlock(ivec3_c &c)
		{
			_mutex[c.x][c.y][c.z].unlock();
		}
	} blockWriteLock;
	ivec3 pos, nextMove;

	EntityFieldArray<EntityType> entityTypes;
	BlockFieldArray<BlockType> blockTypes;

	physics::Module<Shared> physics;
	logic::Module<Shared> logic;
	graphics::Module<Shared> graphics;

	Time gameTime;
	bool loading = true;

	fvec3 camPos, camDir, camLeft, camUp;
	int playerEntity;

	Shared(float aspect) : physics(this), logic(this), graphics(this, aspect)
	{
		blockTypes.fill(BlockType::NONE);
	}

	Time secondsToWorldTime(float seconds)
	{
		return seconds;
	}

	void setBlockType(ivec3::cref b, BlockType type);

	bool onGround();

	int createEntity(EntityType aType, fvec3 p, logic::EntityLogics data = logic::EntityLogics());
	void destroyEntity(int e);

	bool getSelectedBlock(ivec3 &b1, ivec3 &b2)
	{
		return physics.getSelectedBlock(camPos, camPos + camDir * logic.reach, b1, b2);
	}

	int getSelectedEntity()
	{
		return physics.getSelectedEntity(camPos, camPos + camDir * logic.reach);
	}

	bool isEntityCreated(int e)
	{
		return physics.entityPhysics[e].created
			&& graphics.entityGraphics[e].created
			&& logic.entityLogics[e].created;
	}

	void resizeEntityArrays()
	{
		int const size = entityTypes.getCount();
		int const newSize = 10 + size * 2;

#ifdef RESIZE
#error ALREADY DEFINED RESIZE
#endif
#define RESIZE(X) \
			assert(size == X.getCount()); \
			X.resize(newSize);

		RESIZE(physics.entityPhysics);
		RESIZE(physics.entityStartPos);
		RESIZE(graphics.entityGraphics);
		RESIZE(logic.entityLogics);
		RESIZE(entityTypes);
#undef RESIZE
	}

	void tryMove(ivec3_c &m)
	{
		if (!loading && graphics.canMove() && physics.canMove())
			if (moveLock.try_lock())
			{
				pos+= m * CSIZE;

				blockTypes.shift(-m);
				physics.move(m);
				logic.move(m);
				graphics.move(m);
				moveLock.unlock();
			}
	}

	void update(Time time)
	{
		fvec3 ppos = physics.playerBody->getWorldTransform().getOrigin();
		int mx = CCOUNT_X/2 - ppos.x / CSIZE_X;
		int my = 0;
		int mz = CCOUNT_Z/2 - ppos.z / CSIZE_Z;
		if (mx != 0 || my != 0 || mz != 0)
			tryMove(-ivec3(mx, my, mz));

		physics.update(time);
		logic.update(time);
		graphics.update(time);

		if (loading)
		{
			bool allPhysicsClean = true;
			physics.chunkPhysics.iterate([&] (ivec3_c &c, physics::ChunkPhysics &cp)
			{
				if (cp.dirty)
					allPhysicsClean = false;
				return true;
			});
			bool allLogicsClean = true;
			logic.chunkGenerateFlags.iterate([&] (ivec3_c &c, bool &flag)
			{
				if (flag)
					allLogicsClean = false;
				return true;
			});
			if (allPhysicsClean && allLogicsClean)
			{
				logic.resetPlayer();
				loading = false;
			}
		}
	}
};

inline void Shared::setBlockType(ivec3 const &b, BlockType type)
{
	ivec3 const c(b / CSIZE);
	ivec3 const b_c(b % CSIZE);

	blockWriteLock.lock(c);
	blockTypes.blockInChunk(c, b_c) = type;
	blockWriteLock.unlock(c);

	physics.chunkPhysics.chunkAt(c).dirty = true;
	graphics.chunkGraphics.chunkAt(c).dirty = true;
	if (b_c.x == 0 && c.x != 0)
	{
		physics.chunkPhysics.chunkAt(c-ivec3(1,0,0)).dirty = true;
		graphics.chunkGraphics.chunkAt(c-ivec3(1,0,0)).dirty = true;
	}
	else if (b_c.x == CSIZE_X-1 && c.x != CCOUNT_X-1)
	{
		physics.chunkPhysics.chunkAt(c+ivec3(1,0,0)).dirty = true;
		graphics.chunkGraphics.chunkAt(c+ivec3(1,0,0)).dirty = true;
	}
	if (b_c.y == 0 && c.y != 0)
	{
		physics.chunkPhysics.chunkAt(c-ivec3(0,1,0)).dirty = true;
		graphics.chunkGraphics.chunkAt(c-ivec3(0,1,0)).dirty = true;
	}
	else if (b_c.y == CSIZE_Y-1 && c.y != CCOUNT_Y-1)
	{
		physics.chunkPhysics.chunkAt(c+ivec3(0,1,0)).dirty = true;
		graphics.chunkGraphics.chunkAt(c+ivec3(0,1,0)).dirty = true;
	}
	if (b_c.z == 0 && c.z != 0)
	{
		physics.chunkPhysics.chunkAt(c-ivec3(0,0,1)).dirty = true;
		graphics.chunkGraphics.chunkAt(c-ivec3(0,0,1)).dirty = true;
	}
	else if (b_c.z == CSIZE_Z-1 && c.z != CCOUNT_Z-1)
	{
		physics.chunkPhysics.chunkAt(c+ivec3(0,0,1)).dirty = true;
		graphics.chunkGraphics.chunkAt(c+ivec3(0,0,1)).dirty = true;
	}
}

inline bool Shared::onGround()
{
	ivec3 feet = physics.playerBody->getWorldTransform().getOrigin();
	ivec3 check[2] = {feet, feet + ivec3(0, -1, 0)};

	for (int i = 0; i < 2; ++i)
	{
		bool const valid = blockTypes.isValidBlockCoord(check[i]);
		BlockType type = valid ? blockTypes.blockAt(check[i]) : BlockType::OUTSIDE;
		if (type == BlockType::GROUND || type == BlockType::GROUND2)
			return true;
	}
	return false;
}


inline int Shared::createEntity(EntityType aType, fvec3 p, logic::EntityLogics data)
{
	int createdEnt = -1;
	auto tryCreate = [&] (int e, EntityType &type)
	{
		if (type == EntityType::NONE)
		{
			createdEnt = e;
			type = aType;
			graphics.entityGraphics[e].dirty = true;
			physics.entityPhysics[e].dirty = true;
			logic.entityLogics[e].dirty = true;
			physics.entityStartPos[e] = p;
			return false;
		}
		return true;
	};
	entityTypes.iterate(tryCreate);
	if (createdEnt == -1)
	{
		resizeEntityArrays();
		entityTypes.iterate(tryCreate);
		if (createdEnt == -1)
			std::cerr << "Couldn't create entity for unknown reason... (maybe out of memory / corruption)\n";
	}
	return createdEnt;
}

inline void Shared::destroyEntity(int e)
{
	entityTypes[e] = EntityType::NONE;
	logic.entityLogics[e].dirty = true;
	physics.entityPhysics[e].dirty = true;
	graphics.entityGraphics[e].dirty = true;
}

int constexpr Shared::CCOUNT_X;
int constexpr Shared::CCOUNT_Y;
int constexpr Shared::CCOUNT_Z;
int constexpr Shared::CSIZE_X;
int constexpr Shared::CSIZE_Y;
int constexpr Shared::CSIZE_Z;
ivec3 constexpr Shared::CCOUNT;
ivec3 constexpr Shared::CSIZE;

}

#endif /* WORLD_HPP_ */
