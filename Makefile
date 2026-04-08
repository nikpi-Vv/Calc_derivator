CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude -Iexternal
BUILD_DIR = build

APP = $(BUILD_DIR)/app.exe
TESTS = $(BUILD_DIR)/tests.exe

APP_SOURCES = \
	src/main.cpp \
	src/app.cpp \
	src/differentiator.cpp \
	src/evaluator.cpp \
	src/lexer.cpp \
	src/parser.cpp \
	src/printer.cpp \
	src/simplifier.cpp

TEST_SOURCES = \
	tests/test_main.cpp \
	tests/test_app.cpp \
	tests/test_contest.cpp \
	tests/test_hard.cpp \
	src/app.cpp \
	src/differentiator.cpp \
	src/evaluator.cpp \
	src/lexer.cpp \
	src/parser.cpp \
	src/printer.cpp \
	src/simplifier.cpp

.PHONY: all test run clean

all: $(APP)

$(BUILD_DIR):
	if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)

$(APP): $(APP_SOURCES) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -o $(APP) $(APP_SOURCES)

$(TESTS): $(TEST_SOURCES) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -o $(TESTS) $(TEST_SOURCES)

run: $(APP)
	$(APP)

test: $(TESTS)
	$(TESTS)

clean:
	if exist $(BUILD_DIR) rmdir /s /q $(BUILD_DIR)