PINE_SRC = $(filter-out src/vm.c, $(wildcard src/*.c))

CC = gcc

COMPILER_FLAGS = -Werror -Wfloat-conversion -ggdb -g 

LINKER_FLAGS = 

ifeq ($(OS), Windows_NT)
	LINKER_FLAGS = -lmingw32 
else
	LINKER_FLAGS = -lm 
endif

PINE_EXEC_NAME = Pine

all : pine pvm

pine: $(PINE_SRC) 
	 $(CC) $(PINE_SRC) $(INCLUDE_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(PINE_EXEC_NAME) 

PVM_SRC = src/vm.c
PVM_EXEC_NAME = PVM

pvm:$(PVM_SRC) 
	 $(CC) $(PVM_SRC) $(INCLUDE_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(PVM_EXEC_NAME) 
