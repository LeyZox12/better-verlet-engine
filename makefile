CXX = g++
SFML_DIR = ../../../SFML-3.0.0
TRACY_DIR = ../../../tracy-0.13.1/public

CXXFLAGS = -std=c++23 -g -fPIC -DPOINTENGINE_EXPORTS \
	-Iinclude \
	-I$(SFML_DIR)/include \
	-I$(TRACY_DIR) \
	-DTRACY_ENABLE

LDFLAGS = -L$(SFML_DIR)/lib
LDLIBS = \
	-lsfml-graphics-d \
	-lsfml-window-d \
	-lsfml-system-d \
	-lws2_32 \
	-ldbghelp

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