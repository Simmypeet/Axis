# Structures the project source files.
function(axis_assign_source_group sourceFiles relativeToPath)
    foreach(_source IN ITEMS ${sourceFiles})
        if (IS_ABSOLUTE "${_source}")
            file(RELATIVE_PATH _source_rel ${relativeToPath} "${_source}")
        else()
            set(_source_rel "${_source}")
        endif()
        get_filename_component(_source_path "${_source_rel}" PATH)
        string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
        source_group("${_source_path_msvc}" FILES "${_source}")
    endforeach()
endfunction(axis_assign_source_group)

# Adds new axis library target
macro(axis_add_library target)
    # parse the arguments
    cmake_parse_arguments(THIS "" "RELATIVE_PATH;FOLDER;" "SOURCES;TARGETS_TO_LINK;INCLUDE_DIRECTORIES;" ${ARGN})
    
    # Adds library as shared library
    add_library(${target} SHARED ${THIS_SOURCES})

    # Target folder
    set_target_properties(${target} PROPERTIES FOLDER ${THIS_FOLDER})

    # Uses C++20 standard
    set_target_properties(${target} PROPERTIES CXX_STANDARD 20)

    # C++20 standard is required
    set_target_properties(${target} PROPERTIES CXX_STANDARD_REQUIRED TRUE)

    # Defines build symbol
    string(REPLACE "-" "_" MODULE_NAME_UPPER "${target}")
    string(TOUPPER "${MODULE_NAME_UPPER}" MODULE_NAME_UPPER)
    set_target_properties(${target} PROPERTIES DEFINE_SYMBOL "BUILD_${MODULE_NAME_UPPER}")

    # Adds target to the list of targets to link
    target_link_libraries(${target} PUBLIC ${THIS_TARGETS_TO_LINK})

    # Adds include directories
    target_include_directories(${target} PUBLIC ${THIS_INCLUDE_DIRECTORIES})
    
    # Adds source files to the source group
    axis_assign_source_group("${THIS_SOURCES}" "${THIS_RELATIVE_PATH}")
endmacro()

# Taken from DiligentCore's BuildUtils.cmake. Reference: https://github.com/DiligentGraphics/DiligentCore/blob/master/BuildUtils.cmake
function(axis_find_target_in_directory _RESULT _DIR)
    get_property(_subdirs DIRECTORY "${_DIR}" PROPERTY SUBDIRECTORIES)
    foreach(_subdir IN LISTS _subdirs)
        axis_find_target_in_directory(${_RESULT} "${_subdir}")
    endforeach()
    get_property(_SUB_TARGETS DIRECTORY "${_DIR}" PROPERTY BUILDSYSTEM_TARGETS)
    set(${_RESULT} ${${_RESULT}} ${_SUB_TARGETS} PARENT_SCOPE)
endfunction(axis_find_target_in_directory)

function(axis_set_directory_root_folder _DIRECTORY _ROOT_FOLDER)
    axis_find_target_in_directory(_TARGETS ${_DIRECTORY})
    foreach(_TARGET IN LISTS _TARGETS)
        get_target_property(_FOLDER ${_TARGET} FOLDER)
        if(_FOLDER)
            set_target_properties(${_TARGET} PROPERTIES FOLDER "${_ROOT_FOLDER}/${_FOLDER}")
        else()
            set_target_properties(${_TARGET} PROPERTIES FOLDER "${_ROOT_FOLDER}")
        endif()
    endforeach()
endfunction(axis_set_directory_root_folder)

# Adds new axis library target
macro(axis_add_test target)
    # parse the arguments
    cmake_parse_arguments(THIS "" "RELATIVE_PATH;FOLDER;" "SOURCES;TARGETS_TO_LINK;INCLUDE_DIRECTORIES;" ${ARGN})

    # Adds executable
    add_executable(${target} ${THIS_SOURCES})

    # Target folder
    set_target_properties(${target} PROPERTIES FOLDER ${THIS_FOLDER})

    # Uses C++20 standard
    set_target_properties(${target} PROPERTIES CXX_STANDARD 20)

    # C++20 standard is required
    set_target_properties(${target} PROPERTIES CXX_STANDARD_REQUIRED TRUE)

    # Adds source files to the source group
    axis_assign_source_group("${THIS_SOURCES}" "${THIS_RELATIVE_PATH}")

    # Links to the test executable
    target_link_libraries(${target} PRIVATE ${THIS_TARGETS_TO_LINK})

    # Adds test executable
    add_test(${target} ${target})

    # Copies shared library binary to the test executable folder
    foreach(TARGET_TO_LINK ${THIS_TARGETS_TO_LINK})
        add_custom_command(TARGET ${target} POST_BUILD
                           COMMAND ${CMAKE_COMMAND} -E copy_directory
                           $<TARGET_FILE_DIR:${TARGET_TO_LINK}>
                           $<TARGET_FILE_DIR:${target}>)
    endforeach()

endmacro(axis_add_test)

# Links shared library to the executable and also adds a custom command to copy the shared library to the executable folder
macro(axis_add_example target)
# parse the arguments
    cmake_parse_arguments(THIS "" "RELATIVE_PATH;FOLDER;" "SOURCES;TARGETS_TO_LINK;INCLUDE_DIRECTORIES;" ${ARGN})

    add_executable(${target} ${THIS_SOURCES})
    
    # Target folder
    set_target_properties(${target} PROPERTIES FOLDER ${THIS_FOLDER})

    # Uses C++20 standard
    set_target_properties(${target} PROPERTIES CXX_STANDARD 20)

    # C++20 standard is required
    set_target_properties(${target} PROPERTIES CXX_STANDARD_REQUIRED TRUE)

    # Adds target to the list of targets to link
    target_link_libraries(${target} PRIVATE ${THIS_TARGETS_TO_LINK})

    # Adds include directories
    target_include_directories(${target} PRIVATE ${THIS_INCLUDE_DIRECTORIES})

    # Adds source files to the source group
    axis_assign_source_group("${THIS_SOURCES}" "${THIS_RELATIVE_PATH}")

    # Copies shared library binary to the executable folder
    foreach(TARGET_TO_LINK ${THIS_TARGETS_TO_LINK})
        add_custom_command(TARGET ${target} POST_BUILD
                           COMMAND ${CMAKE_COMMAND} -E copy_directory
                           $<TARGET_FILE_DIR:${TARGET_TO_LINK}>
                           $<TARGET_FILE_DIR:${target}>)
    endforeach()
endmacro(axis_add_example)