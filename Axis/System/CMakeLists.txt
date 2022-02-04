# Contains CMake utilities functions
include("../../CMake/Utility.cmake")

# Collects all system's header files
set(AXIS_SYSTEM_HEADER_FILES
   "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/Assert.hpp"
   "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/Config.hpp"
   "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/Memory.hpp"
   "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/SystemExport.hpp"
   "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/SystemPch.hpp"
   "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/String.hpp"
   "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/Math.hpp"
   "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/List.hpp"
   "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/Vector2.hpp"
   "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/Vector3.hpp"
   "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/Rectangle.hpp"
   "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/Random.hpp"
   "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/SmartPointer.hpp"
   "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/HashSet.hpp"
   "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/HashMap.hpp"
   "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/LinkedList.hpp"
   "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/Enum.hpp"
   "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/Span.hpp"
   "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/TimePeriod.hpp"
   "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/Assembly.hpp"
   "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/System.hpp"
   "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/Matrix.hpp"
   "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/FileStream.hpp"
   "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/Path.hpp"
   "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/StringView.hpp"
   "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/System"
   "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/Trait.hpp")

# Collects all system's source files
set(AXIS_SYSTEM_SOURCE_FILES
   "${CMAKE_CURRENT_LIST_DIR}/Source/Axis/Exception.cpp"
   "${CMAKE_CURRENT_LIST_DIR}/Source/Axis/Random.cpp"
   "${CMAKE_CURRENT_LIST_DIR}/Source/Axis/Memory.cpp"
   "${CMAKE_CURRENT_LIST_DIR}/Source/Axis/Stream.cpp"
   "${CMAKE_CURRENT_LIST_DIR}/Source/Axis/FileStream.cpp"
   "${CMAKE_CURRENT_LIST_DIR}/Source/Axis/Path.cpp"
   "${CMAKE_CURRENT_LIST_DIR}/Source/Axis/SmartPointer.cpp")

# Collects all system's private files
set(AXIS_SYSTEM_PRIVATE_FILES
   "${CMAKE_CURRENT_LIST_DIR}/Private/Axis/MemoryImpl.inl"
   "${CMAKE_CURRENT_LIST_DIR}/Private/Axis/MathImpl.inl"
   "${CMAKE_CURRENT_LIST_DIR}/Private/Axis/ListImpl.inl"
   "${CMAKE_CURRENT_LIST_DIR}/Private/Axis/Vector2Impl.inl"
   "${CMAKE_CURRENT_LIST_DIR}/Private/Axis/Vector3Impl.inl"
   "${CMAKE_CURRENT_LIST_DIR}/Private/Axis/RectangleImpl.inl"
   "${CMAKE_CURRENT_LIST_DIR}/Private/Axis/RandomImpl.inl"
   "${CMAKE_CURRENT_LIST_DIR}/Private/Axis/SmartPointerImpl.inl"
   "${CMAKE_CURRENT_LIST_DIR}/Private/Axis/HashSetImpl.inl"
   "${CMAKE_CURRENT_LIST_DIR}/Private/Axis/HashMapImpl.inl"
   "${CMAKE_CURRENT_LIST_DIR}/Private/Axis/LinkedListImpl.inl"
   "${CMAKE_CURRENT_LIST_DIR}/Private/Axis/EnumImpl.inl"
   "${CMAKE_CURRENT_LIST_DIR}/Private/Axis/SpanImpl.inl"
   "${CMAKE_CURRENT_LIST_DIR}/Private/Axis/TimePeriodImpl.inl"
   "${CMAKE_CURRENT_LIST_DIR}/Private/Axis/MatrixImpl.inl"
   "${CMAKE_CURRENT_LIST_DIR}/Private/Axis/StringViewImpl.inl"
   "${CMAKE_CURRENT_LIST_DIR}/Private/Axis/StringImpl.inl")

# Win32 platform specific header and source files
if(${AXIS_PLATFORM_WIN32})
    list(APPEND AXIS_SYSTEM_HEADER_FILES "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/Platform/Win32/Win32Timer.hpp")
    list(APPEND AXIS_SYSTEM_HEADER_FILES "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/Platform/Win32/Win32Assembly.hpp")

    list(APPEND AXIS_SYSTEM_SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/Source/Axis/Platform/Win32/Win32Path.cpp")
    list(APPEND AXIS_SYSTEM_SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/Source/Axis/Platform/Win32/Win32Assembly.cpp")
    list(APPEND AXIS_SYSTEM_SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/Source/Axis/Platform/Win32/Win32Timer.cpp")
    list(APPEND AXIS_SYSTEM_SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/Source/Axis/Platform/Win32/Win32System.cpp")
endif()


# Collects all target's source files
set(AXIS_SYSTEM_ALL_SOURCE_FILES ${AXIS_SYSTEM_HEADER_FILES} ${AXIS_SYSTEM_SOURCE_FILES} ${AXIS_SYSTEM_PRIVATE_FILES})

# Adds System module to the project
axis_add_library(Axis-System
                    FOLDER "Axis/Axis/System"
                    RELATIVE_PATH "${CMAKE_CURRENT_LIST_DIR}/../"
                    INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/Include"
                    SOURCES ${AXIS_SYSTEM_ALL_SOURCE_FILES})

# Uses precompiled header
target_precompile_headers(Axis-System PRIVATE "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/SystemPch.hpp")

# Win32 platform specific lib
if(${AXIS_PLATFORM_WIN32})
    target_link_libraries(Axis-System PRIVATE "winmm.lib")
    target_link_libraries(Axis-System PRIVATE "shlwapi.lib")
endif()
