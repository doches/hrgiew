# Makefile for lda-c and assorted tools (int->int hash, string->int hash, sparsecount, etc.)

CC = g++
OPTIMIZATION_FLAGS = -O3 
DEBUG_FLAGS = 

UNAME := $(shell uname)
ifeq ($(UNAME),Linux)
CFLAGS = -Wall -I lib/ -I vendor/include/ -I tools/ -I test/ -Werror $(DEBUG_FLAGS) $(OPTIMIZATION_FLAGS)
endif
ifeq ($(UNAME),Darwin)
CFLAGS = -Wall -I lib/ -I vendor/include/ -I tools/ -I test/ -Werror $(DEBUG_FLAGS) $(OPTIMIZATION_FLAGS)
endif

LFLAGS = -lm -lgsl -lgslcblas $(DEBUG_FLAGS)
EXECUTABLES = tests

LIB := ${wildcard lib/*.cpp}
LIB_OBJ := ${foreach src,${LIB},${subst .cpp,.o, ${lastword ${subst /, ,${src}}}}}

test: test.o ${LIB_OBJ} logger.o
	$(CC) $(CFLAGS) test.o $(LIB_OBJ) logger.o -o tests && ./tests

test.o: test/test.cpp test/test.h
	$(CC) -c $(CFLAGS) test/test.cpp

testGraph.o: test/testGraph.* lib/graph.*
	$(CC) -c $(CFLAGS) test/testGraph.cpp

progressbar.o: vendor/lib/progressbar.cpp vendor/include/progressbar.h
	$(CC) -c $(CFLAGS) vendor/lib/progressbar.cpp

logger.o: vendor/lib/logger.cpp vendor/include/logger.h
	$(CC) -c $(CFLAGS) vendor/lib/logger.cpp
	
graph.o: lib/graph.h lib/graph.cpp
	$(CC) -c $(CFLAGS) lib/graph.cpp

.PHONY: clean doc all debug

debug:
	echo $(LIB_OBJ)
	echo $(TEST_OBJ)

# Clean target
clean:
	rm -f *.o
	rm -rf doc/
	rm -f $(EXECUTABLES)
	rm -f **/*~
	rm -f *~

# Doc target
doc: lib/*.h tools/*.h
	rm -rf doc/
	crud

all: $(EXECUTABLES) doc
