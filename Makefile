CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g

SRCS = main.cpp Arrow.cpp ArrowString.cpp PlayerState.cpp GameState.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = game_cli

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
