#pragma once
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <vector>
#include <string>

class SandboxGUI
{
public:
    ImGuiIO* io;

    SandboxGUI(GLFWwindow* window);
    void update();
    void render();
    void destroy();
    void addText(const std::string textPtr);
    void addIntSlider(std::string sliderName, int& var, int min, int max);
    void addFloatSlider(std::string sliderName, float& var, float min, float max);

private:
    std::vector<std::string> texts;
};