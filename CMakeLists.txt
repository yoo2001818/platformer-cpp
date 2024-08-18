cmake_minimum_required(VERSION 3.10)

project(PlatformerCpp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)
set(OpenGL_GL_PREFERENCE GLVND)

# find_package(Vulkan REQUIRED)
find_package(glm REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
include(FetchContent)
include(FindPkgConfig)

PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
PKG_SEARCH_MODULE(SDL2_IMAGE REQUIRED SDL2_image)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.5.1
)

FetchContent_Declare(
  EnTT
  GIT_REPOSITORY https://github.com/skypjack/entt.git
  GIT_TAG        v3.12.2
)

FetchContent_Declare(
  assimp
  GIT_REPOSITORY https://github.com/assimp/assimp.git
  GIT_TAG        v5.3.1
)

FetchContent_MakeAvailable(Catch2)
FetchContent_MakeAvailable(EnTT)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INJECT_DEBUG_POSTFIX OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(assimp)

# add_compile_options(-fsanitize=address)
# add_link_options(-fsanitize=address)

# include_directories(${VULKAN_INCLUDE_DIRS})
include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${SDL2_IMAGE_INCLUDE_DIRS})
include_directories(${SDL2_TTF_INCLUDE_DIRS})
include_directories(${GLM_INCLUDE_DIRS})
include_directories(${OPENGL_INCLUDE_DIRS})
include_directories(${GLEW_INCLUDE_DIRS})

include_directories(${PROJECT_SOURCE_DIR}/imgui)
include_directories(${PROJECT_SOURCE_DIR}/stb)
include_directories(${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/main)
file(GLOB IMGUI_SOURCES ${PROJECT_SOURCE_DIR}/imgui/*.cpp ${PROJECT_SOURCE_DIR}/imgui/backends/imgui_impl_sdl2.cpp ${PROJECT_SOURCE_DIR}/imgui/backends/imgui_impl_opengl3.cpp)
file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp ${PROJECT_SOURCE_DIR}/main/*.cpp)
add_executable(PlatformerCpp ${SOURCES} ${IMGUI_SOURCES})
target_link_libraries(PlatformerCpp ${SDL2_LIBRARIES})
target_link_libraries(PlatformerCpp ${SDL2_IMAGE_LIBRARIES})
target_link_libraries(PlatformerCpp ${SDL2_TTF_LIBRARIES})
target_link_libraries(PlatformerCpp ${OPENGL_LIBRARIES})
target_link_libraries(PlatformerCpp ${GLEW_LIBRARIES})
target_link_libraries(PlatformerCpp EnTT::EnTT)
target_link_libraries(PlatformerCpp assimp)

#[[
file(GLOB_RECURSE TEST_SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp ${PROJECT_SOURCE_DIR}/test/*.cpp)
add_executable(tests ${TEST_SOURCES} ${IMGUI_SOURCES})
target_link_libraries(tests ${SDL2_LIBRARIES})
target_link_libraries(tests ${SDL2_IMAGE_LIBRARIES})
target_link_libraries(tests ${SDL2_TTF_LIBRARIES})
target_link_libraries(tests ${OPENGL_LIBRARIES})
target_link_libraries(tests ${GLEW_LIBRARIES})
target_link_libraries(tests EnTT::EnTT)
target_link_libraries(tests Catch2::Catch2WithMain)
target_link_libraries(tests assimp)
]]

file(GLOB_RECURSE RES_FILES "${CMAKE_CURRENT_SOURCE_DIR}/res/*")

foreach(RES_FILE ${RES_FILES})
  get_filename_component(RES_FILE_NAME ${RES_FILE} NAME)
  set(DEST_FILE "${CMAKE_CURRENT_BINARY_DIR}/${RES_FILE_NAME}")
  add_custom_command(
    OUTPUT ${DEST_FILE}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${RES_FILE} ${DEST_FILE}
    DEPENDS ${RES_FILE}
  )
  list(APPEND RES_FILES_OUT ${DEST_FILE})
endforeach()

add_custom_target(copy_res_files ALL DEPENDS ${RES_FILES_OUT})
