CXX=g++
CXX_FLAGS=-Og -g -Wall -Wextra -Wpedantic
LD_FLAGS=-lSDL2
HEADERS=include/rawio.h lib/visuals/visuals.hpp

all: sim_game sprite_creator

bin/visuals.o: lib/visuals/* include/rawio.h
	${CXX} ${CXX_FLAGS} -c lib/visuals/visuals.cpp -o $@

bin/simulated_io.o: src/simulated_io.cpp include/rawio.h
	${CXX} ${CXX_FLAGS} -c src/simulated_io.cpp -o $@

bin/game.o: src/game.cpp ${HEADERS}
	${CXX} ${CXX_FLAGS} -c src/game.cpp -o $@

sim_game: bin/game.o bin/simulated_io.o bin/visuals.o
	${CXX} $^ ${LD_FLAGS} -o $@

bin/sprite_creator.o: src/sprite_creator.cpp ${HEADERS}
	${CXX} ${CXX_FLAGS} -c src/sprite_creator.cpp -o $@

sprite_creator: bin/sprite_creator.o bin/simulated_io.o bin/visuals.o
	${CXX} $^ ${LD_FLAGS} -o $@

clean:
	rm sim_game sprite_creator bin/*.o || true
