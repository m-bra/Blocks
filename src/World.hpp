#ifndef WORLD_HPP_INCLUDED
#define WORLD_HPP_INCLUDED

#ifndef PRECOMPILED_HPP_INCLUDED
#warning #include "precompiled.hpp" at the beginning of the TU!
#include "precompiled.hpp"
#endif

#include "Timer.hpp"

#include "EntityArgs.hpp"

#include <mutex>

#include <map>


namespace blocks
{

class Module;

using GameTime = float;
using EntityType = unsigned char;
using BlockType = unsigned char;
using Entity = int;

template <typename T, typename B>
T *getFirstByType(std::vector<B *> const &vector)
{
	for (B *base : vector)
	{
		T *promoted = dynamic_cast<T *>(base);
		if (promoted)
			return promoted;
	}
	return 0;
}

template <typename T, typename B>
T *getAllByType(std::vector<T *> &results, std::vector<B *> const &vector)
{
	for (B *base : vector)
	{
		T *promoted = dynamic_cast<T *>(base);
		if (promoted)
			results.push_back(promoted);
	}
}

class World
{
public:
	struct EntityArgs : BaseEntityArgs
	{
		EntityType type;
		fvec3 pos;
	};

	float reach = 50;
	float playerHeight = 2;
	// chunk count and size
	ivec3 ccount = ivec3(32, 1, 32);
	ivec3 csize = ivec3(16, 16, 16);

	std::mutex moveLock;
	ChunkFieldArray<std::mutex> chunkWriteLocks;
	ivec3 pos{0, 0, 0}, nextMove;

	int entityTypeCount = 0, blockTypeCount = 0;

	struct
	{
		EntityType none, player, block;
	} entityType;

	struct
	{
		BlockType none, outside, air, ground, ground2, companion;
	} blockType;

	// every entity CAN have "hold slots". if an entitytype is not listed in this map,
	// entities of that type will not have any slots.
	// a hold slot is essentially an index to an entity, which is holded by the entity which owns the slot.
	std::map<EntityType, int> entityTypeHoldSlotCount;

	EntityFieldArray<EntityType> entityTypes;
	EntityFieldArray<fvec3> entityEyePos;
	EntityFieldArray<bool> entityKill;
	EntityFieldArray<std::vector<Entity>> entityHoldSlots;
	EntityFieldArray<std::vector<float>> entityHoldDistances;

	// to set a blocks, you need to consider two things:
	// 1. call onChunkChange and optionally onBlockChange afterwards
	//    NOTE that if the block borders on another chunk onChunkChange()'s got to be called there, too!
	// 2. lock the corresponding chunkWriteLock
	// OR: use setBlockType() which handles the dirty stuff for ye.
	// OR: don't care about it and break the game! :) :) :) :) 			(no. please dont.)
	BlockFieldArray<BlockType> blockTypes;

	class GraphicsProvider *graphics;
	class PhysicsProvider *physics;
	std::vector<Module *> modules;

	GameTime gameTime;
	Timer timer;
	bool loading = true;
	int playerEntity = -1;

	World(std::vector<Module *> &a_modules, GraphicsProvider *a_graphics, PhysicsProvider *a_physics);
	~World();

	template <typename T>
	T *getFirstModuleByType() {return getFirstByType<T>(modules);}

	template <typename T>
	void getModulesByType(std::vector<T *> &arg) {getAllByType(arg, modules);}

	void setBlockType(ivec3_c &b, BlockType type);
	void onBlockChange(ivec3_c &b);
	void onChunkChange(ivec3_c &c, bool direct);

	bool isPlayerOnGround(int e);

	int createEntity(std::vector<BaseEntityArgs *> const &args);
	void killEntity(Entity e) {destroyEntity(e);}//{entityKill[e] = true;}
	void setEntityPos(Entity e, fvec3_c &pos);
	fvec3 getEntityPos(Entity e);

	int createEntityType();
	int createBlockType();

	void setWalk(int e, fvec3_c &moveSpeeds);
	void resetPlayer(Entity e);
	void jump(Entity e);
	void take(Entity e, int slot);
	void place(Entity e, int slot);

	void update(GameTime gtime);
	void parallel(GameTime gtime);

private:
	void destroyEntity(Entity e);
	void resizeEntityArrays();
	void tryMove(ivec3_c &m);
};

}

#endif//WORLD_HPP_INCLUDED
