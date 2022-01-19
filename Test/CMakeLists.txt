# Contains CMake utilities functions
include("../CMake/Utility.cmake")

set(AXIS_DOCTEST_DIR "${CMAKE_CURRENT_LIST_DIR}/../Library/doctest/")

# Adds doctest main library
add_library(Axis-Doctest-Main STATIC "DoctestMain.cpp")

# Target folder
set_target_properties(Axis-Doctest-Main PROPERTIES FOLDER "Axis/Test/")

# Uses C++20 standard
set_target_properties(Axis-Doctest-Main PROPERTIES CXX_STANDARD 20)

# C++20 standard is required
set_target_properties(Axis-Doctest-Main PROPERTIES CXX_STANDARD_REQUIRED TRUE)

# Adds include directories
target_include_directories(Axis-Doctest-Main PUBLIC "../Library/doctest/")
  
# Adds source files to the source group
axis_assign_source_group("DoctestMain.cpp" ${AXIS_DOCTEST_DIR})

if(TARGET Axis-System)
    add_subdirectory(System)
endif()