# Adds system module to project
add_subdirectory(System)

# Adds window module to project
add_subdirectory(Window)

# Adds graphics module to project
add_subdirectory(Graphics)

# Adds core module to project
add_subdirectory(Core)

# Adds renderer module to project
add_subdirectory(Renderer)

if (NOT ${AXIS_SKIP_INSTALLS})
    install(FILES "Axis" DESTINATION "include/Axis")
endif()