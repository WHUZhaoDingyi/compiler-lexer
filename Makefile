CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2 -g
TARGET = toyc_lexer
SOURCES = main.cpp lexer.cpp
HEADERS = lexer.h
OBJECTS = $(SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET)

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

debug: CXXFLAGS += -DDEBUG
debug: $(TARGET)

.PHONY: all clean debug
