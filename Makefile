HOMEBREW_PREFIX = $(shell brew --prefix 2>/dev/null || echo "/usr/local")

CC = g++
CFLAGS = -std=c++11 -I$(HOMEBREW_PREFIX)/include
LDFLAGS = -L$(HOMEBREW_PREFIX)/lib -framework OpenGL -lGLEW -lglfw

all: compile run

compile:
	@mkdir -p bin
	$(CC) $(CFLAGS) -I. main.cpp Utils.cpp Navigation.cpp Maths.cpp common/GLShader.cpp common/tiny_obj_loader.cc imgui/imgui.cpp imgui/imgui_draw.cpp imgui/imgui_widgets.cpp imgui/imgui_tables.cpp imgui/imgui_impl_glfw.cpp imgui/imgui_impl_opengl3.cpp -o bin/projet $(LDFLAGS)

run:
	./bin/projet

clean: 
	rm -f ./bin/*