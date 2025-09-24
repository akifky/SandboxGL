#include "SandboxGUI.h"

SandboxGUI::SandboxGUI(GLFWwindow* window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io = &ImGui::GetIO();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void SandboxGUI::update()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void SandboxGUI::addText(const std::string text)
{
	ImGui::Text(text.c_str());
}

void SandboxGUI::addIntSlider(std::string sliderName, int &var, int min, int max)
{
    ImGui::SliderInt(sliderName.c_str(), &var, min, max);
}

void SandboxGUI::addFloatSlider(std::string sliderName, float& var, float min, float max)
{
    ImGui::SliderFloat(sliderName.c_str(), &var, min, max);
}

void SandboxGUI::render()
{
    /*ImGui::Begin("Sandbox HUD");
    ImGui::End();*/

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void SandboxGUI::destroy()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}