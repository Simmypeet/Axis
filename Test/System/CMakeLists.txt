# System test source files
set(AXIS_SYSTEM_TEST_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/Memory.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/String.cpp")

# Targets to link with system test target
set(AXIS_SYSTEM_TEST_TARGETS_TO_LINK
    Axis-System)

# Adds system module test unit
axis_add_test(Axis-Test-System
              FOLDER "Axis/Test/System"
              RELATIVE_PATH "${CMAKE_CURRENT_LIST_DIR}/"
              SOURCES ${AXIS_SYSTEM_TEST_SOURCES}
              TARGETS_TO_LINK ${AXIS_SYSTEM_TEST_TARGETS_TO_LINK})

target_link_libraries(Axis-Test-System PUBLIC Axis-Doctest-Main)