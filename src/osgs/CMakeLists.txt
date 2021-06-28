
# add the executable
add_executable(OpenSGS osgs.c)

configure_file(osgs_config.h.in osgs_config.h)

# prefer GLVND over legacy
cmake_policy(SET CMP0072 NEW)

# find packages via findpkg config
INCLUDE(FindPkgConfig)
PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
PKG_SEARCH_MODULE(OpenGL REQUIRED opengl)
PKG_SEARCH_MODULE(GLEW REQUIRED glew)

# includes on target
target_include_directories(OpenSGS PUBLIC
    ${OPENGL_INCLUDE_DIRS}
    ${SDL2_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/Nuklear/
    ${PROJECT_SOURCE_DIR}/osgs/
)

# link on target
# -m = libm
target_link_libraries(OpenSGS ${OPENGL_LIBRARIES} ${SDL2_LIBRARIES} ${GLEW_LIBRARIES} ${PROJECT_SOURCE_DIR}/osgs/ m)