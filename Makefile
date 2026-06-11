all: compile run

compile:
	g++ -std=c++11 main.cpp tiny_obj_loader.cc -o projet -framework OpenGL -lGLEW -lglfw

run:
	./projet