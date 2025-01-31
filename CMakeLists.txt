cmake_minimum_required(VERSION 3.22)

project(MediaPlayer)

include(FetchContent)


FetchContent_Declare(glfw
    GIT_REPOSITORY "https://github.com/glfw/glfw"
    GIT_TAG "3.3.8"
    GIT_SHALLOW ON
)
FetchContent_Declare(glm
    GIT_REPOSITORY "https://github.com/Michaeltetteh/glm"
    GIT_TAG "0.9.9.8"
    GIT_SHALLOW ON
)

set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")

find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)

find_package(SDL2 REQUIRED CONFIG COMPONENTS SDL2main)


set(CMAKE_CXX_STANDARD 17)

list(APPEND IMGUI_SRC 
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/backends/imgui_impl_glfw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/backends/imgui_impl_opengl3.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/imgui.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/imgui_demo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/imgui_draw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/imgui_tables.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/imgui_widgets.cpp
 
 #File Dialog
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/ImGuiFileDialog/ImGuiFileDialog.cpp
)

list(APPEND APP_SRC
    # ${CMAKE_CURRENT_SOURCE_DIR}/src/video_decoder.cpp
    # ${CMAKE_CURRENT_SOURCE_DIR}/src/audio_player.cpp
    # ${CMAKE_CURRENT_SOURCE_DIR}/src/player.cpp
    # ${CMAKE_CURRENT_SOURCE_DIR}/src/app.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/renderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)

# macro(copy_files files)
#     foreach(file ${files})
#         message(STATUS "Copying resource ${file}")
#         file(COPY ${file} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/shaders)
#     endforeach()
# endmacro()
# copy_files(${CMAKE_CURRENT_SOURCE_DIR}/src/shaders/)

add_subdirectory(libs)

add_executable(MediaPlayer ${IMGUI_SRC} ${APP_SRC})

target_include_directories(MediaPlayer PRIVATE src libs)

# flag for debugging (Generate source-level debug information)
set(CMAKE_BUILD_TYPE "Debug")
target_compile_options(MediaPlayer PRIVATE -g)

target_compile_definitions(MediaPlayer PRIVATE NDEBUG)

# SDL2::SDL2main may or may not be available. It is e.g. required by Windows GUI applications
# if(TARGET SDL2::SDL2main)
#     # It has an implicit dependency on SDL2 functions, so it MUST be added before SDL2::SDL2 (or SDL2::SDL2-static)
#     target_link_libraries(MediaPlayer PRIVATE SDL2::SDL2main)
# endif()

FetchContent_MakeAvailable(glm glfw)
# target_link_libraries(MediaPlayer FFmpeg glad glm glfw SDL2::SDL2main SDL2::SDL2)
target_link_libraries(MediaPlayer glad glm glfw SDL2::SDL2main SDL2::SDL2)