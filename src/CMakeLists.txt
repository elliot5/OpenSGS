# cmake > 3.10
cmake_minimum_required(VERSION 3.10)

# set the project name
project(OpenSGS)

# add subdirectories
add_subdirectory(osgs)

# set output directory
set_target_properties(OpenSGS
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/../bin"
    LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/../bin"
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/../bin"
)
