set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

cmake_minimum_required(VERSION 3.5)
project(renderer VERSION 0.1.0 LANGUAGES CXX)

# set(CMAKE_GENERATOR_PLATFORM x64)
include(CTest)
enable_testing()
find_package(OpenGL REQUIRED)

option(TRACY_ENABLE "" ON)

add_subdirectory(thirdparty/glfw)
add_subdirectory(thirdparty/glad)
add_subdirectory(thirdparty/glm)
add_subdirectory(thirdparty/imgui-lib)
add_subdirectory(thirdparty/assimp)

file(GLOB_RECURSE LOCAL_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(${PROJECT_NAME} ${LOCAL_SOURCES})
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

target_include_directories(${PROJECT_NAME} PRIVATE thirdparty)
target_include_directories(${PROJECT_NAME} PRIVATE thirdparty/glad/include)
target_include_directories(${PROJECT_NAME} PRIVATE thirdparty/glfw/include)

target_include_directories(${PROJECT_NAME} PRIVATE thirdparty/imgui-lib)
target_include_directories(${PROJECT_NAME} PRIVATE thirdparty/imgui-lib/imgui)
target_include_directories(${PROJECT_NAME} PRIVATE src/)

target_link_libraries(${PROJECT_NAME} glfw)
target_link_libraries(${PROJECT_NAME} glad)
target_link_libraries(${PROJECT_NAME} glm)
target_link_libraries(${PROJECT_NAME} OpenGL::GL)
target_link_libraries(${PROJECT_NAME} imgui)
target_link_libraries(${PROJECT_NAME} assimp)

SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/PROFILE")

# SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/PROFILE")
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
add_custom_target(resources ALL)
add_custom_command(TARGET resources PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/resources/ $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources)
