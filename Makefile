CXX = g++
CXXFLAGS = -Wall -std=c++17 -IHeaders
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system
SRC = $(wildcard Source/*.cpp) Source/GUI/coupGUI.cpp
OUT = coupGUI

all: $(OUT)

$(OUT): $(SRC)
	$(CXX) $(SRC) $(CXXFLAGS) $(LDFLAGS) -o $(OUT)

run: $(OUT)
	@export DISPLAY=$(shell grep nameserver /etc/resolv.conf | awk '{print $$2}'):0 && \
	export LIBGL_ALWAYS_INDIRECT=1 && \
	./$(OUT)

clean:
	rm -f $(OUT)
