#ifndef LOGIC_LOGIC_HPP_INCLUDED
#define LOGIC_LOGIC_HPP_INCLUDED

#ifndef PRECOMPILED_HPP_INCLUDED
#warning This header assumes "precompiled.hpp" to be #included
#include "precompiled.hpp"
#endif

#include <vector>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include <btBulletDynamicsCommon.h>

#include "Module.hpp"

#include "logic/Types.hpp"
#include "logic/EntityFuncs.hpp"

#include "EntityArgs.hpp"

namespace blocks
{

namespace physics {class BulletPhysics;}

namespace logic
{

class DefaultLogic : public Module
{
private:
	physics::BulletPhysics *physics;
	int seed;
	EntityFuncs entityFuncs;
public:
	struct EntityArgs : BaseEntityArgs
	{
		BlockType blockEntityBlockType;
	};

	EntityFieldArray<EntityLogics> entityLogics;
	ChunkFieldArray<bool> chunkGenerateFlags;


	void onEntityCreate(Entity e, std::vector<BaseEntityArgs *> const &args) override;
	void onEntityDestroy(Entity e) override;
	void onEntityCountChange(int newSize) override
	{
		entityLogics.resize(newSize);
	}

	void onRegister() override;
	void onDeregister() override;
	void onUpdate(GameTime time) override;
	void getSubModules(std::vector<Module *> &subs) override
	{
		subs.push_back(&entityFuncs);
	}

	void generate(ivec3_c &c);
	void parallel(GameTime time) override;

	bool canMoveArea() override {return true;}
	void moveArea(ivec3_c &m) override;
};

}

}

#endif//LOGIC_HPP_INCLUDED
