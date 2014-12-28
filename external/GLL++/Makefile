LIB=libGLLpp.a
SOURCES=GLL.cpp Program.cpp
HEADERS=GLL.hpp Program.hpp Buffer.hpp VertexArray.hpp
CFLAGS=-Wall -std=c++11 -g

OBJECTS_TMP=$(SOURCES:.cpp=.o)
OBJECTS=$(OBJECTS_TMP:.c=.o)

all: $(LIB)

$(LIB): $(OBJECTS)
	ar -crs $(LIB) $(OBJECTS)
	
.cpp.o: $(HEADERS)
	gcc -o$@ -c $< $(CFLAGS)

install: $(LIB)
	cp $(LIB) /usr/local/lib/
	mkdir -p /usr/local/include/GLL
	cp $(HEADERS) /usr/local/include/GLL

clean:
	mkdir -p ".backup"
	cp -t".backDup" $(OBJECTS) $(LIB) $(SOURCES)
	rm -f $(OBJECTS)
	rm -f $(EXECUTABLE)
	
backup:
	mkdir -p ".backup"
	cp -t".backup" $(OBJECTS) $(LIB) $(SOURCES)
