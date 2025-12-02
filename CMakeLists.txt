cmake_minimum_required(VERSION 3.10)
project(NewGameMaker)

set(CMAKE_CXX_STANDARD 17)

# 查找OpenGL
find_package(OpenGL REQUIRED)

# 包含渲染器目录
include_directories(Renderer)

# 添加渲染器库
set(RENDERER_SOURCES
    Renderer/OpenGLRenderer.cpp
    Renderer/DirectXRenderer.cpp
    Renderer/RendererFactory.cpp
)

add_library(RendererLib ${RENDERER_SOURCES})

# Windows特定设置
if(WIN32)
    target_link_libraries(RendererLib 
        OpenGL::GL 
        opengl32
        glu32
        d3d11
        dxgi
        d3dcompiler
    )
endif()

# 添加运行时库
add_library(RuntimeLib
    Runtime/dllmain.cpp
)

# 添加IDE可执行文件
add_executable(EngineIde
    EngineIde/EngineIde.cpp
    EngineIde/framework.h
    EngineIde/EngineIde.h
    EngineIde/Resource.h
    EngineIde/EngineIde.rc
)

target_link_libraries(EngineIde PRIVATE RendererLib)

# 设置输出目录
set_target_properties(EngineIde PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# 复制资源文件
configure_file(EngineIde/EngineIde.ico EngineIde/EngineIde.ico COPYONLY)
configure_file(EngineIde/small.ico EngineIde/small.ico COPYONLY)