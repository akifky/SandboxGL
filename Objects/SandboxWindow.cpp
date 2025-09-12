#include "SandboxWindow.h"

SandboxWindow::SandboxWindow(GLFWwindow* window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io = &ImGui::GetIO();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void SandboxWindow::update()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void SandboxWindow::setPosition(ImVec2 pos)
{
    nextPos = pos;
}

void SandboxWindow::addText(const std::string* textPtr)
{
    texts.push_back(textPtr);
}

void SandboxWindow::render()
{
    ImGui::SetNextWindowPos(nextPos);
    ImGui::Begin("Sandbox HUD");

    for (auto t : texts)
    {
        ImGui::Text("%s", t->c_str());
    }

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    texts.clear();
}

void SandboxWindow::destroy()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}