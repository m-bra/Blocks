#ifndef LOGIC_LOGIC_HPP_INCLUDED
#define LOGIC_LOGIC_HPP_INCLUDED

#ifndef PRECOMPILED_HPP_INCLUDED
#warning This header assumes "precompiled.hpp" to be #included
#endif

#include <vector>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include "../vec.hpp"
#include "../Logger.hpp"

#include <btBulletDynamicsCommon.h>

#include "../BlockFieldArray.hpp"
#include "../ChunkFieldArray.hpp"
#include "../EntityFieldArray.hpp"

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
	class World *world;
	physics::Module *physics;
	int seed;
	EntityFuncs entityFuncs;
public:
	EntityFieldArray<EntityLogics> entityLogics;
	ChunkFieldArray<bool> chunkGenerateFlags;

	void onWorldCreate(World *world);
	void onWorldUpdate(Time time);
	void onWorldDestroy();

	void onEntityCreate(int e, EntityArgs args);
	void onEntityDestroy(int e);
	void onEntityUpdate(int e, Time time);
	void onEntityArrayResize(int newSize)
	{
		entityLogics.resize(newSize);
	}

	bool doneLoading();

	void onRegister(World *world);
	void getSubRegisterables(std::vector<Registerable *> &subs)
	{
		subs.push_back(&entityFuncs);
	}

	void generate(ivec3 const &c);
	void parallel(Time time);
	void update(Time time);

	bool canMove() {return true;}
	void move(ivec3_c &m);
	void onChunkChange(ivec3_c &c) {}

	void setWalk(fvec3_c &moveSpeeds);
	void jump();

	void use();
	void place();
	void take();
	void supertake();
};

}

}

#endif//LOGIC_HPP_INCLUDED
