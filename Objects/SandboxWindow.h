#pragma once
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <vector>
#include <string>

class SandboxWindow
{
public:
    ImGuiIO* io;

    SandboxWindow(GLFWwindow* window);
    void update();
    void render();
    void destroy();
    void setPosition(ImVec2 pos);
    void addText(const std::string* textPtr);

private:
    std::vector<const std::string*> texts;
    ImVec2 nextPos = ImVec2(0, 0);
};