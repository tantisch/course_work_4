CC = g++
CFLAGS = -Wall -c -std=c++11
LDFLAGS = -lstdc++
EXECUTABLE = posit.exe

# Define source and object file paths
SRC_DIR = src
OBJ_DIR = obj
POSIT_DIR = $(SRC_DIR)/POSIT
TESTS_DIR = $(SRC_DIR)/TESTS

# List all source files
POSIT_SRCS = $(wildcard $(POSIT_DIR)/*.cpp)
TESTS_SRCS = $(wildcard $(TESTS_DIR)/*.cpp)
MAIN_SRCS = $(wildcard $(SRC_DIR)/*.cpp)

# Define object file nasize_exp
POSIT_OBJS = $(patsubst $(POSIT_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(POSIT_SRCS))
TESTS_OBJS = $(patsubst $(TESTS_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(TESTS_SRCS))
MAIN_OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(MAIN_SRCS))

# Default target
all: $(EXECUTABLE)

# Link object files into executable
$(EXECUTABLE): $(POSIT_OBJS) $(TESTS_OBJS) $(MAIN_OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

# create OBJ_DIR if it is absent
$(shell mkdir -p $(OBJ_DIR))

# Compile TESTS source files
$(OBJ_DIR)/%.o: $(TESTS_DIR)/%.cpp
	$(CC) $(CFLAGS) $< -o $@

# Compile POSIT source files
$(OBJ_DIR)/posit.o: $(POSIT_DIR)/posit.cpp $(POSIT_DIR)/posit.h $(POSIT_DIR)/operations.h
	$(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/operations.o: $(POSIT_DIR)/operations.cpp $(POSIT_DIR)/operations.h
	$(CC) $(CFLAGS) $< -o $@


# Compile main source file
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) $< -o $@

# Clean up object files and executable
clean:
	rm -f $(OBJ_DIR)/* $(EXECUTABLE)
