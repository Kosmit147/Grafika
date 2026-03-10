#include <glad/glad.h>
#include <GLFW/glfw3.h>

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
}

auto run() -> std::expected<int, std::string> {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) {
        return std::unexpected{ "Failed to initialize glfw." };
    }
    Defer terminate_glfw{ []{ glfwTerminate(); } };

    auto* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, nullptr, nullptr);
    Defer destroy_window{ [&window]{ glfwDestroyWindow(window); } };

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    return 0;
}

auto main() -> int {
    if (auto result = run(); !result) {
        std::println("ERROR: {}", result.error());
    } else {
        return *result;
    }
}
