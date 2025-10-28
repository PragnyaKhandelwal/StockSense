cmake_minimum_required(VERSION 3.16)
project(StockSenseQt VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt settings
set(CMAKE_AUTOMOC ON)

# Find Qt6 components - ADDED Network for real API calls
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network)

# Create executable
add_executable(StockSenseQt src/main.cpp)

# Link Qt libraries - ADDED Network for real API calls
target_link_libraries(StockSenseQt PRIVATE 
    Qt6::Core 
    Qt6::Widgets 
    Qt6::Network
)

set_target_properties(StockSenseQt PROPERTIES WIN32_EXECUTABLE TRUE)

message(STATUS "🚀 StockSense with Real Stock Data API")
