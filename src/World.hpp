#ifndef WORLD_HPP_INCLUDED
#define WORLD_HPP_INCLUDED

#include <iostream>
#include <atomic>
#include <mutex>
#include <ctime>
#include <glm/glm.hpp>
#include <map>
#include <cstdint>
#include <typeinfo>

#include "vec.hpp"
#include "EntityFieldArray.hpp"
#include "ChunkFieldArray.hpp"
#include "BlockFieldArray.hpp"

#include "WorldTypes.hpp"

#include "Registerable.hpp"

namespace blocks
{

class World
{
public:
	// chunk size and count
	ivec3_c count{8, 2, 8}, size{16, 32, 16};

	std::mutex moveLock;
	struct
	{
		ChunkFieldArray<std::mutex> _mutex;

		void lock(ivec3_c &c)
		{
			_mutex[c].lock();
		}

		bool try_lock(ivec3_c &c)
		{
			return _mutex[c].try_lock();
		}

		void unlock(ivec3_c &c)
		{
			_mutex[c].unlock();
		}
	} blockWriteLock;
	ivec3 pos, nextMove;

	EntityFieldArray<EntityType> entityTypes;
	EntityFieldArray<fvec3> entityEyePos;

	BlockFieldArray<BlockType> blockTypes;

	std::vector<GraphicsCallback *> graphics;
	std::vector<PhysicsCallback *> physics;

	std::vector<Registerable *> registerables;
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

	World(Registerable **p_registerables, int registerables_count);
	~World();

	template <typename T>
	T *getFirstRegisterableByType()
	{
		for (Registerable *r : registerables)
			if (typeid(*r) == typeid(T))
				return (T *) r;
		return 0;
	}

	template <typename T>
	void getRegisterablesByType(std::vector<T *> registerables)
	{
		for (Registerable *r : registerables)
			if (typeid(*r) == typeid(T))
				registerables.push_back(r);
	}

	void setWindowSize(int x, int y)
	{
		for (GraphicsCallback *g : graphics)
			g->setWindowSize(x, y);
	}

	Time secondsToWorldTime(float seconds)
	{
		return seconds;
	}

	void setBlockType(ivec3::cref b, BlockType type);
	void onBlockChange(ivec3_c &b);
	void onChunkChange(ivec3_c &c);

	bool onGround();

	int createEntity(EntityArgs args);
	void destroyEntity(int e);

	void resetPlayer();

	void resizeEntityArrays();
	void tryMove(ivec3_c &m);
	void update(Time time);
};

}

#endif//WORLD_HPP_INCLUDED
