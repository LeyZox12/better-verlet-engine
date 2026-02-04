CXX = g++

SFML_DIR = ../../../SFML-3.0.0

CXXFLAGS = -std=c++23 -g -fPIC -DPOINTENGINE_EXPORTS \
	-Iinclude \
	-I$(SFML_DIR)/include

LDFLAGS = -L$(SFML_DIR)/lib

LDLIBS = \
	-lsfml-graphics-d \
	-lsfml-window-d \
	-lsfml-system-d

SRC = $(wildcard src/*.cpp)
OBJ = $(SRC:.cpp=.o)

TARGET = PointEngine.dll
IMPORT = lib/libPointEngine.a

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) -shared $(OBJ) -o $(TARGET) \
		-Wl,--out-implib,$(IMPORT) \
		$(LDFLAGS) $(LDLIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(TARGET) $(IMPORT)

.PHONY: all clean
