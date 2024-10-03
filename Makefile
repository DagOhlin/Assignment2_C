# Define the compiler
CC = gcc

# Define compiler flags
CFLAGS = -Wall -g -pthread

# Define the target executable
TARGET = memoryManager

# Define the source files
SRCS = memory_manager.c

# Define the object files
OBJS = $(SRCS:.c=.o)

# The default target that will be built when "make" is run
all: $(TARGET)

# Rule to compile the target executable from the object files
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Rule to compile .c files into .o files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up the build (remove object files and executable)
clean:
	rm -f $(OBJS) $(TARGET)

# Run the program
run: $(TARGET)
	./$(TARGET)

# Run the program
runt: $(TARGET)
	time ./$(TARGET)
