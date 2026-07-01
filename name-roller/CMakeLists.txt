cmake_minimum_required(VERSION 3.16)
project(NameRoller VERSION 1.0.0 LANGUAGES CXX)

# ==================== 项目配置 ====================
# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 编译选项
if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# ==================== 依赖查找 ====================
# 查找SFML库（必需）
# 组件：graphics(图形), window(窗口), system(系统), audio(音频)
find_package(SFML 2.5 COMPONENTS graphics window system audio REQUIRED)

# 查找sfeMovie库（可选，用于视频播放）
# 如果找不到sfeMovie，会自动使用帧序列动画作为降级方案
find_package(sfeMovie QUIET)

# ==================== 源文件配置 ====================
# 定义源文件列表
set(SOURCES
    src/main.cpp
    src/NameRoller.cpp
    src/VideoPlayer.cpp
    src/CharacterDisplay.cpp
    src/ResourceManager.cpp
)

# 定义头文件目录
set(INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# ==================== 可执行文件 ====================
add_executable(${PROJECT_NAME} ${SOURCES})

# 包含目录
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIRS})

# 链接SFML库
target_link_libraries(${PROJECT_NAME} PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
    sfml-audio
)

# ==================== 视频播放支持 ====================
# 如果找到了sfeMovie库，启用视频播放功能
if(sfeMovie_FOUND)
    message(STATUS "找到sfeMovie库，启用完整视频播放功能")
    target_compile_definitions(${PROJECT_NAME} PRIVATE USE_SFE_MOVIE)
    target_link_libraries(${PROJECT_NAME} PRIVATE sfeMovie)
else()
    message(STATUS "未找到sfeMovie库，将使用帧序列动画方案")
    message(STATUS "如需完整视频支持，请安装sfeMovie: https://github.com/SFML/sfeMovie")
endif()

# ==================== 资源文件复制 ====================
# 将resources目录复制到构建目录，方便运行
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/resources
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources
    COMMENT "复制资源文件到构建目录"
)

# ==================== 安装配置（可选） ====================
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
install(DIRECTORY resources/ DESTINATION share/${PROJECT_NAME}/resources)
