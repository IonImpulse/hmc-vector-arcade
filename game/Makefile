CXX=g++
CXX_FLAGS=-Og -g
HEADERS=rawio.h

all: game

%.o: %.c ${HEADERS}
	${CXX} ${CXX_FLAGS} -c -o $@ $<

game: game.o
	g++ -o game game.o

clean:
	rm game
