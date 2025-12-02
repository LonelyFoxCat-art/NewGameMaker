#pragma once
#include <string>

// 窗口接口定义
class IWindow {
public:
    virtual ~IWindow() = default;

    // 窗口基本操作
    virtual bool Create(const std::string& title, int x, int y, int width, int height) = 0;
    virtual void Destroy() = 0;
    virtual void Show() = 0;
    virtual void Hide() = 0;
    virtual void Update() = 0;

    // 窗口状态
    virtual bool IsVisible() const = 0;
    virtual bool IsValid() const = 0;

    // 窗口属性
    virtual void SetTitle(const std::string& title) = 0;
    virtual std::string GetTitle() const = 0;
    virtual void SetPosition(int x, int y) = 0;
    virtual void GetPosition(int& x, int& y) const = 0;
    virtual void SetSize(int width, int height) = 0;
    virtual void GetSize(int& width, int& height) const = 0;
    virtual int GetX() const = 0;
    virtual int GetY() const = 0;
    virtual int GetWidth() const = 0;
    virtual int GetHeight() const = 0;

    // 渲染相关
    virtual void* GetNativeHandle() = 0;  // 获取原生窗口句柄
    virtual void BeginRender() = 0;
    virtual void EndRender() = 0;
};