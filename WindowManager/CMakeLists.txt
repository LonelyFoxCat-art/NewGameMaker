cmake_minimum_required(VERSION 3.10)
project(WindowManager)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找Windows库
if(WIN32)
    set(WINDOWS_LIBS
        user32
        gdi32
        shell32
    )
endif()

# 定义窗口管理器源文件
set(WINDOW_MANAGER_SOURCES
    Win32Window.cpp
    WindowManager.cpp
    RenderWindow.cpp
)

set(WINDOW_MANAGER_HEADERS
    IWindow.h
    Win32Window.h
    WindowManager.h
    RenderWindow.h
)

# 创建窗口管理器库
add_library(windowmanager ${WINDOW_MANAGER_SOURCES} ${WINDOW_MANAGER_HEADERS})

# 链接Windows库
if(WIN32)
    target_link_libraries(windowmanager ${WINDOWS_LIBS})
endif()

# 包含当前目录
target_include_directories(windowmanager PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# 链接渲染器库
target_include_directories(windowmanager PRIVATE ../Renderer)

# 创建示例可执行文件
add_executable(windowmanager_example WindowManagerExample.cpp)
target_link_libraries(windowmanager_example windowmanager)

# 设置预处理器定义
if(WIN32)
    target_compile_definitions(windowmanager PRIVATE _WIN32)
endif()