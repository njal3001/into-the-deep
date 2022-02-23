cmake_minimum_required(VERSION 3.12)
set(CMAKE_CXX_STANDARD 17)
project(uboat)

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} ${FREETYPE_INCLUDE_DIRS})

add_executable(uboat
    main.cpp
    platform.cpp
    graphics/graphics.cpp
    graphics/color.cpp
    )

target_link_libraries(uboat ${SDL2_LIBRARIES} ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} ${FREETYPE_LIBRARIES})

add_custom_target(run
    COMMAND uboat
    DEPENDS uboat
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR})
