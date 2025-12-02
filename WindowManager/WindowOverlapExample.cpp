#include "WindowManager.h"
#include "IWindow.h"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    std::cout << "Window Overlap Border Rendering Example" << std::endl;
    
    WindowManager wm;
    
    // 创建几个重叠的窗口
    IWindow* window1 = wm.CreateWindow("window1", "Window 1", 100, 100, 400, 300);
    if (!window1) {
        std::cerr << "Failed to create window 1" << std::endl;
        return -1;
    }
    
    IWindow* window2 = wm.CreateWindow("window2", "Window 2", 300, 200, 400, 300);
    if (!window2) {
        std::cerr << "Failed to create window 2" << std::endl;
        return -1;
    }
    
    IWindow* window3 = wm.CreateWindow("window3", "Window 3", 500, 150, 400, 300);
    if (!window3) {
        std::cerr << "Failed to create window 3" << std::endl;
        return -1;
    }
    
    // 显示所有窗口
    wm.ShowAllWindows();
    
    // 主循环
    int frame_count = 0;
    const int max_frames = 100; // 运行一定帧数后退出
    
    while (frame_count < max_frames && wm.GetWindowCount() > 0) {
        wm.ProcessMessages();
        
        // 每隔一定时间渲染一次
        if (frame_count % 10 == 0) {
            std::cout << "\n--- Rendering Frame " << frame_count << " ---" << std::endl;
            wm.RenderAllWindows(); // 使用新的渲染方法，处理重叠边框
        }
        
        // 模拟动画 - 移动窗口
        if (frame_count % 30 == 0) {
            int x, y;
            window1->GetPosition(x, y);
            window1->SetPosition(x + 5, y + 2);
            
            window2->GetPosition(x, y);
            window2->SetPosition(x - 3, y + 4);
            
            window3->GetPosition(x, y);
            window3->SetPosition(x - 2, y - 3);
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        frame_count++;
    }
    
    std::cout << "Example completed." << std::endl;
    
    return 0;
}