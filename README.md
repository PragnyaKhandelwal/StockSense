# StockSense

A Qt6 Widgets desktop application scaffolded with CMake and Ninja (MinGW toolchain) on Windows. This repo includes tasks to configure, build, and run the app directly from VS Code.

## Features
- Qt6 Widgets app with a main window and an About dialog
- Clean CMake setup with AUTOUIC/AUTOMOC
- Automatic deployment of required Qt DLLs via `windeployqt`
- VS Code tasks for Configure, Build, Run, and Rebuild
- IntelliSense configured using `compile_commands.json`

## Prerequisites
- Windows 10/11
- Qt 6.5.x for MinGW (64-bit)
	- Example install paths used here:
		- `C:/Qt/6.5.10/mingw_64`
		- `C:/Qt/Tools/mingw1120_64`
		- `C:/Qt/Tools/CMake_64`
		- `C:/Qt/Tools/Ninja`
- VS Code with extensions:
	- C/C++ (ms-vscode.cpptools)
	- CMake Tools (ms-vscode.cmake-tools)

If your Qt version or paths differ, update `CMakeLists.txt` (CMAKE_PREFIX_PATH and compiler paths) and `.vscode/tasks.json` env PATH entries accordingly.

## Project Structure
- `src/` — C++ sources
- `include/` — headers for UI classes
- `ui/` — Qt Designer `.ui` files for AUTOUIC
- `CMakeLists.txt` — project configuration
- `build/` — out-of-source build directory (created by CMake tasks)
- `.vscode/` — tasks for configure/build/run

## How to Build (VS Code Tasks)
Use the predefined tasks (Terminal → Run Task):
1. Configure CMake
2. Build Application
3. Run Application

These tasks ensure the environment PATH contains Ninja, CMake, and MinGW compilers, and they run `windeployqt` after linking so the app starts without missing DLLs.

## How to Build (Manual, optional)
You can also build manually from a VS Code terminal (PowerShell):

```powershell
$env:PATH = "C:/Qt/Tools/Ninja;C:/Qt/Tools/CMake_64/bin;C:/Qt/Tools/mingw1120_64/bin;$env:PATH"
C:/Qt/Tools/CMake_64/bin/cmake.exe -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=C:/Qt/Tools/mingw1120_64/bin/g++.exe
C:/Qt/Tools/CMake_64/bin/cmake.exe --build build
./build/QtApp.exe
```

## IntelliSense
We generate `build/compile_commands.json` for better IntelliSense and include the Qt headers in `.vscode/c_cpp_properties.json`. If squiggles persist, run:
- Configure CMake
- Build Application
- Command Palette → “C/C++: Reset IntelliSense Database”

## Troubleshooting
- CMake cache mismatch: Delete `build/` and re-run “Configure CMake”.
- Ninja not found: Ensure `C:/Qt/Tools/Ninja` is in PATH (the tasks set this automatically).
- Missing Qt DLLs at runtime: We call `windeployqt` after linking; ensure your Qt bin path is correct via `CMAKE_PREFIX_PATH`.
- Compiler not found: Update `CMAKE_CXX_COMPILER` and PATHs for your MinGW version.

## Git Workflow
Main branch: `main`

Typical flow:
```powershell
git pull
git add -A
git commit -m "Message"
git push
```

Remote is set to: `https://github.com/PragnyaKhandelwal/StockSense.git`

## Roadmap / Future Enhancements
- Stock Market Data Integration
	- Fetch real-time and historical data via APIs (e.g., Alpha Vantage, Yahoo Finance)
	- Configurable API keys and polling intervals
- Charts & Visualization
	- Candlestick, line, and volume charts (Qt Charts or QCustomPlot)
	- Indicators: SMA/EMA, RSI, MACD
- Portfolio & Watchlist
	- Add/remove tickers, track positions, P/L calculations
	- Persistent storage (SQLite)
- Alerts & Notifications
	- Price thresholds, indicator triggers, desktop notifications
- Theming & UX
	- Light/Dark themes, responsive layout
- Packaging
	- Create installer (NSIS/Inno Setup), signed binaries

## License
MIT (add a `LICENSE` file if you intend to open-source formally).

## Acknowledgements
- Qt 6 Project
- VS Code CMake Tools