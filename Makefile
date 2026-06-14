HOMEBREW_PREFIX = $(shell brew --prefix 2>/dev/null || echo "/usr/local")

CC = g++
CFLAGS = -std=c++11 -I$(HOMEBREW_PREFIX)/include
LDFLAGS = -L$(HOMEBREW_PREFIX)/lib -framework OpenGL -lGLEW -lglfw

all: compile run

compile:
	@mkdir -p bin
	$(CC) $(CFLAGS) main.cpp Utils.cpp Navigation.cpp Maths.cpp common/GLShader.cpp tiny_obj_loader.cc -o bin/projet $(LDFLAGS)

run:
	./bin/projet

clean: 
	rm -f ./bin/*