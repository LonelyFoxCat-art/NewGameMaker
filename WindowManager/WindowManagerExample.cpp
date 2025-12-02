#include "WindowManager.h"
#include "RenderWindow.h"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    std::cout << "Game Engine Window Manager Example" << std::endl;

    // 创建窗口管理器
    WindowManager window_manager;

    // 创建多个渲染窗口
    const int window_count = 3;
    for (int i = 0; i < window_count; ++i) {
        std::string window_name = "Window" + std::to_string(i + 1);
        std::string window_title = "Game Engine Window " + std::to_string(i + 1);
        
        // 在不同位置创建窗口
        int x = 100 + i * 50;  // 每个窗口向右偏移50像素
        int y = 100 + i * 50;  // 每个窗口向下偏移50像素
        int width = 800;
        int height = 600;
        
        // 使用窗口管理器创建窗口
        IWindow* window = window_manager.CreateWindow(
            window_name, 
            window_title, 
            x, y, width, height
        );
        
        if (window) {
            std::cout << "Created window: " << window_name << std::endl;
            window->Show();
        } else {
            std::cerr << "Failed to create window: " << window_name << std::endl;
        }
    }

    std::cout << "Total windows created: " << window_manager.GetWindowCount() << std::endl;

    // 获取所有窗口名称
    auto window_names = window_manager.GetWindowNames();
    std::cout << "Window names: ";
    for (const auto& name : window_names) {
        std::cout << name << " ";
    }
    std::cout << std::endl;

    // 模拟运行循环
    std::cout << "Starting main loop... Press Ctrl+C to exit." << std::endl;
    
    int frame_count = 0;
    const int max_frames = 300; // 运行300帧后退出
    
    while (frame_count < max_frames) {
        // 处理窗口消息
        window_manager.ProcessMessages();
        
        // 更新所有窗口
        window_manager.UpdateAllWindows();
        
        // 简单的帧计数
        frame_count++;
        
        // 添加一个小延迟以控制帧率
        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // 约60 FPS
    }

    std::cout << "Exiting application..." << std::endl;
    window_manager.Exit();

    return 0;
}