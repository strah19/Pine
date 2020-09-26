OBJS = src/*.c 
CC = gcc

ifeq ($(OS), Windows_NT)
	INCLUDE_PATHS = -IC:\MinGW\include\SDL2
	LIBRARY_PATHS = -LC:\MinGW\lib
endif 

COMPILER_FLAGS = -Werror -Wfloat-conversion -ggdb -g 

LINKER_FLAGS = 0

ifeq ($(OS), Windows_NT)
	LINKER_FLAGS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf
else
	LINKER_FLAGS = -lSDL2main -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -lm
endif

OBJ_NAME = Pine

all : $(OBJS)
	$(CC) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)