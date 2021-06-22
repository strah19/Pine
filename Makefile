PINE_SRC = $(wildcard src/*.c)

CC = gcc

COMPILER_FLAGS = -Werror -Wfloat-conversion -ggdb -g 

LINKER_FLAGS = 

ifeq ($(OS), Windows_NT)
	LINKER_FLAGS = -lmingw32 
else
	LINKER_FLAGS = -lm 
endif

PINE_EXEC_NAME = Pine

all : pine

pine: $(PINE_SRC) 
	 $(CC) $(PINE_SRC) $(INCLUDE_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(PINE_EXEC_NAME) 