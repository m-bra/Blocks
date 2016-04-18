#include "precompiled.hpp"

#include "Module.hpp"

#include "PhysicsProvider.hpp"
#include "GraphicsProvider.hpp"

#include "logic/DefaultLogic.hpp"

#include <functional>

namespace blocks
{

void pushModuleRecursively(std::vector<Module *> &modules, std::vector<Module *> &parents, Module *m, Module *parent = 0)
{
	modules.push_back(m);
	parents.push_back(parent);

	std::vector<Module *> subs;
	m->getSubModules(subs);
	for (Module *s : subs)
		pushModuleRecursively(modules, parents, s, m);
}

World::World(std::vector<Module *> &a_modules, GraphicsProvider *a_graphics, PhysicsProvider *a_physics)
{
	chunkWriteLocks.create(ccount);
	blockTypes.create(ccount, csize);

	blockTypes.fill(blockType.none);

	pos.z = time(0) % 1000;
	pos.x = (time(0) % 1000000) / 1000;

	graphics = a_graphics;
	physics = a_physics;

	std::vector<Module *> parents;

	for (auto it = a_modules.begin(); it != a_modules.end(); ++it)
		pushModuleRecursively(modules, parents, *it);

	assert(parents.size() == modules.size());

	for (int i = 0; i < modules.size(); ++i)
		modules[i]->register(this, parents[i]);

	blockType.none = createBlockType();
	blockType.outside = createBlockType();
	blockType.air = createBlockType();
	blockType.ground = createBlockType();
	blockType.ground2 = createBlockType();
	blockType.companion = createBlockType();

	entityType.none = createEntityType();
	entityType.player = createEntityType();
	entityTypeHoldSlotCount[entityType.player] = 1;
	entityType.block = createEntityType();

	fvec3 playerPos = ccount * csize / 2;
	playerPos.y = ccount.y * csize.y;

	EntityArgs args;
	args.type = entityType.player;
	args.pos = playerPos;
	playerEntity = createEntity({&args});

	timer.add([&]{
		// wrap entities fallen out of the world
		entityKill.iterate([&](Entity e, int &kill)
		{
			if (entityTypes[e] != entityType.none)
			{
				fvec3 pos = getEntityPos(e);
				if (pos.y < -100)
					setEntityPos(e, pos * fvec3::XZ + fvec3::Y * 100);
				if (pos.x < 0 || pos.z < 0 || pos.x > ccount.x * csize.x || pos.z > ccount.z * csize.z)
					killEntity(e);
			}
		});
	}, 1);
}

World::~World()
{
	destroyEntity(playerEntity);
	for (auto it = modules.rbegin(); it != modules.rend(); ++it)
		(*it)->deregister();
}

void World::setEntityPos(Entity e, fvec3_c &pos)
{
	physics->setEntityPos(e, pos);
}

fvec3 World::getEntityPos(Entity e)
{
	return physics->getEntityPos(e);
}

void World::tryMove(ivec3_c &m)
{
	bool canMove = true;
	for (Module *cl : modules)
		if (!cl->canMoveArea())
			canMove = false;

	if (!loading && canMove)
		if (moveLock.try_lock())
		{
			pos+= m * csize;

			blockTypes.shift(-m);
			for (Module *cl : modules)
				cl->moveArea(m);
			moveLock.unlock();
		}
}

/*
* Movement is relative to the camera (including the direction)
* e.g. vector (0,0,1) (pointing to z positive) is FORWARDS
*/
void World::setWalk(int e, fvec3_c &moveSpeeds)
{
	//if (isPlayerOnGround(e))
    	physics->setEntityForce(e,
        	(graphics->camDir * fvec3::XZ).normalize() * moveSpeeds.z * 300
        	+(graphics->camLeft * fvec3::XZ).normalize() * moveSpeeds.x * 300);
}

void World::resetPlayer(Entity e)
{
	fvec3 const playerPos = getEntityPos(e);
	int bx = ccount.x * csize.x / 2;
	int bz = ccount.z * csize.z / 2;
	for (int i = 0; i < 50; ++i)
	{
		for (int by = ccount.y * csize.y-2; by >= 1; --by)
			if (blockTypes.blockAt(ivec3(bx, by, bz)) == blockType.air
				&& blockTypes.blockAt(ivec3(bx, by+1, bz)) == blockType.air
				&& (
					blockTypes.blockAt(ivec3(bx, by-1, bz)) == blockType.ground
				 || blockTypes.blockAt(ivec3(bx, by-1, bz)) == blockType.ground2
				))
			{
				fvec3 newpos = fvec3(bx+.5, by+playerHeight/2, bz+.5);
				setEntityPos(e, newpos);
				assert(newpos == getEntityPos(e));
				return;
			}
		bx = rand() % ccount.x * csize.x;
		bz = rand() % ccount.z * csize.z;
	}
	LOG_ERR("Cannot reset player, no free ground found.");
}

void World::jump(Entity e)
{
    // sometimes doesnt jump although on ground
    if (isPlayerOnGround(e))
    	physics->addEntityImpulse(e, {0, 150, 0});
}

void World::take(Entity e, int slot)
{
    // if selected block
    ivec3 b1, b2;
    fvec3 from, to;
    from = getEntityPos(e) + entityEyePos[e];
    to = from + graphics->camDir * 10;
    bool const isSelectingBlock = physics->getSelectedBlock(from, to, b1, b2);
    bool const isBlockValid = blockTypes.isValidBlockCoord(b1);

    int const selectedEntity = physics->getSelectedEntity(from, to);

	assert(slot < entityTypeHoldSlotCount[entityTypes[e]]);

    // push away held entity
    int &heldEntity = entityHoldSlots[e][slot];
    if (heldEntity != -1)
    {
		physics->addEntityVelocity(heldEntity, graphics->camDir * 10);
        for (Module *m : modules)
			m->onEntityDrop(heldEntity, slot, e);
        heldEntity = -1;
    }
    else
    {
        if (isSelectingBlock && isBlockValid)
        {
            if (selectedEntity != -1)
                LOG_ERR("Selected entity AND block simultaneously"
                    "(none of getSelectedBlock(...) and getSelectedEntity(...) failed)");

            fvec3 pos = b1 + fvec3(.5, .5, .5);
			EntityArgs world_args;
			world_args.type = entityType.block;
			world_args.pos = pos;
			logic::DefaultLogic::EntityArgs logic_args;
			logic_args.blockEntityBlockType = blockTypes.blockAt(b1);
            heldEntity = createEntity({&world_args, &logic_args});

            setBlockType(b1, blockType.air);
        }
        else
        {
			// set held entity to WHICHEVER entity is selected, could also be NO entity at all (-1)
            heldEntity = selectedEntity;
			if (heldEntity == e)
				LOG_WARNING("Entity ", e, " is holding itself.");
        }

		if (heldEntity != -1)
			for (Module *m : modules)
				m->onEntityTake(heldEntity, slot, e);

    }
}

void World::place(Entity e, int slot)
{
	for (Module *m : modules)
		m->onEntityPlace(entityHoldSlots[e][slot], slot, e);
	entityHoldSlots[e][slot] = -1;
}

void World::resizeEntityArrays()
{
	int const size = entityTypes.getCount();
	int const newSize = 10 + size * 2;

	entityTypes.resize(newSize, entityType.none);
	entityEyePos.resize(newSize);
	entityKill.resize(newSize, false);
	entityHoldSlots.resize(newSize);
	entityHoldDistances.resize(newSize);

	for (Module *m : modules)
		m->onEntityCountChange(newSize);
}

void World::update(GameTime time)
{
	fvec3 ppos = getEntityPos(playerEntity);
	int mx = ccount.x/2 - ppos.x / csize.x;
	int my = 0;
	int mz = ccount.z/2 - ppos.z / csize.z;
	if (mx != 0 || my != 0 || mz != 0)
		tryMove(-ivec3(mx, my, mz));

	for (Module *m : modules)
		m->onUpdate(time);

	timer.update(time);

	entityKill.iterate([&](Entity e, int &kill)
	{
		if (kill)
		{
			destroyEntity(e);
			kill = false;
		}
	});

	if (loading)
	{
		bool allDone = true;
		for (Module *m : modules)
			if (m->loading)
				allDone = false;

		if (allDone)
		{
			loading = false;
			resetPlayer(playerEntity);
		}
	}
}

void World::parallel(GameTime gtime)
{
	for (Module *p : modules)
		p->parallel(gtime);
}

void World::setBlockType(ivec3 const &b, BlockType type)
{
	ivec3 const c(b / csize);
	ivec3 const b_c(b % csize);

	chunkWriteLocks[c].lock();
	blockTypes.blockInChunk(c, b_c) = type;
	chunkWriteLocks[c].unlock();

	for (Module *m : modules)
		m->onBlockChange(b);

	#define CHUNK_CHANGED(C) \
	{ \
	for (Module *m : modules) \
		m->onChunkChange(C, false); \
	}


	CHUNK_CHANGED(c);

	if (b_c.x == 0 && c.x != 0) 					  CHUNK_CHANGED(c - ivec3(1, 0, 0))
	else if (b_c.x == csize.x-1 && c.x != ccount.x-1) CHUNK_CHANGED(c + ivec3(1, 0, 0))
	if (b_c.y == 0 && c.y != 0)						  CHUNK_CHANGED(c - ivec3(0, 1, 0))
	else if (b_c.y == csize.y-1 && c.y != ccount.y-1) CHUNK_CHANGED(c + ivec3(0, 1, 0))
	if (b_c.z == 0 && c.z != 0)						  CHUNK_CHANGED(c - ivec3(0, 0, 1))
	else if (b_c.z == csize.z-1 && c.z != ccount.z-1) CHUNK_CHANGED(c + ivec3(0, 0, 1))
}

void World::onBlockChange(ivec3_c &b)
{
	ivec3 const c(b / csize);
	ivec3 const b_c(b % csize);

	for (Module *m : modules)
		m->onBlockChange(b);

	#define CHUNK_CHANGED(C) \
	{ \
	for (Module *m : modules) \
		m->onChunkChange(C, false); \
	}


	CHUNK_CHANGED(c);

	if (b_c.x == 0 && c.x != 0) 					  CHUNK_CHANGED(c - ivec3(1, 0, 0))
	else if (b_c.x == csize.x-1 && c.x != ccount.x-1) CHUNK_CHANGED(c + ivec3(1, 0, 0))
	if (b_c.y == 0 && c.y != 0)						  CHUNK_CHANGED(c - ivec3(0, 1, 0))
	else if (b_c.y == csize.y-1 && c.y != ccount.y-1) CHUNK_CHANGED(c + ivec3(0, 1, 0))
	if (b_c.z == 0 && c.z != 0)						  CHUNK_CHANGED(c - ivec3(0, 0, 1))
	else if (b_c.z == csize.z-1 && c.z != ccount.z-1) CHUNK_CHANGED(c + ivec3(0, 0, 1))
}

void World::onChunkChange(ivec3_c &c, bool direct)
{
	for (Module *m : modules)
		m->onChunkChange(c, direct);
}

bool World::isPlayerOnGround(int e)
{
	fvec3 feet = getEntityPos(e) - fvec3::Y * playerHeight / 2;
	ivec3 check[] = {feet, feet - fvec3::Y * .1};

	for (int i = 0; i < sizeof check / sizeof *check; ++i)
	{
		bool const valid = blockTypes.isValidBlockCoord(check[i]);
		BlockType type = valid ? blockTypes.blockAt(check[i]) : blockType.outside;
		if (type == blockType.ground || type == blockType.ground2)
			return true;
	}
	return false;
}

int World::createEntity(std::vector<BaseEntityArgs *> const &args)
{
	EntityArgs *world_args = getFirstByType<EntityArgs>(args);
	assert(world_args);

	int createdEnt = -1;
	auto tryCreate = [&] (Entity e, EntityType &type)
	{
		if (type == entityType.none)
		{
			createdEnt = e;
			type = world_args->type;
			entityHoldSlots[e].resize(entityTypeHoldSlotCount[type]);
			entityHoldDistances[e].resize(entityTypeHoldSlotCount[type]);
			for (Module *m : modules)
				m->onEntityCreate(e, args);
			return false;
		}
		return true;
	};
	entityTypes.iterate_cond(tryCreate);
	if (createdEnt == -1)
	{
		resizeEntityArrays();
		entityTypes.iterate_cond(tryCreate);
		if (createdEnt == -1)
			LOG_FATAL("Couldn't create entity ", createdEnt, " for unknown reason... (maybe corruption / out of memory)\n");
	}
	return createdEnt;
}

void World::destroyEntity(Entity e)
{
	for (auto it = modules.rbegin(); it != modules.rend(); ++it)
		(*it)->onEntityDestroy(e);
	entityTypes[e] = entityType.none;
}

int World::createEntityType()
{
	int et = entityTypeCount++;
	for (Module *m : modules)
		m->onEntityTypeCreate(et);
	return et;
}

int World::createBlockType()
{
	int bt = blockTypeCount++;
	for (Module *m : modules)
		m->onBlockTypeCreate(bt);
	return bt;
}

}
