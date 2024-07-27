cmake_minimum_required(VERSION 3.28)
set(name eurl)
project(${name})
file(GLOB src
        *.cpp
)
file(GLOB header
        *.hpp
        *.h
)
find_package(CURL REQUIRED)
add_subdirectory(library)

add_library(eurl-static STATIC ${src} ${header})
target_link_libraries(eurl-static PUBLIC CURL::libcurl nlohmann_json)

add_library(eurl ALIAS eurl-static)