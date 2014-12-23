#ifndef LOGIC_HPP_INCLUDED
#define LOGIC_HPP_INCLUDED

#include <fstream>
#include <ctime>
#include <iostream>
#include <thread>
#include <chrono>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>

#include <btBulletDynamicsCommon.h>

#include "../Logger.hpp"
#include "../BlockFieldArray.hpp"
#include "../ChunkFieldArray.hpp"
#include "../EntityFieldArray.hpp"
#include "../vec.hpp"

#include "../Shared.hpp"
#include "../SharedTypes.hpp"
#include "../Registerable.hpp"

#include "Types.hpp"
#include "EntityFuncs.hpp"

namespace blocks
{

namespace logic
{

class Module : public Registerable, public EntityListener, public WorldListener, public LoadCallback, public ChunkListener
{
private:
	template <typename T>
	using ChunkFieldArray = ChunkFieldArray<Shared::ChunkFieldArraySize, T>;
	template <typename T>
	using BlockFieldArray = BlockFieldArray<Shared::BlockFieldArraySize, T>;

	Shared *shared;
	int seed;
	EntityFuncs<Shared> entityFuncs;
public:
	EntityFieldArray<EntityLogics> entityLogics;
	ChunkFieldArray<bool> chunkGenerateFlags;

	void onWorldCreate(Shared *shared);
	void onWorldUpdate(Time time);
	void onWorldDestroy() {}
	WorldListener *getWorldListener() {return this;}

	void onEntityCreate(int e, EntityArgs args);
	void onEntityDestroy(int e);
	void onEntityUpdate(int e, Time time);
	void onEntityArrayResize(int newSize)
	{
		entityLogics.resize(newSize);
	}
	EntityListener *getEntityListener() {return this;}

	bool doneLoading();
	LoadCallback *getLoadCallback() {return this;}

	void generate(ivec3 const &c);
	void parallel(Time time);
	void update(Time time);

	bool canMove() {return true;}
	void move(ivec3_c &m);
	void onChunkChange(ivec3_c &c) {}
	ChunkListener *getChunkListener() {return this;}

	void setWalk(fvec3_c &moveSpeeds);
	void jump();
	void resetPlayer();

	void use();
	void place();
	void take();
	void supertake();
};

}

}

#endif//LOGIC_HPP_INCLUDED
