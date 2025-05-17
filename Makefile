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

# Default target
all: coupGUI

# Run the app — detect WSL vs WSLg vs Linux
run: coupGUI
	@ \
	OS_NAME=$$(uname -s); \
	WSL_VERSION=$$(grep -i microsoft /proc/version 2>/dev/null || true); \
	if [ "$$OS_NAME" = "Linux" ] && [ -n "$$WSL_VERSION" ]; then \
		# Inside WSL — now detect if WSLg or VcXsrv needed \
		if grep -q "WSLg" /proc/version 2>/dev/null; then \
			echo "[INFO] WSLg detected (Windows 11). Running GUI natively."; \
			./coupGUI; \
		else \
			echo "[INFO] WSL (Windows 10) detected. Using VcXsrv fallback..."; \
			export DISPLAY=$$(grep nameserver /etc/resolv.conf | awk '{print $$2}'):0; \
			export LIBGL_ALWAYS_INDIRECT=1; \
			./coupGUI; \
		fi \
	elif [ "$$OS_NAME" = "Linux" ]; then \
		echo "[INFO] Native Linux detected. Running normally."; \
		./coupGUI; \
	else \
		echo "[ERROR] Unknown or unsupported environment."; \
		exit 1; \
	fi

# Build GUI version
coupGUI:
	$(CXX) $(GUI_SRC) $(CXXFLAGS) $(LDFLAGS) -o coupGUI

# Command-line version for logic testing
main: main.cpp $(LOGIC_SRC)
	$(CXX) main.cpp $(LOGIC_SRC) $(CXXFLAGS) $(LDFLAGS) -g -o demo

# Unit testing binary
test: test.cpp $(LOGIC_SRC)
	$(CXX) test.cpp $(LOGIC_SRC) $(CXXFLAGS) $(LDFLAGS) -o test

# Valgrind memory check (Linux/WSL only)
valgrind:
	$(CXX) main.cpp $(LOGIC_SRC) $(CXXFLAGS) $(LDFLAGS) -o demo
	valgrind --leak-check=full ./demo

# Cleanup
clean:
	rm -f coupGUI demo test
