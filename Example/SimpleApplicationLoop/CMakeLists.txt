include("../../CMake/Utility.cmake")

# Source file
set(AXIS_SIMPLEAPPLICATIONLOOP_SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/Source/SimpleApplicationLoop/SimpleApplicationLoop.cpp")

# Targets to link
set(AXIS_SIMPLEAPPLICATIONLOOP_TARGETS_TO_LNK
    Axis-System
    Axis-Window)

axis_add_example(Axis-SimpleApplicationLoop
                 SOURCES ${AXIS_SIMPLEAPPLICATIONLOOP_SOURCE_FILES}
                 TARGETS_TO_LINK "${AXIS_SIMPLEAPPLICATIONLOOP_TARGETS_TO_LNK}"
                 FOLDER "Axis/Example/SimpleApplicationLoop"
                 RELATIVE_PATH "${CMAKE_CURRENT_LIST_DIR}")