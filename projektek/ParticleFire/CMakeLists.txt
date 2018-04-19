cmake_minimum_required(VERSION 2.8)
project(Particle_fire)

# Before CMake 3.1
add_compile_options(-std=c++11)

# After CMake 3.1
# set(CMAKE_CXX_STANDARD 11)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

set(SOURCES 	src/main.cpp
		src/model.cpp
		src/texture.cpp
		src/heightmap.cpp
		src/camera.cpp
		src/sky.cpp
		src/environment.cpp
		src/light.cpp
		src/sphere.cpp
		src/tdogl/Program.cpp
		src/tdogl/Program.h
		src/tdogl/Shader.cpp
		src/tdogl/Shader.h
		src/phong.fs
		src/phong.vs
		src/unifColor-Phong.fs
		src/unifColor-Phong.vs
		src/fire.cpp
		src/fireShader.fs
		src/fireShader.vs

		include/model.hpp
		include/texture.hpp
		include/heightmap.hpp
		include/camera.hpp
		include/sky.hpp
		include/environment.hpp
		include/light.hpp
		include/sphere.hpp
		include/fire.hpp
		)

add_executable(Particle_fire ${SOURCES})

# Find SOIL
FIND_PATH( SOIL_INCLUDE_DIRS SOIL.h PATH_SUFFIXES SOIL )
FIND_LIBRARY( SOIL_LIBRARIES SOIL)

# Find FreeGlut
FIND_PATH(FREEGLUT_INCLUDE_DIRS NAMES GL/freeglut.h)
FIND_LIBRARY(FREEGLUT_LIBRARY NAMES freeglut)

include_directories( ${OPENGL_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS}  ${FREEGLUT_INCLUDE_DIRS} /include)

target_link_libraries(Particle_fire ${OPENGL_LIBRARIES} ${FREEGLUT_LIBRARY} ${SOIL_LIBRARIES} ${GLEW_LIBRARIES}) 
