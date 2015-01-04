#ifndef GRAPHICS_GRAPHICS_HPP_INCLUDED
#define GRAPHICS_GRAPHICS_HPP_INCLUDED

#ifndef PRECOMPILED_HPP_INCLUDED
#warning This header assumes "precompiled.hpp" to be #included
#endif

#include <mutex>
#include <atomic>
#include <vector>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include "../vec.hpp"

#include <GLL/GLL.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>


#include "../ChunkFieldArray.hpp"
#include "../BlockFieldArray.hpp"

#include "../Registerable.hpp"

#include "Types.hpp"
#include "BlockFuncs.hpp"
#include "EntityFuncs.hpp"

namespace blocks
{

namespace graphics
{

// multi inheritence :)))
class Module : public Registerable, public WorldListener, public ChunkListener, public EntityListener, public GraphicsCallback, public ParallelCallback
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

	class World *world;

	BlockFuncs blockFuncs;
	EntityFuncs entityFuncs;

	ChunkFieldArray<glm::mat4> chunkTransforms;

	std::atomic_bool vertexBufFlushed;
	std::mutex vertexBufLock;
	std::vector<Vertex> vertexBuf;
	ivec3 vertexBufChunk;

	void checkGLError(std::string msg);
public:
	ChunkFieldArray<ChunkGraphics> chunkGraphics;
	EntityFieldArray<EntityGraphics> entityGraphics;

	void onWorldCreate(World *world);
	void onWorldDestroy();
	void onWorldUpdate(Time time);

	void onEntityCreate(int e, EntityArgs args);
	void onEntityDestroy(int e);
	void onEntityUpdate(int e, Time time) {}
	void onEntityArrayResize(int newsize)
	{
		entityGraphics.resize(newsize);
	}

	void getSubRegisterables(std::vector<Registerable *> &subs)
	{
		subs.push_back(&entityFuncs);
		subs.push_back(&blockFuncs);
	}

	void setWindowSize(int x, int y);
	void parallel(Time time);
	void render();
	bool buildChunk(ivec3_c &);

	bool canMove();
	void move(ivec3_c &m);
	void onChunkChange(ivec3_c &c);
};

}

}

#endif /* GRAPHICS_HPP_ */
