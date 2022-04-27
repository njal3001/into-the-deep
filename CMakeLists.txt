cmake_minimum_required(VERSION 3.12)
set(CMAKE_CXX_STANDARD 17)
project(itd)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)
find_package(SDL2_mixer REQUIRED)

message("${SDL2_MIXER_INCLUDE_DIRS}")
message("${SDL2_MIXER_LIBRARIES}")

include_directories(${SDL2_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} ${GLM_INCLUDE_DIRS} ${SDL2_MIXER_INCLUDE_DIRS})

add_executable(itd
    src/main.cpp
    src/platform.cpp
    src/input.cpp
    src/file.cpp
    src/debug.cpp
    src/sound.cpp
    src/graphics/graphics.cpp
    src/graphics/renderer.cpp
    src/graphics/shader.cpp
    src/graphics/color.cpp
    src/graphics/image.cpp
    src/graphics/texture.cpp
    src/graphics/material.cpp
    src/graphics/subtexture.cpp
    src/maths/calc.cpp
    src/gameplay/content.cpp
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
    src/gameplay/torpedo.cpp
    src/gameplay/explosion.cpp
    src/gameplay/playerhud.cpp
    src/gameplay/animator.cpp
    src/gameplay/playerinput.cpp
    src/gameplay/camera.cpp
    src/gameplay/wall.cpp
    )

target_link_libraries(itd ${SDL2_LIBRARIES} ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} ${SDL2_MIXER_LIBRARIES})

add_custom_target(run
    COMMAND itd
    DEPENDS itd
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR})
