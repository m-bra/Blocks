#ifndef WORLD_HPP_
#define WORLD_HPP_

#include <iostream>
#include <atomic>
#include <mutex>
#include <ctime>
#include <glm/glm.hpp>
#include <map>

#include "vec.hpp"
#include "EntityFieldArray.hpp"
#include "ChunkFieldArray.hpp"
#include "BlockFieldArray.hpp"

#include "graphics/Module.hpp"
#include "physics/Module.hpp"

#include "SharedTypes.hpp"
#include <cstdint>

#include "Registerable.hpp"

namespace blocks
{

class Shared
{
public:
	static int constexpr CCOUNT_X = 8;
	static int constexpr CCOUNT_Y = 2;
	static int constexpr CCOUNT_Z = CCOUNT_X;
	static int constexpr CSIZE_X = 16;
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
		ChunkFieldArray<std::mutex> _mutex;

		void lock(ivec3_c &c)
		{
			_mutex.chunkAt(c).lock();
		}

		bool try_lock(ivec3_c &c)
		{
			return _mutex.chunkAt(c).try_lock();
		}

		void unlock(ivec3_c &c)
		{
			_mutex.chunkAt(c).unlock();
		}
	} blockWriteLock;
	ivec3 pos, nextMove;

	// A nice lil class which enables setting the operator [] function.
	// e.g. Indexer indexer(MY_FUNCTION_HERE);
	// when calling indexer[1] -> same as MY_FUNCTION_HERE(1)
	template <typename T>
	struct Indexer
	{
		using Function = std::function<T&(int)>;
		Function const _function;

		Indexer(Function const &function) : _function(function) {}

		T &operator [](int e)
		{
			return _function(e);
		}
	};

	EntityFieldArray<EntityType> entityTypes;
	EntityFieldArray<fvec3> entityEyePos;
	// brace init needed bc of most vexing parse -.-
	Indexer<btVector3> entityPos = Indexer<btVector3>(
	[&] (int e) -> btVector3 &
	{
		assert(e >= 0 && e < physics.entityPhysics.getCount());
		assert(physics.entityPhysics[e].created);
		return physics.entityPhysics[e].body->getWorldTransform().getOrigin();
	} );

	BlockFieldArray<BlockType> blockTypes;

	physics::Module<Shared> physics;
	graphics::Module<Shared> graphics;

	std::vector<WorldListener *> worldListeners;
	std::vector<EntityListener *> entityListeners;
	std::vector<LoadCallback *> loadCallbacks;
	std::vector<ChunkListener *> chunkListeners;

	Time gameTime;
	bool loading = true;
	int playerEntity;

	//fvec3 camPos;
	fvec3 camDir, camLeft, camUp;

	// configuration
	float const reach = 50;
	float const playerHeight = 2;

	Shared(Registerable **registerables, int registerables_count)
	{
		blockTypes.fill(BlockType::NONE);

		pos.z = time(0) % 1000;
		pos.x = (time(0) % 1000000) / 1000;

		for (int i = 0; i < registerables_count; ++i)
		{
			if (registerable[i]->getEntityListener())
				entityListeners.push_back(registerable[i]->getEntityListener());
			if (registerable[i]->getWorldListener())
				entityListeners.push_back(registerable[i]->getWorldListener());
			if (registerable[i]->getChunkListener())
				entityListeners.push_back(registerable[i]->getChunkListener());
			if (registerable[i]->getLoadCallback())
				entityListeners.push_back(registerable[i]->getLoadCallback());
		}

		entityListeners.push_back(&physics);
		chunkListeners.push_back(&physics);
		loadCallbacks.push_back(&physics);
		worldListeners.push_back(&physics);

		worldListeners.push_back(&graphics);
		chunkListeners.push_back(&graphics);
		entityListeners.push_back(&graphics);

		for (WorldListener *wl : worldListeners)
			wl->onWorldCreate(this);

		fvec3 playerPos = CCOUNT * CSIZE / 2;
		playerPos.y = CCOUNT_Y * CSIZE_Y;
		playerEntity = createEntity({{"type", (intptr_t) EntityType::PLAYER}, {"pos", (intptr_t) &playerPos}});
	}

	~Shared()
	{
		destroyEntity(playerEntity);
		for (WorldListener *wl : worldListeners)
			wl->onWorldDestroy();
	}

	void setWindowSize(int x, int y)
	{
		graphics.setWindowSize(x, y);
	}

	Time secondsToWorldTime(float seconds)
	{
		return seconds;
	}

	void setBlockType(ivec3::cref b, BlockType type);

	bool onGround();

	int createEntity(EntityArgs args);
	void destroyEntity(int e);

	bool getSelectedBlock(ivec3 &b1, ivec3 &b2)
	{
		return physics.getSelectedBlock(entityPos[playerEntity], fvec3(entityPos[playerEntity]) + camDir * reach, b1, b2);
	}

	int getSelectedEntity()
	{
		return physics.getSelectedEntity(entityPos[playerEntity], fvec3(entityPos[playerEntity]) + camDir * reach);
	}

	void resetPlayer();

