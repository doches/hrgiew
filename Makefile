# Makefile for lda-c and assorted tools (int->int hash, string->int hash, sparsecount, etc.)

CC = g++

# -finstrument-functions -lSaturn -m32 #Saturn profiler on OS X
DEBUG_FLAGS = 

OPTIMIZATION_FLAGS = -O3

UNAME := $(shell uname)
ifeq ($(UNAME),Linux)
CFLAGS = -Wall -I lib/ -I vendor/include/ -I tools/ -I test/ $(DEBUG_FLAGS) $(OPTIMIZATION_FLAGS)
endif
ifeq ($(UNAME),Darwin)
CFLAGS = -Wall -I lib/ -I vendor/include/ -I tools/ -I test/ $(DEBUG_FLAGS) $(OPTIMIZATION_FLAGS)
endif

LFLAGS = -lm -lgsl -lgslcblas $(DEBUG_FLAGS)
EXECUTABLES = tests

LIB := ${wildcard lib/*.cpp}
LIB_OBJ := ${foreach src,${LIB},${subst .cpp,.o, ${lastword ${subst /, ,${src}}}}}

VENDOR := ${wildcard vendor/lib/*.cpp}
VENDOR_OBJ := ${foreach src,${VENDOR},${subst .cpp,.o, ${lastword ${subst /, ,${src}}}}}

DEMO := ${wildcard demo/*.cpp}
DEMO_OBJ := ${foreach src,${DEMO},${subst .cpp,.o, ${lastword ${subst /, ,${src}}}}}
DEMO_BIN := ${foreach src,${DEMO},${subst .cpp,, ${lastword ${subst /, ,${src}}}}}

EXECUTABLES := ${EXECUTABLES} ${DEMO_BIN}

all: ${DEMO_BIN} ${LIB_OBJ} ${VENDOR_OBJ}

test: test.o ${LIB_OBJ} ${VENDOR_OBJ}
	$(CC) $(CFLAGS) test.o $(LIB_OBJ) ${VENDOR_OBJ} -o tests && ./tests; rm test.o

test.o: test/test.cpp test/test.h
	$(CC) -c $(CFLAGS) test/test.cpp

test_graph.o: test/test_graph.* lib/graph.*
	$(CC) -c $(CFLAGS) test/test_graph.cpp

progressbar.o: vendor/lib/progressbar.cpp vendor/include/progressbar.h
	$(CC) -c $(CFLAGS) vendor/lib/progressbar.cpp

consensus.o: lib/consensus.h lib/consensus.cpp
	$(CC) -c $(CFLAGS) lib/consensus.cpp

statusbar.o: vendor/lib/statusbar.cpp vendor/include/statusbar.h
	$(CC) -c $(CFLAGS) vendor/lib/statusbar.cpp

logger.o: vendor/lib/logger.cpp vendor/include/logger.h
	$(CC) -c $(CFLAGS) vendor/lib/logger.cpp
	
graph.o: lib/graph.h lib/graph.cpp
	$(CC) -c $(CFLAGS) lib/graph.cpp

dendrogram.o: lib/dendrogram.h lib/dendrogram.cpp graph.o dendrogram_node.o
	$(CC) -c $(CFLAGS) lib/dendrogram.cpp

dendrogram_node.o: lib/dendrogram_node.h lib/dendrogram_node.cpp graph.o
	$(CC) -c $(CFLAGS) lib/dendrogram_node.cpp

demo: ${VENDOR_OBJ} ${DEMO_BIN}

figure_4: $(LIB_OBJ) ${VENDOR_OBJ} demo/figure_4.cpp
	$(CC) $(CFLAGS) demo/figure_4.cpp ${LIB_OBJ} ${VENDOR_OBJ} -o figure_4

figure_5: $(LIB_OBJ) ${VENDOR_OBJ} demo/figure_5.cpp
	$(CC) $(CFLAGS) demo/figure_5.cpp ${LIB_OBJ} ${VENDOR_OBJ} -o figure_5

learner: corpus.o $(LIB_OBJ) $(VENDOR_OBJ) demo/learner.cpp
	$(CC) $(CFLAGS) $(LIB_OBJ) $(VENDOR_OBJ) demo/learner.cpp -o learner

corpus.o: lib/corpus.h lib/corpus.cpp
	$(CC) -c $(CFLAGS) lib/corpus.cpp

graphify: $(LIB_OBJ) $(VENDOR_OBJ) demo/graphify.cpp
	$(CC) $(CFLAGS) demo/graphify.cpp $(LIB_OBJ) $(VENDOR_OBJ) -o graphify
	
distance.o: lib/distance.h lib/distance.cpp lib/corpus.h lib/graph.h
	$(CC) -c $(CFLAGS) lib/distance.cpp

hierize: $(LIB_OBJ) $(VENDOR_OBJ) demo/hierize.cpp
	$(CC) $(CFLAGS) demo/hierize.cpp $(LIB_OBJ) $(VENDOR_OBJ) -o hierize

resample: $(LIB_OBJ) $(VENDOR_OBJ) demo/resample.cpp
	$(CC) $(CFLAGS) demo/resample.cpp $(LIB_OBJ) $(VENDOR_OBJ) -o resample

consensus: $(LIB_OBJ) $(VENDOR_OBJ) demo/consensus.cpp
	$(CC) $(CFLAGS) demo/consensus.cpp $(LIB_OBJ) $(VENDOR_OBJ) -o consensus

dendrify: $(LIB_OBJ) $(VENDOR_OBJ) demo/dendrify.cpp
	$(CC) $(CFLAGS) demo/dendrify.cpp $(LIB_OBJ) $(VENDOR_OBJ) -o dendrify

dendrogram2dot: $(LIB_OBJ) $(VENDOR_OBJ) demo/dendrogram2dot.cpp
	$(CC) $(CFLAGS) demo/dendrogram2dot.cpp $(LIB_OBJ) $(VENDOR_OBJ) -o dendrogram2dot

.PHONY: clean all debug demo doc 

debug:
	@echo "LIB_OBJ: " $(LIB_OBJ)
	@echo "TEST_OBJ: " $(TEST_OBJ)
	@echo "DEMO_OBJ: " $(DEMO_OBJ)
	@echo "DEMO_BIN: " $(DEMO_BIN)
	@echo "VENDOR: " ${VENDOR}
	@echo "VENDOR_OBJ: " ${VENDOR_OBJ}

# Clean target
clean:
	rm -f *.o
	rm -rf doc/
	rm -f $(EXECUTABLES)
	rm -f **/*~
	rm -f *~
	rm -rf *.dSYM
	rm corpus.length

doc:
	mkdir -p doc/html/
	dot -Tpng demo/system.dot -o doc/html/system.png
	doxygen Doxyfile
