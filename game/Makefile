CXX=g++
CXX_FLAGS=-Og -g -Wall -Wextra -Wpedantic
HEADERS=rawio.h enemy.h player.h basics.h

all: sim_game

%.o: %.cpp ${HEADERS}
	${CXX} ${CXX_FLAGS} -c -o $@ $<

sim_game: simulated_io.o game.o
	${CXX} ${LD_FLAGS} -o $@ $^

clean:
	rm sim_game *.o || true
