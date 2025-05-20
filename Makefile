all:
	gcc src/main.cpp src/simulations/game_of_life_3D.cpp src/ui.cpp src/utilities.cpp -lSDL3 -lGLEW -lGL -lstdc++ -lGLU -lm -ggdb3 -Isrc -std=c++23 -o gameof3dlife

test:
	gcc src/utilities_test.cpp src/utilities.cpp -I src -lstdc++ -lm -ggdb3 -std=c++23 -o utilities_test
