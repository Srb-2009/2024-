/**
 * @file main.cpp
 * @brief 随机点名器主程序入口
 * 
 * 功能特点：
 * - 支持开场视频动画（sfeMovie或帧序列）
 * - 随机点名，名字滚动效果
 * - 显示对应角色立绘
 * - 支持自定义资源文件
 * 
 * 操作说明：
 * - 空格键：开始点名 / 停止点名 / 下一轮
 * - 空格键（开场时）：跳过动画
 * - Ctrl+R：重置点名记录
 * - ESC：返回准备状态
 */

#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>

#include "ResourceManager.h"
#include "NameRoller.h"

/**
 * @brief 打印使用帮助
 */
void printHelp() {
    std::cout << "=== 随机点名器 ===" << std::endl;
    std::cout << "操作说明：" << std::endl;
    std::cout << "  空格键    - 开始点名 / 停止点名 / 下一轮" << std::endl;
    std::cout << "  空格键    - 开场动画时跳过" << std::endl;
    std::cout << "  Ctrl + R  - 重置点名记录" << std::endl;
    std::cout << "  ESC       - 返回准备状态" << std::endl;
    std::cout << "  关闭窗口  - 退出程序" << std::endl;
    std::cout << std::endl;
}

/**
 * @brief 主函数
 * @param argc 参数数量
 * @param argv 参数数组
 * @return 退出码
 */
int main(int argc, char* argv[]) {
    // 打印欢迎信息
    std::cout << "========================================" << std::endl;
    std::cout << "       随机点名器 v1.0.0" << std::endl;
    std::cout << "  基于 SFML 多媒体库开发" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;

    printHelp();

    // ==================== 资源目录设置 ====================
    // 默认资源目录为可执行文件同目录下的resources文件夹
    // 也可以通过命令行参数指定资源目录
    std::string resourceDir = "resources";
    
    if (argc > 1) {
        resourceDir = argv[1];
        std::cout << "[主程序] 使用指定资源目录: " << resourceDir << std::endl;
    } else {
        std::cout << "[主程序] 使用默认资源目录: " << resourceDir << std::endl;
    }

    // ==================== 初始化资源管理器 ====================
    ResourceManager& rm = ResourceManager::getInstance();
    if (!rm.init(resourceDir)) {
        std::cerr << "[错误] 资源管理器初始化失败！" << std::endl;
        std::cerr << "请确保 resources 目录存在且包含必要的资源文件。" << std::endl;
        std::cerr << "资源目录结构请参考 README.md 文档。" << std::endl;
        return EXIT_FAILURE;
    }

    // ==================== 创建窗口 ====================
    // 窗口大小可以根据需要调整
    const unsigned int windowWidth = 900;
    const unsigned int windowHeight = 700;

    sf::RenderWindow window(
        sf::VideoMode(windowWidth, windowHeight),
        "随机点名器",
        sf::Style::Titlebar | sf::Style::Close
    );

    // 设置帧率限制，避免占用过多CPU
    window.setFramerateLimit(60);

    // 启用垂直同步（可选）
    // window.setVerticalSyncEnabled(true);

    std::cout << "[主程序] 窗口创建成功: " << windowWidth << "x" << windowHeight << std::endl;

    // ==================== 初始化点名器 ====================
    NameRoller nameRoller;
    if (!nameRoller.init(window.getSize())) {
        std::cerr << "[错误] 点名器初始化失败！" << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "[主程序] 点名器初始化完成，开始运行..." << std::endl;
    std::cout << std::endl;

    // ==================== 主循环 ====================
    sf::Clock clock;  // 用于计算帧时间

    while (window.isOpen()) {
        // 计算上一帧到现在的时间
        float deltaTime = clock.restart().asSeconds();

        // ==================== 事件处理 ====================
        sf::Event event;
        while (window.pollEvent(event)) {
            // 窗口关闭事件
            if (event.type == sf::Event::Closed) {
                std::cout << "[主程序] 用户关闭窗口，退出程序" << std::endl;
                window.close();
                break;
            }

            // 将事件传递给点名器处理
            nameRoller.handleEvent(event);
        }

        // 如果窗口已关闭，跳出循环
        if (!window.isOpen()) {
            break;
        }

        // ==================== 更新逻辑 ====================
        nameRoller.update(deltaTime);

        // ==================== 渲染 ====================
        // 清空窗口
        window.clear(sf::Color(20, 20, 40));

        // 渲染点名器内容
        nameRoller.render(window);

        // 显示渲染结果
        window.display();
    }

    std::cout << std::endl;
    std::cout << "[主程序] 程序正常退出" << std::endl;

    return EXIT_SUCCESS;
}
