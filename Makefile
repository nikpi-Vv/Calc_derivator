SHELL := cmd.exe
.SHELLFLAGS := /C

CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Iinclude

SRC := $(wildcard src/*.cpp)
TARGET := build/app.exe

.PHONY: all run clean rebuild dirs

all: dirs $(TARGET)

dirs:
	if not exist build mkdir build

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

run: all
	$(TARGET)

clean:
	if exist build rmdir /S /Q build

rebuild: clean all