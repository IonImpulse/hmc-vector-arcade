CXX=g++
CXX_FLAGS=-Og -g -Wall -Wextra -Wpedantic
LD_FLAGS=-lSDL2
HEADERS=include/rawio.h include/enemy.h include/player.h include/basics.h include/projectile.h include/player.h include/render.h

all: sim_game

bin/%.o: src/%.cpp ${HEADERS}
	${CXX} ${CXX_FLAGS} -c -o $@ $<

sim_game: bin/simulated_io.o bin/game.o bin/enemy.o bin/basics.o bin/projectile.o bin/player.o bin/scene.o bin/render.o
	${CXX} $^ ${LD_FLAGS} -o $@

clean:
	rm sim_game bin/*.o || true
