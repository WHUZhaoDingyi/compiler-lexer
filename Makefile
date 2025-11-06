CXX := g++
CXXFLAGS := -std=gnu++17 -O2 -Wall -Wextra -Wno-sign-compare -Wno-unused-parameter

SRC := src/main.cpp src/lexer.cpp src/parser.cpp
OBJ := $(SRC:.cpp=.o)
BIN := your_compiler

all: $(BIN)

$(BIN): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

src/%.o: src/%.cpp src/%.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

src/main.o: src/main.cpp src/lexer.h src/parser.h src/tokens.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

.PHONY: clean
clean:
	rm -f $(OBJ) $(BIN)


