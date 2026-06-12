all: compile run

compile:
	g++ -std=c++11 main.cpp Utils.cpp tiny_obj_loader.cc -o bin/projet -framework OpenGL -lGLEW -lglfw

run:
	./bin/projet

clean: 
	rm -f ./bin/*