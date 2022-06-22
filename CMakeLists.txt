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
	"includes/KHR/khrplatform.h"

	"src/DemoOpenGL.cpp"
	"src/DemoOpenGL.h"
	"src/glad.c"
	"src/image.cpp"
	"src/shader_s.h"
	"src/stb_image.h"
	"src/DemoMain.cpp"

	"includes/glm/detail/glm.cpp"
	"includes/glm/simd/common.h"
	"includes/glm/simd/exponential.h"
	"includes/glm/simd/geometric.h"
	"includes/glm/simd/integer.h"
	"includes/glm/simd/matrix.h"
	"includes/glm/simd/neon.h"
	"includes/glm/simd/packing.h"
	"includes/glm/simd/platform.h"
	"includes/glm/simd/trigonometric.h"
	"includes/glm/simd/vector_relational.h"
 )

target_link_libraries(DemoOpenGL glfw3)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET DemoOpenGL PROPERTY CXX_STANDARD 20)
endif()

# TODO: 如有需要，请添加测试并安装目标。
