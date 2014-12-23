#ifndef GRAPHICS_HPP_
#define GRAPHICS_HPP_

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

#include "../SharedTypes.hpp"
#include "Types.hpp"
#include "BlockFuncs.hpp"
#include "EntityFuncs.hpp"

namespace blocks
{

namespace graphics
{

template <typename Shared>
class Module : public Registerable, public WorldListener, public ChunkListener, public EntityListener
{
private:
	friend class EntityFuncs<Shared>;

	template <typename T>
	using ChunkFieldArray = ChunkFieldArray<typename Shared::ChunkFieldArraySize, T>;
	template <typename T>
	using BlockFieldArray = BlockFieldArray<typename Shared::BlockFieldArraySize, T>;

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

	Shared *shared;

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

template <typename Shared>
bool Module<Shared>::canMove()
{
	return vertexBufFlushed;
}

template <typename Shared>
void Module<Shared>::move(ivec3_c &m)
{
	auto createChunk = [&] (ivec3::cref c)
	{
		GLuint vbo, vao;
		glGenBuffers(1, &vbo);
		glGenVertexArrays(1, &vao);
		chunkGraphics.chunkAt(c).vao = vao;
		chunkGraphics.chunkAt(c).vbo = vbo;
	};

	auto destroyChunk = [&] (ivec3::cref c)
	{
		GLuint vbo, vao;
		glGenBuffers(1, &vbo);
		glGenVertexArrays(1, &vao);
		chunkGraphics.chunkAt(c).vao = vao;
		chunkGraphics.chunkAt(c).vbo = vbo;
	};

	chunkGraphics.shift(-m, createChunk, destroyChunk);
}

template <typename Shared>
void Module<Shared>::onChunkChange(ivec3_c &c)
{
	chunkGraphics.chunkAt(c).dirty = true;
}

template <typename Shared>
inline void Module<Shared>::checkGLError(std::string msg)
{
	gll::onOpenGLErr([&] (GLenum err, GLubyte const *errstr)
	{
		Log::error("OpenGL error (" + msg + "): " + std::string((char const *)errstr));
		exit(EXIT_FAILURE);
	});
}

template <typename Shared>
inline void Module<Shared>::onWorldCreate(Shared *shared)
{
	this->shared = shared;
	blockFuncs.onWorldCreate(shared);
	entityFuncs.setBlockFuncs(&blockFuncs);
	entityFuncs.onWorldCreate(shared);
	vertexBufFlushed = true;

	// shader
	program.create();
	program.addShader(GL_VERTEX_SHADER, "res/shaders/phong_frag.vert", true);
	program.addShader(GL_FRAGMENT_SHADER, "res/shaders/phong_frag.frag", true);
	attributes.vertXYZ = program.getAttribute("vertXYZ");
	attributes.vertUV = program.getAttribute("vertUV");
	attributes.vertNormalXYZ = program.getAttribute("vertNormalXYZ");
	program.link();
	uniforms.mvp = program.getUniformLocation("mvp");
	uniforms.model = program.getUniformLocation("model");
	uniforms.eyePosXYZ = program.getUniformLocation("eyePosXYZ");
	uniforms.materialDiffuseTex = program.getUniformLocation("materialDiffuseTex");
	uniforms.materialShininess = program.getUniformLocation("materialShininess");
	uniforms.materialSpecularRGB = program.getUniformLocation("materialSpecularRGB");
	uniforms.ambientLightRGB = program.getUniformLocation("ambientLightRGB");
	uniforms.lightRGB = program.getUniformLocation("lightRGB");
	uniforms.lightXYZ = program.getUniformLocation("lightXYZ");
	uniforms.lightPower = program.getUniformLocation("lightPower");
	program.bind();

	// set uniforms
	glUniform1i(uniforms.materialDiffuseTex, 0);
	glUniform1f(uniforms.materialShininess, 5);
	glUniform3f(uniforms.materialSpecularRGB, .1, .1, .1);
	glUniform3f(uniforms.ambientLightRGB, .12, .12, .12);
	glUniform3f(uniforms.lightRGB, 50, 50, 50);
	glUniform1f(uniforms.lightPower, 2);

	// bind chunk texture
	using ImgComponent = unsigned char;
	CImg<ImgComponent> chunkImg("res/chunk.png");
	ImgComponent *pixels = new ImgComponent[chunkImg.width() * chunkImg.height() * 4];
	if (chunkImg.spectrum() == 4)
		for (int y = 0; y < chunkImg.height(); ++y)
			for (int x = 0; x < chunkImg.width(); ++x)
			{
				pixels[y * chunkImg.width() * 4 + x * 4 + 0] = chunkImg(x, y, 0);
				pixels[y * chunkImg.width() * 4 + x * 4 + 1] = chunkImg(x, y, 1);
				pixels[y * chunkImg.width() * 4 + x * 4 + 2] = chunkImg(x, y, 2);
				pixels[y * chunkImg.width() * 4 + x * 4 + 3] = chunkImg(x, y, 3);
			}
	else if (chunkImg.spectrum() == 3)
		for (int y = 0; y < chunkImg.height(); ++y)
			for (int x = 0; x < chunkImg.width(); ++x)
			{
				pixels[y * chunkImg.width() * 4 + x * 4 + 0] = chunkImg(x, y, 0);
				pixels[y * chunkImg.width() * 4 + x * 4 + 1] = chunkImg(x, y, 1);
				pixels[y * chunkImg.width() * 4 + x * 4 + 2] = chunkImg(x, y, 2);
				pixels[y * chunkImg.width() * 4 + x * 4 + 3] = 255;
			}
	else
		Log::fatalError("Cannot load chunk.png: 3 or 4 channels required");

	glGenTextures(1, &chunkTbo);
	glBindTexture(GL_TEXTURE_2D, chunkTbo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, chunkImg.width(), chunkImg.height(), 0,
		GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	delete[] pixels;

	chunkTransforms.iterate([&] (ivec3 const &c, glm::mat4 &trans)
	{
		trans = glm::translate(c.glm() * glm::vec3(Shared::CSIZE_X,Shared::CSIZE_Y,Shared::CSIZE_Z));
		return true;
	});

	chunkGraphics.iterate([&] (ivec3 const &c, ChunkGraphics &cg)
	{
		cg.create();
		return true;
	});

	checkGLError("init");
}

template <typename Shared>
inline void Module<Shared>::onWorldDestroy()
{
	chunkGraphics.iterate([&] (ivec3 const &c, ChunkGraphics &cg)
	{
		cg.destroy();
		return true;
	});

	program.destroy();
	checkGLError("End");
}

template <typename Shared>
inline void Module<Shared>::setWindowSize(int x, int y)
{
	Log::debug("graphics/Module::setWindowSize() called.");
	projection = glm::perspective<float>(glm::radians(60.), float(x) / y, .1, 1000);
	glViewport(0, 0, x, y);
}

template <typename Shared>
inline void Module<Shared>::onEntityCreate(int e, EntityArgs args)
{
	EntityGraphics &eg = entityGraphics[e];
	assert(!eg.created);
	eg.created = true;

	glGenBuffers(1, &eg.vbo);
	glGenVertexArrays(1, &eg.vao);

	glBindBuffer(GL_ARRAY_BUFFER, eg.vbo);
	glBindVertexArray(eg.vao);

	GLenum const openGLType = gll::OpenGLType<VertexComponent>::type;

	glEnableVertexAttribArray(attributes.vertXYZ);
	glEnableVertexAttribArray(attributes.vertUV);
	glEnableVertexAttribArray(attributes.vertNormalXYZ);
	glVertexAttribPointer(attributes.vertXYZ,
	3, openGLType, GL_FALSE, sizeof (Vertex), (void*)(0 * sizeof (VertexComponent)));
	glVertexAttribPointer(attributes.vertUV,
	2, openGLType, GL_FALSE, sizeof (Vertex), (void*)(3 * sizeof (VertexComponent)));
	glVertexAttribPointer(attributes.vertNormalXYZ,
	3, openGLType, GL_FALSE, sizeof (Vertex), (void*)(5 * sizeof (VertexComponent)));

	entityFuncs.onEntityCreate(e, args);
	eg.vertCount = entityFuncs.putVertices(eg.vbo, e);
}

template <typename Shared>
inline void Module<Shared>::onEntityDestroy(int e)
{
	EntityGraphics &eg = entityGraphics[e];
	assert(eg.created);

	entityFuncs.onEntityDestroy(e);

	eg.created = false;
	glDeleteBuffers(1, &eg.vbo);
	glDeleteVertexArrays(1, &eg.vao);
}

template <typename Shared>
inline void Module<Shared>::parallel(Time time)
{
	chunkGraphics.iterate([&] (ivec3 const &c, ChunkGraphics &graphics)
	{
		if (graphics.dirty)
		{
			buildChunk(c);
			return true;
		}
		return true;
	});
}

template <typename Shared>
inline void Module<Shared>::onWorldUpdate(Time time)
{
	if (!vertexBufFlushed.load())
		if (vertexBufLock.try_lock())
		{
			GLuint vbo = chunkGraphics.chunkAt(vertexBufChunk).vbo;
			GLuint vao = chunkGraphics.chunkAt(vertexBufChunk).vao;

			chunkGraphics.chunkAt(vertexBufChunk).vertCount = vertexBuf.size();

			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, vertexBuf.size() * sizeof (Vertex), vertexBuf.data(), GL_STATIC_DRAW);

			// set format
			glBindVertexArray(vao);

			GLenum const openGLType = gll::OpenGLType<VertexComponent>::type;

			glEnableVertexAttribArray(attributes.vertXYZ);
			glEnableVertexAttribArray(attributes.vertUV);
			glEnableVertexAttribArray(attributes.vertNormalXYZ);
			glVertexAttribPointer(attributes.vertXYZ,
				3, openGLType, GL_FALSE, sizeof (Vertex), (void*)(0 * sizeof (VertexComponent)));
			glVertexAttribPointer(attributes.vertUV,
				2, openGLType, GL_FALSE, sizeof (Vertex), (void*)(3 * sizeof (VertexComponent)));
			glVertexAttribPointer(attributes.vertNormalXYZ,
				3, openGLType, GL_FALSE, sizeof (Vertex), (void*)(5 * sizeof (VertexComponent)));

			vertexBufFlushed = true;
			vertexBufLock.unlock();
		}
}

template <typename Shared>
inline void Module<Shared>::render()
{
	if (shared->loading)
	{
		glClearColor(.1, .1, .2, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		return;
	}

	program.bind();

	fvec3 eyePos = shared->entityPos[shared->playerEntity];
	glUniform3f(uniforms.eyePosXYZ, eyePos.x, eyePos.y, eyePos.z);
	glUniform3f(uniforms.lightXYZ,  eyePos.x, eyePos.y + 200, eyePos.z);

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, chunkTbo);

	projview = projection * glm::lookAt(playerPos.glm(),
		playerPos.glm() + shared->camDir.glm(), shared->camUp.glm());

	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//static double const chunkDiameter = sqrt(World::CSIZE_X*World::CSIZE_X+World::CSIZE_Y*World::CSIZE_Y+World::CSIZE_Z*World::CSIZE_Z);

	glBindTexture(GL_TEXTURE_2D, chunkTbo);

	bool printed = false;
	chunkGraphics.iterate([&] (ivec3_c &c, ChunkGraphics &cg)
	{
		glBindVertexArray(cg.vao);
		glm::mat4 const &model = chunkTransforms.chunkAt(c);
		glm::mat4 const &finalTransform = projview * model;
		glUniformMatrix4fv(uniforms.mvp, 1, false, &finalTransform[0][0]);
		glUniformMatrix4fv(uniforms.model, 1, false, &model[0][0]);

		/*glm::vec4 projected = projview * glm::vec4(
			cx + World::CSIZE_X/2.f, cy + World::CSIZE_Y/2.f, cz + World::CSIZE_Z/2.f, 1);
		projected/= projected.w;
		float projDiameter = chunkDiameter / projected.w;
		if (fabsf(projected.x) > 1+projDiameter || fabsf(projected.y) > 1+projDiameter
			|| projected.z < -projDiameter)
			return true;*/

		glDrawArrays(GL_TRIANGLES, 0, cg.vertCount);
		return true;
	});

	entityGraphics.iterate([&] (int e, EntityGraphics &eg)
	{
		if (!eg.created)
			return true;
		glBindVertexArray(eg.vao);

		glm::mat4 model;
		shared->physics.entityPhysics[e].body->getWorldTransform()
			.getOpenGLMatrix(&model[0][0]);
		glm::mat4 const &finalTransform = projview * model;
		glUniformMatrix4fv(uniforms.mvp, 1, false, &finalTransform[0][0]);
		glUniformMatrix4fv(uniforms.model, 1, false, &model[0][0]);

		glBindTexture(GL_TEXTURE_2D, eg.tbo);
		glDrawArrays(GL_TRIANGLES, 0, eg.vertCount);
		return true;
	});
	checkGLError("render");
}

template <typename Shared>
inline bool Module<Shared>::buildChunk(ivec3_c &c)
{
	if (!vertexBufFlushed.load())
		return false;
	// set data
	int chunkVertexCount = 0;

	vertexBufLock.lock();
	shared->blockWriteLock.lock(c);
	shared->moveLock.lock();

	shared->blockTypes.iterateInChunk(c, [&] (ivec3::cref b, BlockType const &blockType)
	{
		if (blockFuncs.isBlockVisible(b))
		{
			auto neighbor = [&] (int xoff, int yoff, int zoff)
				{
					bool const neighborValid = shared->blockTypes
						.isValidBlockCoord(b + ivec3(xoff, yoff, zoff));

					bool const neighborVisible = neighborValid
						? blockFuncs.isBlockVisible(b + ivec3(xoff, yoff, zoff))
						: false;

					chunkVertexCount+= neighborVisible ? 0 : 6;
				};
			neighbor(-1, 0, 0);
			neighbor(+1, 0, 0);
			neighbor(0, -1, 0);
			neighbor(0, +1, 0);
			neighbor(0, 0, -1);
			neighbor(0, 0, +1);
		}
		return true;
	});

	vertexBuf.resize(chunkVertexCount);
	int currChunkVertex = 0;
	shared->blockTypes.iterateInChunk(c, [&] (ivec3::cref b, BlockType const &blockType)
	{
		if (blockFuncs.isBlockVisible(b))
		{
			auto addPos = [&] (std::initializer_list<VertexComponent> components)
				{
					auto const t = blockFuncs.getBlockTexCoords(b);
					auto const t1 = t.first, t2 = t.second;
					auto it = components.begin();
					while (it != components.end())
					{
						vertexBuf[currChunkVertex].x = (b.x-c.x*Shared::CSIZE_X) + *(it++);
						vertexBuf[currChunkVertex].y = (b.y-c.y*Shared::CSIZE_Y) + *(it++);
						vertexBuf[currChunkVertex].z = (b.z-c.z*Shared::CSIZE_Z) + *(it++);
						vertexBuf[currChunkVertex].u = t1.x + *(it++) * t2.x;
						vertexBuf[currChunkVertex].v = t1.y + *(it++) * t2.y;
						vertexBuf[currChunkVertex].nx = *(it++);
						vertexBuf[currChunkVertex].ny = *(it++);
						vertexBuf[currChunkVertex].nz = *(it++);
						++currChunkVertex;
					}
				};

			auto neighbor = [&] (int xoff, int yoff, int zoff)
				{
					bool const neighborValid = shared->blockTypes
						.isValidBlockCoord(b + ivec3(xoff, yoff, zoff));

					bool const neighborVisible = neighborValid
						? blockFuncs.isBlockVisible(b + ivec3(xoff, yoff, zoff))
						: false;

					if (!neighborVisible)
					{
						if (xoff != 0)
						{
							// from -1 or 1 to 0 or 1
							VertexComponent const off = (xoff + 1) / 2;
							if (xoff == -1)
								addPos({
									off, 0, 0, 0, 0, -1, 0, 0,
									off, 1, 1, 1, 1, -1, 0, 0,
									off, 1, 0, 1, 0, -1, 0, 0,

									off, 0, 0, 0, 0, -1, 0, 0,
									off, 0, 1, 0, 1, -1, 0, 0,
									off, 1, 1, 1, 1, -1, 0, 0,
								});
							else
								addPos({
									off, 1, 1, 1, 1, 1, 0, 0,
									off, 0, 0, 0, 0, 1, 0, 0,
									off, 1, 0, 1, 0, 1, 0, 0,

									off, 0, 1, 0, 1, 1, 0, 0,
									off, 0, 0, 0, 0, 1, 0, 0,
									off, 1, 1, 1, 1, 1, 0, 0,
								});
						}

						else if (yoff != 0)
						{
							VertexComponent const off = (yoff + 1) / 2;
							if (yoff == 1)
								addPos({
									0, off, 0, 0, 0, 0, 1, 0,
									1, off, 1, 1, 1, 0, 1, 0,
									1, off, 0, 1, 0, 0, 1, 0,

									0, off, 0, 0, 0, 0, 1, 0,
									0, off, 1, 0, 1, 0, 1, 0,
									1, off, 1, 1, 1, 0, 1, 0,
								});
							else
								addPos({
									1, off, 1, 1, 1, 0, -1, 0,
									0, off, 0, 0, 0, 0, -1, 0,
									1, off, 0, 1, 0, 0, -1, 0,
                                                        -
									0, off, 1, 0, 1, 0, -1, 0,
									0, off, 0, 0, 0, 0, -1, 0,
									1, off, 1, 1, 1, 0, -1, 0,
								});
						}

						else if (zoff != 0)
						{
							VertexComponent const off = (zoff + 1) / 2;
							if (zoff == 1)
								addPos({
									0, 0, off, 0, 0, 0, 0, 1,
									1, 0, off, 1, 0, 0, 0, 1,
									1, 1, off, 1, 1, 0, 0, 1,

									0, 1, off, 0, 1, 0, 0, 1,
									0, 0, off, 0, 0, 0, 0, 1,
									1, 1, off, 1, 1, 0, 0, 1,
								});
							else
								addPos({
									1, 0, off, 1, 0, 0, 0, -1,
									0, 0, off, 0, 0, 0, 0, -1,
									1, 1, off, 1, 1, 0, 0, -1,
                                                           -
									0, 0, off, 0, 0, 0, 0, -1,
									0, 1, off, 0, 1, 0, 0, -1,
									1, 1, off, 1, 1, 0, 0, -1,
								});
						}
					}
				};
			neighbor(-1, 0, 0);
			neighbor(+1, 0, 0);
			neighbor(0, -1, 0);
			neighbor(0, +1, 0);
			neighbor(0, 0, -1);
			neighbor(0, 0, +1);
		}
		return true;
	});

	vertexBufFlushed = false;
	vertexBufChunk = c;
	chunkGraphics.chunkAt(c).dirty = false;

	shared->moveLock.unlock();
	shared->blockWriteLock.unlock(c);
	vertexBufLock.unlock();

	if (currChunkVertex != chunkVertexCount)
	{
		std::stringstream ss;
		ss << "Asynchronous measurement and acquirement of chunk vertices!\n"
		   << "Measured " << chunkVertexCount
		   << ", acquired " << currChunkVertex << " vertices.";
		Log::error(ss);
		exit(EXIT_FAILURE);
	}
	return true;
}

}

}

#endif /* GRAPHICS_HPP_ */
