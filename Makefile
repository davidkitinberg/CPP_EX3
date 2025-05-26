# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -std=c++17 -IHeaders
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system

# Logic-only sources
LOGIC_SRC = Source/Game.cpp Source/Player.cpp Source/PlayerFactory.cpp \
            Source/Baron.cpp Source/Spy.cpp Source/Judge.cpp Source/Governor.cpp \
            Source/Merchant.cpp Source/General.cpp

# GUI source includes main()
GUI_SRC = $(LOGIC_SRC) Source/GUI/coupGUI.cpp

# Target: Windows 11 WSLg environment
win11: coupGUI
	@echo "[INFO] WSLg (Windows 11) detected. Running GUI natively."
	./coupGUI

# Target: Windows 10 WSL environment (VcXsrv fallback)
win10: coupGUI
	@echo "[INFO] WSL (Windows 10) detected. Using VcXsrv fallback..."
	@export DISPLAY=$$(grep nameserver /etc/resolv.conf | awk '{print $$2}'):0; \
	export LIBGL_ALWAYS_INDIRECT=1; \
	./coupGUI

# Build GUI version
coupGUI:
	$(CXX) $(GUI_SRC) $(CXXFLAGS) $(LDFLAGS) -o coupGUI

# Target: compile and run main logic-only CLI program
main: main.cpp $(LOGIC_SRC)
	$(CXX) main.cpp $(LOGIC_SRC) $(CXXFLAGS) $(LDFLAGS) -o demo
	./demo

# Target: compile and run unit tests
test: test.cpp $(LOGIC_SRC)
	$(CXX) test.cpp $(LOGIC_SRC) $(CXXFLAGS) $(LDFLAGS) -o test
	./test

# Optional: Valgrind memory check on CLI program
valgrind:
	$(CXX) main.cpp $(LOGIC_SRC) $(CXXFLAGS) $(LDFLAGS) -o demo
	valgrind --leak-check=full ./demo

# Target: cleanup build artifacts
clean:
	rm -f coupGUI demo test
