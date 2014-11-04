EXECUTABLE=blocks
MAKEFILE=Makefile
SOURCES=main.cpp
HEADERS=vec.hpp BlockFieldArray.hpp ChunkFieldArray.hpp EntityFieldArray.hpp OpenGL.h config.h Logger.hpp
HEADERS+=AppFuncs.hpp Shared.hpp SharedTypes.hpp
HEADERS+=logic/EntityFuncs.hpp logic/Module.hpp logic/Types.hpp
HEADERS+=physics/BlockFuncs.hpp physics/EntityFuncs.hpp physics/Module.hpp physics/Types.hpp
HEADERS+=graphics/BlockFuncs.hpp graphics/EntityFuncs.hpp graphics/Module.hpp graphics/Types.hpp
CFLAGS= -std=c++0x -I/media/data/Libraries/ubuntu-build/bullet3/src/
LDFLAGS=-lsfml-graphics -lsfml-window -lsfml-system /usr/local/lib/libGLLpp.a -lGLEW -lGL -lGLU -lBulletDynamics -lBulletCollision -lLinearMath -lpthread

OBJECTS_TMP=$(SOURCES:.cpp=.o)
OBJECTS=$(OBJECTS_TMP:.c=.o)

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	g++ -o$(EXECUTABLE) $(OBJECTS) $(LDFLAGS)
	
%.o: %.cpp $(HEADERS) $(MAKEFILE)
	g++ -o$@ -c $< $(CFLAGS)

clean:
	mkdir -p backup
	cp -tbackup $(OBJECTS) $(EXECUTABLE) $(SOURCES)
	rm -f $(OBJECTS)
	rm -f $(EXECUTABLE)
	
backup:
	mkdir -p backup
	cp -tbackup $(OBJECTS) $(EXECUTABLE) $(SOURCES)
