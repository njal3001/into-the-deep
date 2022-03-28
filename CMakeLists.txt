cmake_minimum_required(VERSION 3.12)
set(CMAKE_CXX_STANDARD 17)
project(itd)

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} ${GLM_INCLUDE_DIRS} ${RAPIDXML_INCLUDE_DIRS})

add_executable(itd
    src/main.cpp
    src/platform.cpp
    src/input.cpp
    src/file.cpp
    src/graphics/graphics.cpp
    src/graphics/renderer.cpp
    src/graphics/shader.cpp
    src/graphics/color.cpp
    src/graphics/image.cpp
    src/graphics/texture.cpp
    src/graphics/material.cpp
    src/maths/calc.cpp
    src/gameplay/entity.cpp
    src/gameplay/component.cpp
    src/gameplay/scene.cpp
    src/gameplay/tilemap.cpp
    src/gameplay/collider.cpp
    src/gameplay/player.cpp
    src/gameplay/factory.cpp
    src/gameplay/collisionhandler.cpp
    src/gameplay/mover.cpp
    src/gameplay/chaser.cpp
    src/gameplay/hurtable.cpp
    src/gameplay/rocket.cpp
    src/gameplay/explosion.cpp
    src/gameplay/playerhud.cpp
    )

target_link_libraries(itd ${SDL2_LIBRARIES} ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES})

add_custom_target(run
    COMMAND itd
    DEPENDS itd
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR})
