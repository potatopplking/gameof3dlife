all:
	gcc main.cpp simulation.cpp ui.cpp -lSDL2 -lGLEW -lGL -lstdc++ -lGLU -lm -ggdb3
