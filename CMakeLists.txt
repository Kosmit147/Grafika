cmake_minimum_required(VERSION 4.1)

project(Grafika)

add_subdirectory(libs/glad SYSTEM)
add_subdirectory(libs/glfw SYSTEM)
add_subdirectory(libs/glm SYSTEM)
add_subdirectory(libs/imgui SYSTEM)

add_executable(grafika)

target_sources(
	grafika

	PRIVATE
		src/main.cpp

    PRIVATE
        FILE_SET HEADERS
        BASE_DIRS
            src
        FILES
            src/common.hpp
)

target_compile_features(grafika PRIVATE cxx_std_23)
add_executable(Grafika::grafika ALIAS grafika)

target_link_libraries(grafika PRIVATE glad::glad glfw glm::glm imgui::imgui)
