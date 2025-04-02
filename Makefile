all:
	gcc src/main.cpp src/simulation.cpp src/ui.cpp src/utilities.cpp -lSDL3 -lGLEW -lGL -lstdc++ -lGLU -lm -ggdb3 -o gameof3dlife
