cmake_minimum_required(VERSION 3.16)
project(archie LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include(FetchContent)

set(IMGUI_DIR ~/dev/libs/imgui/)
set(WHISPER_DIR ~/dev/libs/whisper.cpp)

include_directories(${IMGUI_DIR} ${IMGUI_DIR}/backends ${WHISPER_DIR} ..)


# set the output directory for built objects.
# This makes sure that the dynamic library goes into the build directory automatically.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

# This assumes the SDL source is available in vendored/SDL
add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)
add_subdirectory(vendored/whisper EXCLUDE_FROM_ALL)

# FetchContent_Declare(
#     whisper
#     GIT_REPOSITORY https://github.com/ggml-org/whisper.cpp.git
#     GIT_TAG master
#     GIT_SHALLOW OFF
#     EXCLUDE_FROM_ALL
#     SYSTEM)
# FetchContent_MakeAvailable(whisper)



add_executable(archie 
    main.cpp 
    AudioProcessor.cpp CommandProcessor.cpp
    grammar-parser.cpp common.cpp 
    ${WHISPER_DIR}/src/whisper.cpp 
    ${IMGUI_DIR}/imgui.cpp 
    ${IMGUI_DIR}/imgui_draw.cpp 
    ${IMGUI_DIR}/imgui_demo.cpp 
    ${IMGUI_DIR}/imgui_tables.cpp 
    ${IMGUI_DIR}/imgui_widgets.cpp
)

# target_include_directories(archie PRIVATE ${WHISPER_DIR}/include)
target_link_libraries(archie PRIVATE whisper ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(archie PRIVATE SDL3::SDL3)
install(TARGETS archie RUNTIME)
