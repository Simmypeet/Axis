# System test source files
set(AXIS_SYSTEM_TEST_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/Memory.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/String.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Vector2.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Vector3.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Rectangle.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Random.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Math.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/List.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Function.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/SmartPointer.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/TestAllocator.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/HashSet.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/HashMap.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Event.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/LeakTester.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/StaticArray.cpp")

# Targets to link with system test target
set(AXIS_SYSTEM_TEST_TARGETS_TO_LINK
    Axis-System)

# Adds system module test unit
axis_add_test(Axis-Test-System
              FOLDER "Axis/Test"
              RELATIVE_PATH "${CMAKE_CURRENT_LIST_DIR}/"
              SOURCES ${AXIS_SYSTEM_TEST_SOURCES}
              TARGETS_TO_LINK ${AXIS_SYSTEM_TEST_TARGETS_TO_LINK})

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(Axis-Test-System PRIVATE "-bigobj")
endif()

target_link_libraries(Axis-Test-System PRIVATE doctest_with_main)
