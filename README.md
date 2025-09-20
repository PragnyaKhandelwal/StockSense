# QtApp

A modern Qt6 application with organized project structure.

## Project Structure

```
QtApp/
├── CMakeLists.txt          # Main CMake configuration
├── README.md               # This file
├── .vscode/                # VS Code configuration
│   └── tasks.json         # Build and run tasks
├── src/                   # Source files (.cpp)
│   ├── main.cpp
│   └── mainwindow.cpp
├── include/               # Header files (.h)
│   └── mainwindow.h
├── ui/                    # UI files (.ui) - Design with Qt Designer
│   └── mainwindow.ui
└── build/                 # Build output (generated)
```

## Requirements

- Qt 6.5.10 or later
- CMake 3.16 or later
- MinGW compiler (or MSVC/Clang)
- Ninja build system (recommended)

## Building

### Using VS Code Tasks
1. Open the project in VS Code
2. Press `Ctrl+Shift+P` and run "Tasks: Run Task"
3. Select "Configure CMake" to configure the build
4. Select "Build Application" to build
5. Select "Run Application" to run the executable

### Using Command Line
```bash
# Configure
cmake -B build -G "Ninja" -DCMAKE_BUILD_TYPE=Debug

# Build
cmake --build build

# Run
./build/QtApp.exe
```

## Development

- Source files go in `src/`
- Header files go in `include/`
- UI files go in `ui/` - Design these with **Qt Designer**

## Workflow with Qt Designer

1. **Design UI**: Open Qt Designer → Open `ui/mainwindow.ui` → Design your interface
2. **Code Logic**: Write your C++ logic in `src/` and `include/` folders using VS Code
3. **Build & Run**: Use VS Code tasks or command line to build and run

## Adding New Components

1. Add source files to `src/`
2. Add header files to `include/`
3. Create UI files in `ui/` using Qt Designer
4. CMake will automatically detect and include them