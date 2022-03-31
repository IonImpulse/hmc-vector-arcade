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
<<<<<<< HEAD

=======
>>>>>>> 0621db3749b850e22eb50aa7b5187f7a3cae5da3
