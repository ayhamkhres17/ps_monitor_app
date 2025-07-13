# Makefile for ps_monitor_app

CXX := g++
CXXFLAGS := -std=c++17 -pedantic-errors -Wall -Wextra -DNDEBUG -O3
LDFLAGS := -lsqlite3
INCLUDES := -Iinclude

SRC_DIR := src
OBJ_DIR := obj
BIN := ps_monitor_app

SRCS := $(SRC_DIR)/logger.cpp \
        $(SRC_DIR)/configure_info.cpp \
        $(SRC_DIR)/database.cpp \
        $(SRC_DIR)/os_ps_monitor.cpp \
        ps_monitor_app.cpp

OBJS := $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(SRCS))

# Default rule
all: $(BIN)

# Link
$(BIN): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) $(LDFLAGS) -o $@

# Compile
$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Clean rule
clean:
	rm -rf $(OBJ_DIR) $(BIN)

.PHONY: all clean