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

#include "../World.hpp"
#include "../Registerable.hpp"

#include "Types.hpp"
#include "EntityFuncs.hpp"

namespace blocks
{

namespace physics {class Module;}

namespace logic
{

class Module : public Registerable, public EntityListener, public WorldListener, public LoadCallback, public ChunkListener, public ParallelCallback
{
private:
	World *world;
	physics::Module *physics;
	int seed;
	EntityFuncs entityFuncs{this};
public:
	EntityFieldArray<EntityLogics> entityLogics;
	ChunkFieldArray<bool> chunkGenerateFlags;

	void onWorldCreate(World *world);
	void onWorldUpdate(Time time);
	void onWorldDestroy();
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

	ParallelCallback *getParallelCallback() {return this;}

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
