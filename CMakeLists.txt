cmake_minimum_required(VERSION 3.16)
project(StockSenseQt VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt settings
set(CMAKE_AUTOMOC ON)

# Set Qt path
if(NOT DEFINED CMAKE_PREFIX_PATH)
    set(CMAKE_PREFIX_PATH "C:/Qt/6.5.3/mingw_64")
endif()

# Try to find Qt Charts, if not available, build without it
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
find_package(Qt6 QUIET COMPONENTS Charts)

if(Qt6Charts_FOUND)
    message(STATUS "✅ Qt Charts found - Building with real charts")
    set(HAVE_CHARTS TRUE)
else()
    message(STATUS "❌ Qt Charts not found - Building with custom charts")
    set(HAVE_CHARTS FALSE)
endif()

# Create executable
add_executable(StockSenseQt src/main.cpp)

# Link Qt libraries
target_link_libraries(StockSenseQt PRIVATE Qt6::Core Qt6::Widgets)

if(HAVE_CHARTS)
    target_link_libraries(StockSenseQt PRIVATE Qt6::Charts)
    target_compile_definitions(StockSenseQt PRIVATE HAVE_QT_CHARTS)
endif()

# Set properties
set_target_properties(StockSenseQt PROPERTIES WIN32_EXECUTABLE TRUE)

message(STATUS "🚀 StockSense Build Configuration")
message(STATUS "📊 Charts Support: ${HAVE_CHARTS}")
message(STATUS "⚡ Qt Version: ${Qt6_VERSION}")
