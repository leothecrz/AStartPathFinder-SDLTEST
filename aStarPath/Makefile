
all: Game.o Grid.o
	gcc src/Game.o src/Grid.o  -lSDL2 -lstdc++ -lm -g -o RUN 

Game.o:	src/Game.cpp
	gcc src/Game.cpp -c -o src/Game.o -Wall

Grid.o: src/Grid.cpp
	gcc src/Grid.cpp -c -o src/Grid.o -Wall