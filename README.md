# Coup Game (C++ SFML Project)

This is a C++ implementation of the board game **Coup**, featuring:
- Multiple player roles with unique abilities
- Turn-based gameplay with a graphical interface using **SFML**
- Role-specific blocking mechanics and special actions
- Scrollable event log and winner detection

---

## 🕹️ Game Overview

Each player is randomly assigned one of the following roles:

- **Baron** – Can use **Invest** to convert 3 coins into 6 coins
- **Spy** – Can block arrests and view all players' coin counts
- **Governor** – Can block tax
- **Merchant** – Passively gains +1 coin if they have at least 3 coins at the start of their turn
- **Judge** – Can block bribes
- **General** – Can block coups

Players take turns performing actions:
- **Gather**, **Tax**, **Bribe**, **Arrest**, **Sanction**, **Coup**
- Some actions require targeting another player
- Some actions may be **blocked** by other players with appropriate roles
- The game continues until only one player remains — they are declared the winner

---

## 📦 Dependencies

### Required:
- **C++17-compatible compiler** (e.g., `g++`)
- **SFML 2.5+** (`libsfml-graphics`, `libsfml-window`, `libsfml-system`)
- `make` (on Linux/macOS/WSL)

### Ubuntu / WSL Installation:
```bash
sudo apt update
sudo apt install libsfml-dev
```

---

## ⚙️ Building the Project

### Step 1: Clone or Download the Repo
```bash
git clone <your-repo-url>
cd <project-directory>
```

### Step 2: Build with Makefile
```bash
make        # Builds the GUI version (coupGUI)
make run    # Runs the game with platform-specific display handling
```

### Other available targets:
```bash
make main     # Builds CLI version from main.cpp (for logic testing)
make test     # Builds from test.cpp (if you have test cases)
make valgrind # Runs Valgrind on the CLI version (Linux only)
make clean    # Removes compiled binaries
```

---

## 🖥️ Platform Support

| Platform                      | Works? | Notes                                      |
|-------------------------------|--------|---------------------------------------------|
| **Linux (native)**            | ✅     | Full support out of the box                 |
| **Windows 11 + WSL2 + WSLg**  | ✅     | GUI works without extra setup               |
| **Windows 10 + WSL + VcXsrv** | ✅     | Requires [VcXsrv](https://sourceforge.net/projects/vcxsrv/) for GUI |
| **Windows (native)**          | ❌     | Use CLion + CMake or manually compile `coupGUI.cpp` with SFML |

✅ The `Makefile` auto-detects whether you're on WSLg (Windows 11) or VcXsrv (Windows 10), and configures the environment accordingly.

---

## 🧪 Testing

To run a command-line version of the game:
```bash
make main
./demo
```

To test for memory leaks (on Linux):
```bash
make valgrind
```

To run tests:
```bash
make test
./test
```

---

## 📁 Project Structure

```
├── Headers/
│   ├── Baron.hpp
│   ├── General.hpp
│   ├── Governor.hpp
│   ├── Judge.hpp
│   ├── Merchant.hpp
│   ├── Spy.hpp
│   ├── Player.hpp
│   ├── PlayerFactory.hpp
│   ├── Game.hpp
├── Source/
│   ├── GUI/
│   ├── Baron.cpp
│   ├── General.cpp
│   ├── Governor.cpp
│   ├── Judge.cpp
│   ├── Merchant.cpp
│   ├── Spy.cpp
│   ├── Player.cpp
│   ├── PlayerFactory.cpp
│   ├── Game.cpp
├── arial.ttf
├── main.cpp
├── test.cpp
├── Makefile
└── README.md
```

---

## 📃 License

This project is for educational purposes. Free to use and modify. Attribution appreciated.