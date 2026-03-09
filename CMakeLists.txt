cmake_minimum_required(VERSION 4.1)

project(Grafika)

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
            src/header.hpp
)

target_compile_features(grafika PRIVATE cxx_std_23)
add_executable(Grafika::grafika ALIAS grafika)
