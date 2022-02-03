# Adds system module to project
add_subdirectory(System)

# Adds window module to project
add_subdirectory(Window)

# Adds graphics module to project
add_subdirectory(Graphics)

# Adds graphics module to project
add_subdirectory(Core)

if (NOT ${AXIS_SKIP_INSTALLS})
    install(FILES "Axis" DESTINATION "include/Axis")
endif()