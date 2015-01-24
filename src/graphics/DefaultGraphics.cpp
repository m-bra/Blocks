#include "precompiled.hpp"

#include "graphics/DefaultGraphics.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "CImg.h"
using namespace cimg_library;

#include "PhysicsProvider.hpp"

#include "logic/DefaultLogic.hpp"

namespace blocks
{

namespace graphics
{

bool DefaultGraphics::canMoveArea()
{
	return vertexBufFlushed;
}

void DefaultGraphics::moveArea(ivec3_c &m)
{
	auto createChunk = [&] (ivec3_c &c) {chunkGraphics[c].create();};
	auto destroyChunk = [&] (ivec3_c &c) {chunkGraphics[c].destroy();};
	chunkGraphics.shift(-m, createChunk, destroyChunk);
}

void DefaultGraphics::onChunkChange(ivec3_c &c)
{
	chunkGraphics[c].dirty = true;
}

void DefaultGraphics::checkGLError(std::string msg)
{
	gll::onOpenGLErr([&] (GLenum err, GLubyte const *errstr)
	{
		LOG_ERR("OpenGL error ", err, " (", msg, "): ", errstr);
		exit(EXIT_FAILURE);
	});
}

void DefaultGraphics::onRegister()
{
	logic = world->getFirstModuleByType<logic::DefaultLogic>();

	chunkTransforms.create(world->ccount);
	chunkGraphics.create(world->ccount);

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

	// load chunk texture from file
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
		LOG_FATAL("Cannot load chunk.png: 3 or 4 channels required, have ", chunkImg.spectrum());

	// push texture to gpu
	glGenTextures(1, &chunkTbo);
	glBindTexture(GL_TEXTURE_2D, chunkTbo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, chunkImg.width(), chunkImg.height(), 0,
		GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	delete[] pixels;

	chunkTransforms.iterate([&] (ivec3_c &c, glm::mat4 &trans)
	{
		trans = glm::translate(c.glm() * world->csize.glm());
	});

	chunkGraphics.iterate([&] (ivec3_c &c, ChunkGraphics &cg)
	{
		cg.create();
	});

    camDir = -fvec3::Y;
    camLeft = -fvec3::X;
    camUp = fvec3::Y;

	checkGLError("init");
}

void DefaultGraphics::onDeregister()
{
	chunkTransforms.destroy();
	chunkGraphics.iterate([&] (ivec3_c &c, ChunkGraphics &cg)
	{
		cg.destroy();
	});
	chunkGraphics.destroy();

	program.destroy();
	checkGLError("End");
}

void DefaultGraphics::setFrameBufSize(int x, int y)
{
	projection = glm::perspective<float>(glm::radians(60.), float(x) / y, .1, 1000);
	glViewport(0, 0, x, y);
}

void DefaultGraphics::onEntityCreate(Entity e, std::vector<BaseEntityArgs *> const &args)
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
}

void DefaultGraphics::onEntityDestroy(Entity e)
{
	EntityGraphics &eg = entityGraphics[e];
	assert(eg.created);

	eg.created = false;
	glDeleteBuffers(1, &eg.vbo);
	glDeleteVertexArrays(1, &eg.vao);
}

void DefaultGraphics::parallel(GameTime time)
{
	chunkGraphics.iterate([&] (ivec3_c &c, ChunkGraphics &graphics)
	{
		if (graphics.dirty)
			buildChunk(c);
	});
}

void DefaultGraphics::onUpdate(GameTime time)
{
	if (!vertexBufFlushed.load())
		if (vertexBufLock.try_lock())
		{
			GLuint vbo = chunkGraphics[vertexBufChunk].vbo;
			GLuint vao = chunkGraphics[vertexBufChunk].vao;

			chunkGraphics[vertexBufChunk].vertCount = vertexBuf.size();

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

	if (loading)
	{
		bool done = !logic->loading;
		chunkGraphics.iterate([&] (ivec3_c &c, ChunkGraphics &cg)
		{
			if (cg.dirty) done = false;
		});
		if (done)
			setDoneLoading();
	}
}

void DefaultGraphics::render()
{
	if (world->loading)
	{
		glClearColor(.1, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		return;
	}

	program.bind();

	fvec3 eyePos = world->getEntityPos(world->playerEntity) + world->entityEyePos[world->playerEntity];
	glUniform3f(uniforms.eyePosXYZ, eyePos.x, eyePos.y, eyePos.z);
	glUniform3f(uniforms.lightXYZ,  eyePos.x, eyePos.y + 200, eyePos.z);

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, chunkTbo);

	projview = projection * glm::lookAt(eyePos.glm(),
		eyePos.glm() + camDir.glm(), camUp.glm());

	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//static double const chunkDiameter = sqrt(World::CSIZE_X*World::CSIZE_X+World::CSIZE_Y*World::CSIZE_Y+World::CSIZE_Z*World::CSIZE_Z);

	glBindTexture(GL_TEXTURE_2D, chunkTbo);

	bool printed = false;
	chunkGraphics.iterate([&] (ivec3_c &c, ChunkGraphics &cg)
	{
		glBindVertexArray(cg.vao);
		glm::mat4 const &model = chunkTransforms[c];
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
	});

	entityGraphics.iterate([&] (Entity e, EntityGraphics &eg)
	{
		if (!eg.created)
			return;
		glBindVertexArray(eg.vao);

		glm::mat4 model;
		world->physics->getEntityOpenGLMatrix(e, &model[0][0]);
		glm::mat4 const &finalTransform = projview * model;
		glUniformMatrix4fv(uniforms.mvp, 1, false, &finalTransform[0][0]);
		glUniformMatrix4fv(uniforms.model, 1, false, &model[0][0]);

		glBindTexture(GL_TEXTURE_2D, eg.tbo);
		glDrawArrays(GL_TRIANGLES, 0, eg.vertCount);
	});
	checkGLError("render");
}

bool DefaultGraphics::buildChunk(ivec3_c &c)
{
	if (!vertexBufFlushed.load())
		return false;
	// set data
	int chunkVertexCount = 0;

	vertexBufLock.lock();
	world->chunkWriteLocks[c].lock();
	world->moveLock.lock();

	world->blockTypes.iterateInChunk(c, [&] (ivec3::cref b, BlockType const &blockType)
	{
		if (blockFuncs.isBlockVisible(b))
		{
			auto neighbor = [&] (int xoff, int yoff, int zoff)
			{
				bool const neighborValid = world->blockTypes
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
	world->blockTypes.iterateInChunk(c, [&] (ivec3::cref b, BlockType const &blockType)
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
						vertexBuf[currChunkVertex].x = (b.x-c.x*world->csize.x) + *(it++);
						vertexBuf[currChunkVertex].y = (b.y-c.y*world->csize.y) + *(it++);
						vertexBuf[currChunkVertex].z = (b.z-c.z*world->csize.z) + *(it++);
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
					bool const neighborValid = world->blockTypes
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
	chunkGraphics[c].dirty = false;

	world->moveLock.unlock();
	world->chunkWriteLocks[c].unlock();
	vertexBufLock.unlock();

	if (currChunkVertex != chunkVertexCount)
	{
		LOG_ERR("Asynchronous measurement and acquirement of chunk vertices!\n",
		        "Measured ", chunkVertexCount, ", acquired ", currChunkVertex, " vertices.\n--UNDEFINED BEHAVIOUR IS FOLLOWING.--");
	}
	return true;
}

}

}
