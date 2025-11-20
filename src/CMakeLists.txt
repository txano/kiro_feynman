# This file was automatically generated for projects
# without default 'CMakeLists.txt' file.

# Only register non-main source files as ESP-IDF components
# main.cpp is handled by Arduino framework
FILE(GLOB app_sources 
    ${CMAKE_SOURCE_DIR}/src/led_matrix.cpp
    ${CMAKE_SOURCE_DIR}/src/wifi_provisioning.cpp
)

idf_component_register(SRCS ${app_sources})
