# =============================================================================
# PvP-DDR Makefile
# =============================================================================
# Builds the SFML version of the game.
#
# Targets:
#   make          (default) build the SFML game -> ./game
#   make run      build and immediately run
#   make clean    remove build artifacts
#
# Requirements (Windows / MSYS2):
#   pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-sfml
#
# Requirements (Linux):
#   sudo apt install g++ libsfml-dev    (Debian/Ubuntu)
#
# Requirements (Mac, with Homebrew):
#   brew install sfml@2
# =============================================================================

CXX       := g++
CXXFLAGS  := -std=c++17 -O2 -Wall -Wextra -Wpedantic
LDLIBS    := -lsfml-graphics -lsfml-window -lsfml-system

# On Windows / MSYS2, the executable needs the .exe extension. Detect the OS
# and adjust both the binary name and any platform-specific flags.
ifeq ($(OS),Windows_NT)
    TARGET := game.exe
    # If you installed SFML to a non-standard prefix (e.g. C:/SFML),
    # uncomment and edit these:
    # CXXFLAGS += -IC:/SFML/include
    # LDFLAGS  += -LC:/SFML/lib
else
    TARGET := game
endif

SRCS := main.cpp \
        Arrow.cpp \
        ArrowString.cpp \
        PlayerState.cpp \
        GameState.cpp \
        Renderer.cpp

OBJS := $(SRCS:.cpp=.o)

# -----------------------------------------------------------------------------
# Build rules
# -----------------------------------------------------------------------------

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS) $(LDLIBS)

# Generic .cpp -> .o pattern. Headers are listed as order-only prerequisites
# below so that touching a header rebuilds the right .o files.
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Header dependencies (manual; no auto-deps to keep the Makefile simple).
Arrow.o:        Arrow.h
ArrowString.o:  ArrowString.h Arrow.h
PlayerState.o:  PlayerState.h ArrowString.h Arrow.h
GameState.o:    GameState.h PlayerState.h ArrowString.h Arrow.h
Renderer.o:     Renderer.h GameState.h PlayerState.h ArrowString.h Arrow.h
main.o:         GameState.h Renderer.h PlayerState.h ArrowString.h Arrow.h

run: $(TARGET)
	./$(TARGET)

clean:
ifeq ($(OS),Windows_NT)
	-del /Q $(OBJS) $(TARGET) 2>nul
else
	rm -f $(OBJS) $(TARGET)
endif
