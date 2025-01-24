# Set the compiler
CC = gcc

# Set the flags for the compiler
CFLAGS = -Iglad/include -g

# Set the libraries to link against
LIBS = -lglfw -lm -ldl -lcglm

# Set the source files
SRC = src/main.c glad/src/glad.c

# Set the output file
OUT = triangle_shader.out

# Default rule to compile and run the program
all: $(OUT)
	./$(OUT)

# Rule to compile the program
$(OUT): $(SRC)
	$(CC) -o $(OUT) $(SRC) $(CFLAGS) $(LIBS)

# Clean rule to remove the compiled files
clean:
	rm -f $(OUT)

# Phony targets
.PHONY: all clean
