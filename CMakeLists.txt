cmake_minimum_required(VERSION 3.12)
set(CMAKE_CXX_STANDARD 17)
project(uboat)

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} ${GLM_INCLUDE_DIRS} ${RAPIDXML_INCLUDE_DIRS})

add_executable(uboat
    main.cpp
    platform.cpp
    input.cpp
    file.cpp
    graphics/graphics.cpp
    graphics/renderer.cpp
    graphics/shader.cpp
    graphics/color.cpp
    graphics/image.cpp
    graphics/texture.cpp
    graphics/material.cpp
    gameplay/entity.cpp
    gameplay/component.cpp
    gameplay/scene.cpp
    gameplay/tilemap.cpp
    gameplay/collider.cpp
    gameplay/player.cpp
    gameplay/factory.cpp
    gameplay/collisionhandler.cpp
    gameplay/mover.cpp
    gameplay/chaser.cpp
    gameplay/bullet.cpp
    gameplay/hurtable.cpp
    )

target_link_libraries(uboat ${SDL2_LIBRARIES} ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES})

add_custom_target(run
    COMMAND uboat
    DEPENDS uboat
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR})
