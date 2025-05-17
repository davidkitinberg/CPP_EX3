CXX = g++
CXXFLAGS = -Wall -std=c++17 -IHeaders
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system

# All logic files (no GUI)
LOGIC_SRC = Source/Game.cpp Source/Player.cpp Source/PlayerFactory.cpp \
            Source/Baron.cpp Source/Spy.cpp Source/Judge.cpp Source/Governor.cpp \
            Source/Merchant.cpp Source/General.cpp

# GUI includes coupGUI.cpp which has main()
GUI_SRC = $(LOGIC_SRC) Source/GUI/coupGUI.cpp

# Targets
run: coupGUI
	@export DISPLAY=$$(grep nameserver /etc/resolv.conf | awk '{print $$2}'):0 && \
	export LIBGL_ALWAYS_INDIRECT=1 && \
	./coupGUI

coupGUI:
	$(CXX) $(GUI_SRC) $(CXXFLAGS) $(LDFLAGS) -o coupGUI

main: main.cpp $(LOGIC_SRC)
	$(CXX) main.cpp $(LOGIC_SRC) $(CXXFLAGS) $(LDFLAGS) -g -o demo

test: test.cpp $(LOGIC_SRC)
	$(CXX) test.cpp $(LOGIC_SRC) $(CXXFLAGS) $(LDFLAGS) -o test

valgrind:
	g++ main.cpp Source/*.cpp -Wall -std=c++17 -IHeaders -lsfml-graphics -lsfml-window -lsfml-system -o demo
	valgrind --leak-check=full ./demo

clean:
	rm -f coupGUI demo test
