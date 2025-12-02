#pragma once
#include "IWindow.h"
#include <windows.h>

class Win32Window : public IWindow {
public:
    Win32Window();
    virtual ~Win32Window();

    // IWindow接口实现
    bool Create(const std::string& title, int x, int y, int width, int height) override;
    void Destroy() override;
    void Show() override;
    void Hide() override;
    void Update() override;

    bool IsVisible() const override;
    bool IsValid() const override;

    void SetTitle(const std::string& title) override;
    std::string GetTitle() const override;
    void SetPosition(int x, int y) override;
    void GetPosition(int& x, int& y) const override;
    void SetSize(int width, int height) override;
    void GetSize(int& width, int& height) const override;
    int GetX() const override;
    int GetY() const override;
    int GetWidth() const override;
    int GetHeight() const override;

    void* GetNativeHandle() override;
    void BeginRender() override;
    void EndRender() override;

    // Windows特定功能
    HWND GetHwnd() const { return hwnd_; }

private:
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    void RegisterWindowClass();
    void UnregisterWindowClass();

    HWND hwnd_;
    std::string title_;
    bool visible_;
    bool valid_;
    static ATOM window_class_atom_;
    static const char* kWindowClassName;
};