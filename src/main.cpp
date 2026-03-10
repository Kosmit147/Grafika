#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>

#include <array>
#include <cassert>
#include <expected>
#include <string>
#include <print>

#include "common.hpp"

constexpr auto WINDOW_WIDTH = 1920u;
constexpr auto WINDOW_HEIGHT = 1080u;
constexpr auto WINDOW_TITLE = "GRAFIKA";

namespace {
    auto glfw_error_callback(int error_code, const char* description) -> void {
        std::println("GLFW Error {}: {}", error_code, description);
    }

    auto glfw_framebuffer_size_callback([[maybe_unused]] GLFWwindow* window, int width, int height) -> void {
        glViewport(0, 0, width, height);
    }

    auto gl_debug_message_callback([[maybe_unused]] GLenum source, [[maybe_unused]] GLenum type, GLuint id,
                                   GLenum severity, [[maybe_unused]] GLsizei length, const GLchar* message,
                                   [[maybe_unused]] const void* user_param) -> void
    {
        switch (severity)
        {
            case GL_DEBUG_SEVERITY_NOTIFICATION:
                std::println("OpenGL Notification {}: {}", id, message);
                break;
            case GL_DEBUG_SEVERITY_LOW:
                std::println("OpenGL Warning {}: {}", id, message);
                break;
            case GL_DEBUG_SEVERITY_MEDIUM:
            case GL_DEBUG_SEVERITY_HIGH:
                std::println("OpenGL Error {}: {}", id, message);
                break;
            default: ;
                assert(false);
        }
    }
}

auto run() -> std::expected<int, std::string> {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) {
        return std::unexpected{ "Failed to initialize glfw." };
    }
    Defer terminate_glfw{ []{ glfwTerminate(); } };

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifndef NDEBUG
    glfwWindowHint(GLFW_CONTEXT_DEBUG, GLFW_TRUE);
#endif
    auto* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, nullptr, nullptr);
    Defer destroy_window{ [&window]{ glfwDestroyWindow(window); } };

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        return std::unexpected{ "Failed to load OpenGL functions." };
    }

#ifndef NDEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(gl_debug_message_callback, nullptr);

    static constexpr auto disabled_messages = std::array<GLuint, 1>{
        131185, // Buffer detailed info from NVIDIA.
    };

    glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_OTHER, GL_DONT_CARE,
                          static_cast<GLsizei>(disabled_messages.size()), disabled_messages.data(), GL_FALSE);
#endif

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glfwSetFramebufferSizeCallback(window, glfw_framebuffer_size_callback);

    std::println("OpenGL Renderer: {}", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
    std::println("OpenGL Version: {}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    auto& imgui_io = ImGui::GetIO();
    imgui_io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    imgui_io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    imgui_io.ConfigDpiScaleFonts = true;     // Automatically overwrite style.FontScaleDpi when monitor DPI changes.
    imgui_io.ConfigDpiScaleViewports = true; // Scale ImGui and platform windows when monitor DPI changes.

    auto monitor_scale = ImGui_ImplGlfw_GetContentScaleForWindow(window);
    auto& imgui_style = ImGui::GetStyle();
    imgui_style.ScaleAllSizes(monitor_scale);
    imgui_style.FontScaleDpi = monitor_scale;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();
    Defer shut_down_imgui{ [] {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }};

    auto clear_color = glm::vec4{ 0.0f, 0.0f, 0.0f, 1.0f };
    glClearColor(clear_color.r, clear_color.g, clear_color.b, clear_color.a);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Window");
        ImGui::ColorEdit4("Clear Color", glm::value_ptr(clear_color));
        ImGui::End();

        glClearColor(clear_color.r, clear_color.g, clear_color.b, clear_color.a);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (imgui_io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(window);
        }

        glfwSwapBuffers(window);
    }

    return EXIT_SUCCESS;
}

auto main() -> int {
    if (auto result = run(); !result) {
        std::println("ERROR: {}", result.error());
    } else {
        return *result;
    }
}
