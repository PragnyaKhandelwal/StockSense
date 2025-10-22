cmake_minimum_required(VERSION 3.16)
project(StockSense VERSION 1.0.0 LANGUAGES CXX)

# Generate compile_commands.json for better IDE support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt settings
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC OFF)

# Set Qt path - FIXED: Changed from 6.5.10 to 6.5.3
if(NOT DEFINED CMAKE_PREFIX_PATH)
    set(CMAKE_PREFIX_PATH "C:/Qt/6.5.3/mingw_64")
endif()

# Set compiler paths for consistency
if(NOT CMAKE_CXX_COMPILER)
    set(CMAKE_CXX_COMPILER "C:/Qt/Tools/mingw1120_64/bin/g++.exe")
endif()
if(NOT CMAKE_C_COMPILER)
    set(CMAKE_C_COMPILER "C:/Qt/Tools/mingw1120_64/bin/gcc.exe")
endif()

# Add include directories
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

# Set UI files directory for AUTOUIC  
set(CMAKE_AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/ui")

# Find Qt6 components
find_package(Qt6 REQUIRED COMPONENTS 
    Core 
    Widgets 
    Gui
    Charts
    Network
)

# Collect source files
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h")
file(GLOB_RECURSE UI_FILES "ui/*.ui")

# Create executable
add_executable(StockSense
    ${SOURCES}
    ${HEADERS}
    ${UI_FILES}
)

# Link Qt libraries
target_link_libraries(StockSense PRIVATE 
    Qt6::Core
    Qt6::Widgets 
    Qt6::Gui
    Qt6::Charts
    Qt6::Network
)

# Set target properties
set_target_properties(StockSense PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
    VERSION ${PROJECT_VERSION}
)

# Include directories
target_include_directories(StockSense PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_BINARY_DIR}"
    ${Qt6_Core_INCLUDE_DIRS}
    ${Qt6_Widgets_INCLUDE_DIRS}
    ${Qt6_Gui_INCLUDE_DIRS}
    ${Qt6_Charts_INCLUDE_DIRS}
    ${Qt6_Network_INCLUDE_DIRS}
)

# Copy Qt DLLs to output directory on Windows
if(WIN32)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS ${CMAKE_PREFIX_PATH}/bin)
    if(WINDEPLOYQT_EXECUTABLE)
        add_custom_command(TARGET StockSense POST_BUILD
            COMMAND ${WINDEPLOYQT_EXECUTABLE} "$<TARGET_FILE:StockSense>"
            COMMENT "Copying Qt libraries")
    endif()
endif()

message(STATUS "🚀 StockSense Build Configuration")
message(STATUS "📊 Charts Support: Qt6Charts Found")
message(STATUS "⚡ Qt Version: ${Qt6_VERSION}")
