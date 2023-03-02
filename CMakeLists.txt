cmake_minimum_required (VERSION 3.8)

project ("DemoOpenGL")

# 指定C++ 17，采用更加通用的编译参数 比如-std=c++17 而不是-std=gnu++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

# 设定默认build类型
IF(NOT CMAKE_BUILD_TYPE)
	SET(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

# 设置CMake时候的输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# 添加搜索头文件的位置
include_directories(${CMAKE_SOURCE_DIR}/includes)

# 静态链接库，指定要链接的库文件的路径
link_directories(${CMAKE_SOURCE_DIR}/libs)

# 添加module路径，然后查找包
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")
find_package(GLM REQUIRED)
message(STATUS "GLM included at ${GLM_INCLUDE_DIR}")

# 将当前CMAKE_SOURCE_DIR路径设置到root_directory.h，便于文件读取操作
configure_file(configuration/root_directory.h.in configuration/root_directory.h)
include_directories(${CMAKE_BINARY_DIR}/configuration)

# 设置库，从源码编译生成库，并加入LIBS
set(LIBS glfw3 assimp-vc140-mt)
add_library(STB_IMAGE "src/stb_image.cpp")
set(LIBS ${LIBS} STB_IMAGE)
add_library(GLAD "src/glad.c")
set(LIBS ${LIBS} GLAD)

# 用于分别处理每一章节的代码
function(create_project_from_sources chapter)
	file(GLOB SOURCE
        "src/${chapter}/*.h"
        "src/${chapter}/*.cpp"

        "includes/mylib/*"

        "shaders/*.vs"
        "shaders/*.fs"
    )
	set(NAME "${chapter}")
    add_executable(${NAME} ${SOURCE})
    target_link_libraries(${NAME} ${LIBS})

    # 每个章节项目的代码私有，并且使用c++17，多线程编译
    target_compile_options(${NAME} PRIVATE /std:c++17 /MP)
    # target_link_options(${NAME} PUBLIC /ignore:4099)  # 以后再看有啥用

    # 每个章节的最终输出文件的位置
    set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${chapter}")
    set_target_properties(${NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${chapter}/Debug")

    # copy dlls
	file(GLOB DLLS "dlls/*.dll")
    add_custom_command(TARGET ${NAME} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${DLLS} $<TARGET_FILE_DIR:${NAME}>)

    # 替换vs环境下的代码路径
    configure_file(${CMAKE_SOURCE_DIR}/configuration/visualstudio.vcxproj.user.in ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.vcxproj.user @ONLY)
endfunction()

set(CHAPTERS
    1.start_demo
    2.light_demo
    3.model_demo
    4_1.stencil_demo
    4_2.blend_demo
)

# 为上面定义的章节创建子工程
foreach(CHAPTER ${CHAPTERS})
    create_project_from_sources(${CHAPTER})
endforeach(CHAPTER)
