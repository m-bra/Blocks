#include "precompiled.hpp"

#include "World.hpp"

#include "Logger.hpp"

namespace blocks
{

void pushRegisterableRecursively(std::vector<Registerable *> &registerables, Registerable *r)
{
	registerables.push_back(r);

	std::vector<Registerable *> subs;
	r->getSubRegisterables(subs);
	for (Registerable *s : subs)
		pushRegisterableRecursively(registerables, s);
}

World::World(Registerable **p_registerables, int registerables_count)
{
	blockWriteLock._mutex.create(count);
	blockTypes.create(count, size);

	blockTypes.fill(BlockType::NONE);

	pos.z = time(0) % 1000;
	pos.x = (time(0) % 1000000) / 1000;

	for (int i = 0; i < registerables_count; ++i)
		pushRegisterableRecursively(registerables, p_registerables[i]);

	for (Registerable *r : registerables)
	{
		EntityListener *asEntityListener = dynamic_cast<EntityListener *>(r);
		WorldListener *asWorldListener = dynamic_cast<WorldListener *>(r);
		ChunkListener *asChunkListener = dynamic_cast<ChunkListener *>(r);
		LoadCallback *asLoadCallback = dynamic_cast<LoadCallback *>(r);
		ParallelCallback *asParallelCallback = dynamic_cast<ParallelCallback *>(r);

		if (asEntityListener) entityListeners.push_back(asEntityListener);
		if (asWorldListener) worldListeners.push_back(asWorldListener);
		if (asChunkListener) chunkListeners.push_back(asChunkListener);
		if (asLoadCallback) loadCallbacks.push_back(asLoadCallback);
		if (asParallelCallback) parallelCallbacks.push_back(asParallelCallback);
	}

	for (Registerable *r : registerables)
		r->onRegister(this);

	for (WorldListener *wl : worldListeners)
		wl->onWorldCreate(this);

	getRegisterablesByType<GraphicsCallback>(graphics);
	assert(graphics.size() > 0);

	getRegisterablesByType<PhysicsCallback>(physics);
	assert(physics.size() > 0);

	fvec3 playerPos = count * size / 2;
	playerPos.y = count.y * size.y;
	playerEntity = createEntity({{"type", (intptr_t) EntityType::PLAYER}, {"pos", (intptr_t) &playerPos}});
}

World::~World()
{
	destroyEntity(playerEntity);
	for (WorldListener *wl : worldListeners)
		wl->onWorldDestroy();
}

void World::tryMove(ivec3_c &m)
{
	bool canMove = true;
	for (ChunkListener *cl : chunkListeners)
		if (!cl->canMove())
			canMove = false;

	if (!loading && canMove)
		if (moveLock.try_lock())
		{
			pos+= m * size;

			blockTypes.shift(-m);
			for (ChunkListener *cl : chunkListeners)
				cl->move(m);
			moveLock.unlock();
		}
}

void World::resetPlayer()
{
	btVector3 const playerPos = getEntityPos(playerEntity);
	int bx = count.x * size.x / 2;
	int bz = count.z * size.z / 2;
	for (int i = 0; i < 50; ++i)
		{
			for (int by = count.y * size.y-2; by >= 1; --by)
				if (blockTypes.blockAt(ivec3(bx, by, bz)) == BlockType::AIR
					&& blockTypes.blockAt(ivec3(bx, by+1, bz)) == BlockType::AIR
					&& blockTypes.blockAt(ivec3(bx, by-1, bz)) == BlockType::GROUND2)
					{
						setEntityPos(playerEntity, btVector3(bx+.5, by+playerHeight/2, bz+.5));
						return;
					}
					bx = rand() % count.x * size.x;
					bz = rand() % count.z * size.z;
				}
				std::cerr << "ERROR: Cannot reset player.\n";
			}

void World::resizeEntityArrays()
{
	int const size = entityTypes.getCount();
	int const newSize = 10 + size * 2;

	entityTypes.resize(newSize);
	entityEyePos.resize(newSize);
	for (EntityListener *el : entityListeners)
		el->onEntityArrayResize(newSize);
}

void World::update(Time time)
{
	fvec3 ppos = getEntityPos(playerEntity);
	int mx = count.x/2 - ppos.x / size.x;
	int my = 0;
	int mz = count.z/2 - ppos.z / size.z;
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

void World::setBlockType(ivec3 const &b, BlockType type)
{
	ivec3 const c(b / size);
	ivec3 const b_c(b % size);

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
	else if (b_c.x == size.x-1 && c.x != count.x-1) CHUNK_CHANGED(c + ivec3(1, 0, 0))
	if (b_c.y == 0 && c.y != 0)						  CHUNK_CHANGED(c - ivec3(0, 1, 0))
	else if (b_c.y == size.y-1 && c.y != count.y-1) CHUNK_CHANGED(c + ivec3(0, 1, 0))
	if (b_c.z == 0 && c.z != 0)						  CHUNK_CHANGED(c - ivec3(0, 0, 1))
	else if (b_c.z == size.z-1 && c.z != count.z-1) CHUNK_CHANGED(c + ivec3(0, 0, 1))
}

void World::onBlockChange(ivec3_c &b)
{
	ivec3 const c(b / size);
	ivec3 const b_c(b % size);

	#define CHUNK_CHANGED(C) \
	{ \
	for (ChunkListener *cl : chunkListeners) \
		cl->onChunkChange(C); \
	}


	CHUNK_CHANGED(c);

	if (b_c.x == 0 && c.x != 0) 					  CHUNK_CHANGED(c - ivec3(1, 0, 0))
	else if (b_c.x == size.x-1 && c.x != count.x-1) CHUNK_CHANGED(c + ivec3(1, 0, 0))
	if (b_c.y == 0 && c.y != 0)						  CHUNK_CHANGED(c - ivec3(0, 1, 0))
	else if (b_c.y == size.y-1 && c.y != count.y-1) CHUNK_CHANGED(c + ivec3(0, 1, 0))
	if (b_c.z == 0 && c.z != 0)						  CHUNK_CHANGED(c - ivec3(0, 0, 1))
	else if (b_c.z == size.z-1 && c.z != count.z-1) CHUNK_CHANGED(c + ivec3(0, 0, 1))
}

void World::onChunkChange(ivec3_c &c)
{
	for (ChunkListener *cl : chunkListeners)
		cl->onChunkChange(c);
}

bool World::onGround()
{
	ivec3 feet = getEntityPos(playerEntity);
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


int World::createEntity(EntityArgs args)
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

void World::destroyEntity(int e)
{
	entityTypes[e] = EntityType::NONE;
	for (EntityListener *el : entityListeners)
		el->onEntityDestroy(e);
}
}
