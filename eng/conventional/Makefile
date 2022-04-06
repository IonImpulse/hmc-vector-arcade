CXX=g++
CXX_FLAGS=-Og -g -Wall -std=c++17 -Wpedantic

%.o: %.cpp ${HEADERS}
	${CXX} ${CXX_FLAGS} -c -o $@ $<


test: main.o util.o
	g++ -std=c++17 -o test $^

render: render.o simulated_io.o util.o game.o
	g++ -std=c++17 -o render $^

clean:
	rm render test