	void resizeEntityArrays();
	void tryMove(ivec3_c &m);
	void update(Time time);
};

inline void Shared::tryMove(ivec3_c &m)
{
	bool canMove = true;
	for (ChunkListener *cl : chunkListeners)
		if (!cl->canMove())
			canMove = false;

	if (!loading && canMove)
		if (moveLock.try_lock())
		{
			pos+= m * CSIZE;

			blockTypes.shift(-m);
			for (ChunkListener *cl : chunkListeners)
				cl->move(m);
			moveLock.unlock();
		}
}

inline void Shared::resetPlayer()
{
	btVector3 &playerPos = entityPos[playerEntity];
	int bx = CCOUNT_X*CSIZE_X / 2;
	int bz = CCOUNT_Z*CSIZE_Z / 2;
	for (int i = 0; i < 50; ++i)
		{
			for (int by = CCOUNT_Y*CSIZE_Y-2; by >= 1; --by)
				if (blockTypes.blockAt(ivec3(bx, by, bz)) == BlockType::AIR
					&& blockTypes.blockAt(ivec3(bx, by+1, bz)) == BlockType::AIR
					&& blockTypes.blockAt(ivec3(bx, by-1, bz)) == BlockType::GROUND2)
					{
						playerPos = btVector3(bx+.5, by+playerHeight/2, bz+.5);
						return;
					}
					bx = rand() % CCOUNT_X*CSIZE_X;
					bz = rand() % CCOUNT_Z*CSIZE_Z;
				}
				std::cerr << "ERROR: Cannot reset player.\n";
			}

inline void Shared::resizeEntityArrays()
{
	int const size = entityTypes.getCount();
	int const newSize = 10 + size * 2;

	entityTypes.resize(newSize);
	entityEyePos.resize(newSize);
	for (EntityListener *el : entityListeners)
		el->onEntityArrayResize(newSize);
}

inline void Shared::update(Time time)
{
	fvec3 ppos = entityPos[playerEntity];
	int mx = CCOUNT_X/2 - ppos.x / CSIZE_X;
	int my = 0;
	int mz = CCOUNT_Z/2 - ppos.z / CSIZE_Z;
	if (mx != 0 || my != 0 || mz != 0)
		tryMove(-ivec3(mx, my, mz));

	for (WorldListener *wl : worldListeners)
		wl->onWorldUpdate(time);

	for (int e = 0; e < entityTypes.getCount(); ++e)
		if (entityTypes[e] != EntityType::NONE)
			for (EntityListener *el : entityListeners)
				el->onEntityUpdate(e, time);

	if (loading)
	{
		bool allDone = true;
		for (LoadCallback *loadcb : loadCallbacks)
			if (!loadcb->doneLoading())
				allDone = false;

		if (allDone)
		{
			resetPlayer();
			loading = false;
		}
	}
}

inline void Shared::setBlockType(ivec3 const &b, BlockType type)
{
	ivec3 const c(b / CSIZE);
	ivec3 const b_c(b % CSIZE);

	blockWriteLock.lock(c);
	blockTypes.blockInChunk(c, b_c) = type;
	blockWriteLock.unlock(c);

	#define CHUNK_CHANGED(C) \
	{ \
	for (ChunkListener *cl : chunkListeners) \
		cl->onChunkChange(C); \
	}


	CHUNK_CHANGED(c);

	if (b_c.x == 0 && c.x != 0) 					  CHUNK_CHANGED(c - ivec3(1, 0, 0))
	else if (b_c.x == CSIZE_X-1 && c.x != CCOUNT_X-1) CHUNK_CHANGED(c + ivec3(1, 0, 0))
	if (b_c.y == 0 && c.y != 0)						  CHUNK_CHANGED(c - ivec3(0, 1, 0))
	else if (b_c.y == CSIZE_Y-1 && c.y != CCOUNT_Y-1) CHUNK_CHANGED(c + ivec3(0, 1, 0))
	if (b_c.z == 0 && c.z != 0)						  CHUNK_CHANGED(c - ivec3(0, 0, 1))
	else if (b_c.z == CSIZE_Z-1 && c.z != CCOUNT_Z-1) CHUNK_CHANGED(c + ivec3(0, 0, 1))
}

inline bool Shared::onGround()
{
	ivec3 feet = entityPos[playerEntity];
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


inline int Shared::createEntity(EntityArgs args)
{
	assert(args.find("type") != args.end());
	// we do NOT want this since this is removed!
	assert(args.find("eyePos") == args.end());

	int createdEnt = -1;
	auto tryCreate = [&] (int e, EntityType &type)
	{
		if (type == EntityType::NONE)
		{
			createdEnt = e;
			type = static_cast<EntityType>(args["type"]);
			for (EntityListener *el : entityListeners)
				el->onEntityCreate(e, args);
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
			Log::fatalError("Couldn't create entity for unknown reason... (maybe corruption / out of memory)\n");
	}
	return createdEnt;
}

inline void Shared::destroyEntity(int e)
{
	entityTypes[e] = EntityType::NONE;
	for (EntityListener *el : entityListeners)
		el->onEntityDestroy(e);
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
