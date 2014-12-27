#ifndef GRAPHICS_HPP_INCLUDED
#define GRAPHICS_HPP_INCLUDED

#include <thread>
#include <mutex>
#include <chrono>
#include <atomic>
#include <vector>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/vector_angle.hpp>

#include <GLL/GLL.hpp>
#include <GL/glew.h>
#include "../CImg.h"
using namespace cimg_library;

#include "../Logger.hpp"
#include "../vec.hpp"

#include "../ChunkFieldArray.hpp"
#include "../BlockFieldArray.hpp"

#include "../Registerable.hpp"

#include "../World.hpp"
#include "Types.hpp"
#include "BlockFuncs.hpp"
#include "EntityFuncs.hpp"

namespace blocks
{

namespace graphics
{

class Module : public Registerable, public WorldListener, public ChunkListener, public EntityListener, public GraphicsCallback
{
private:
	friend class EntityFuncs;

	struct _Context
	{
		_Context()
		{
		    gll::init();
			gll::setDepthTest(true);
			gll::setCulling(true);
		}
	} context;

	gll::Program program;
	struct
	{
		gll::Attribute vertXYZ, vertUV, vertNormalXYZ;
	} attributes;

	struct
	{
		gll::Uniform mvp, model, eyePosXYZ;
		gll::Uniform materialDiffuseTex, materialShininess, materialSpecularRGB;
		gll::Uniform ambientLightRGB, lightRGB, lightXYZ, lightPower;
	} uniforms;
	glm::mat4 projection, projview;

	GLuint chunkTbo, companionTbo;

	World *world;

	BlockFuncs<Shared> blockFuncs;
	EntityFuncs<Shared> entityFuncs;

	ChunkFieldArray<glm::mat4> chunkTransforms;

	std::atomic_bool vertexBufFlushed;
	std::mutex vertexBufLock;
	std::vector<Vertex> vertexBuf;
	ivec3 vertexBufChunk;

	void checkGLError(std::string msg);
public:
	ChunkFieldArray<ChunkGraphics> chunkGraphics;
	EntityFieldArray<EntityGraphics> entityGraphics;

	void onWorldCreate(Shared *shared);
	void onWorldDestroy();
	void onWorldUpdate(Time time);
	WorldListener *getWorldListener() {return this;}

	void onEntityCreate(int e, EntityArgs args);
	void onEntityDestroy(int e);
	void onEntityUpdate(int e, Time time) {}
	void onEntityArrayResize(int newsize)
	{
		entityGraphics.resize(newsize);
	}
	EntityListener *getEntityListener() {return this;}

	void setWindowSize(int x, int y);
	void parallel(Time time);
	void render();
	bool buildChunk(ivec3_c &);

	bool canMove();
	void move(ivec3_c &m);
	void onChunkChange(ivec3_c &c
	ChunkListener *getChunkListener() {return this;}
};

}

}

#endif /* GRAPHICS_HPP_ */
