all:
	gcc main.cpp simulation.cpp ui.cpp -lSDL3 -lGLEW -lGL -lstdc++ -lGLU -lm -ggdb3
