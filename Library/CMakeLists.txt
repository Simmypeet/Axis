# Contains CMake utilities functions
include("../CMake/Utility.cmake")

# Workaround for suppressing external targets' messages
function(message)
    if (NOT MESSAGE_QUIET)
        _message(${ARGN})
    endif()
endfunction()

set(MESSAGE_QUIET ON)

# Adds stb_image to the project
add_subdirectory(stb_image)
axis_set_directory_root_folder("stb_image" "Axis/Library/stb_image")