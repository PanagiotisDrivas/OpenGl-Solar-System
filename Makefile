CXX := g++

# Directories
SRC_DIR := ./src
STB_DIR := ./stb
HELP_DIR := $(SRC_DIR)/helpingFunctions

# Source files
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp) \
			 $(wildcard $(SRC_DIR)/*.c) \
             $(wildcard $(SRC_DIR)/helpingFunctions/*.cpp) \
             $(wildcard $(STB_DIR)/*.cpp)

# Include directories
INC_DIRS := -I$(SRC_DIR) -I$(STB_DIR)

# Compiler flags
CXXFLAGS := -std=c++11 -Winvalid-pch

# Linker flags
LDFLAGS := -lGL -lGLU -lglut -lGLEW -lglfw -lX11 -lXxf86vm -lXrandr -lpthread -lXi -ldl -lXinerama -lXcursor -lassimp

# Output binary
TARGET := solar_system

all: $(TARGET)

$(TARGET): $(SRC_FILES)
	$(CXX) $(CXXFLAGS) $(INC_DIRS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f $(TARGET)