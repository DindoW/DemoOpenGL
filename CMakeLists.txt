# CMakeList.txt: DemoOpenGL 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

project ("DemoOpenGL")

# 将源代码添加到此项目的可执行文件。
include_directories(${CMAKE_SOURCE_DIR}/includes)

link_directories(${CMAKE_SOURCE_DIR}/libs)

add_executable (DemoOpenGL
	"includes/GLFW/glfw3.h"
	"includes/GLFW/glfw3native.h"
	"includes/glad/glad.h"

	"src/DemoOpenGL.cpp"
	"src/DemoOpenGL.h"
	"src/glad.c"
	"src/GameMain.cpp"
	"includes/KHR/khrplatform.h"
)

target_link_libraries(DemoOpenGL glfw3)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET DemoOpenGL PROPERTY CXX_STANDARD 20)
endif()

# TODO: 如有需要，请添加测试并安装目标。
