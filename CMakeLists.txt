cmake_minimum_required(VERSION 3.16)
project(StockSense VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6 components
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network)

# Qt6 standard setup
qt6_standard_project_setup()

# Enable automoc
set(CMAKE_AUTOMOC ON)

# Create the executable with all source files
qt6_add_executable(StockSense
    src/main.cpp
    include/MarketStatusChecker.h
    src/MarketStatusChecker.cpp
    include/RealStockDataManager.h
    src/RealStockDataManager.cpp
    include/RealNewsManager.h
    src/RealNewsManager.cpp
    include/CustomChartWidget.h
    src/CustomChartWidget.cpp
    include/PredictionChartWidget.h
    src/PredictionChartWidget.cpp
    include/StockSenseApp.h
    src/StockSenseApp.cpp
   
)

# Add include directories
target_include_directories(StockSense PRIVATE include)

# Link Qt6 libraries
target_link_libraries(StockSense PRIVATE 
    Qt6::Core 
    Qt6::Widgets 
    Qt6::Network
)

# Set target properties
set_target_properties(StockSense PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)
