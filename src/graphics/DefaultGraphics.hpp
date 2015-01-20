#ifndef GRAPHICS_GRAPHICS_HPP_INCLUDED
#define GRAPHICS_GRAPHICS_HPP_INCLUDED

#ifndef PRECOMPILED_HPP_INCLUDED
#warning This header assumes "precompiled.hpp" to be #included
#include "precompiled.hpp"
#endif

#include <mutex>
#include <atomic>
#include <vector>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include <GLL/GLL.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Module.hpp"

#include "graphics/Types.hpp"
#include "graphics/BlockFuncs.hpp"
#include "graphics/EntityFuncs.hpp"

#include "GraphicsProvider.hpp"

namespace blocks
{

namespace graphics
{

class DefaultGraphics : public Module, public GraphicsProvider
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

	void onRegister() override;
	void onDeregister() override;

	void onEntityCreate(Entity e, std::vector<BaseEntityArgs *> const &args) override;
	void onEntityDestroy(Entity e) override;
	void onEntityCountChange(int newsize) override
	{
		entityGraphics.resize(newsize);
	}

	void getSubModules(std::vector<Module *> &subs) override
	{
		subs.push_back(&entityFuncs);
		subs.push_back(&blockFuncs);
	}

	void setFrameBufSize(int x, int y) override;
	void onUpdate(GameTime time) override;
	void parallel(GameTime time) override;
	void render() override;
	bool buildChunk(ivec3_c &) override;

	bool canMoveArea() override;
	void moveArea(ivec3_c &m) override;
	void onChunkChange(ivec3_c &c) override;
};

}

}


#endif /* GRAPHICS_HPP_ */
