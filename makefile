TARGET = a
C_COMPILER = gcc
C_COMPILER_FLAGS ?= 
SRCS = lab.c ginput.c
all:
	$(C_COMPILER) $(C_COMPILER_FLAGS) $(SRCS) -o $(TARGET)

clean:
	rm -f $(TARGET)
