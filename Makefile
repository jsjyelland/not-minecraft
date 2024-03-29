CC = g++

CPPFLAGS = -Iinclude -W -std=c++17

SRC = $(wildcard src/*.cpp)
OBJ = $(patsubst src/%.cpp, build/%.o, $(SRC))

OBJDIR = ./build
BIN = ./bin

ifeq ($(OS), Windows_NT)
	CPPFLAGS += -I "C:\Program Files\mingw-w64\x86_64-8.1.0-posix-seh-rt_v6-rev0\mingw64\x86_64-w64-mingw32\include"
	LDFLAGS = -lglfw3 -lopengl32 -lglu32 -lgdi32
	TARGET = $(BIN)/notminecraft.exe
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S), Darwin)
		LDFLAGS = -lglfw -lGLEW -framework OpenGL
		TARGET = $(BIN)/notminecraft
	endif
endif

$(OBJDIR):
	mkdir $@

$(BIN):
	mkdir $@

build/%.o: src/%.cpp | $(OBJDIR)
	$(CC) -c $(CPPFLAGS) -o $@ $^

all: $(TARGET)

$(TARGET): $(OBJ) | $(BIN)
	$(CC) $^ -o $@ $(LDFLAGS)

.PHONY: clean

clean:
	rm -rf $(TARGET) $(OBJ) $(wildcard **/*.o) $(BIN) $(OBJDIR)

run: $(TARGET)
	./$(TARGET)