cmake_minimum_required(VERSION 3.20)

# for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(pong VERSION 0.1.0 LANGUAGES C CXX)

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)

add_executable(pong)

target_sources(pong PRIVATE
	# game files
	source/main.cxx
	source/game.cxx
	source/shader.cxx
	source/texture.cxx
	source/resource_manager.cxx
	source/sprite_renderer.cxx
	source/game_object.cxx
	source/ball_object.cxx
	source/particle_generator.cxx
	# glad
	third_party/glad/src/glad.c
	# stb_image
	third_party/stb_image/src/stb_image.cpp
)

target_include_directories(pong PRIVATE
	third_party/glad/include
	third_party/stb_image/include
)

target_link_libraries(pong PRIVATE
	glfw
	OpenGL::GL
	glm::glm
)
