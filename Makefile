all:
	gcc src/main.cpp src/simulation.cpp src/ui.cpp src/utilities.cpp -lSDL3 -lGLEW -lGL -lstdc++ -lGLU -lm -ggdb3 -o gameof3dlife

test:
	gcc src/utilities_test.cpp src/utilities.cpp -lstdc++ -lm -ggdb3 -std=c++17 -o utilities_test
