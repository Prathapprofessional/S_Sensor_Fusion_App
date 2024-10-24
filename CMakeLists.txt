#Minimum version of CMake required
cmake_minimum_required(VERSION 3.10)

#Project name
project(S_Sensor_Fusion_App)

#C++ standards
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#Adding the include directory
include_directories(include)

#Executable
add_executable(S_Sensor_Fusion_App src/main.cpp)

#Testing
add_subdirectory(tests)