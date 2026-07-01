#include "ResourceManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <random>
#include <filesystem>

namespace fs = std::filesystem;

// ==================== 单例实现 ====================
ResourceManager& ResourceManager::getInstance() {
    static ResourceManager instance;
    return instance;
}

// ==================== 初始化 ====================
bool ResourceManager::init(const std::string& resourceDir) {
    m_resourceDir = resourceDir;
    
    // 设置视频和帧动画路径
    m_introVideoPath = m_resourceDir + "/video/intro.mp4";
    m_frameAnimationDir = m_resourceDir + "/video/frames/";
    
    std::cout << "[资源管理器] 初始化中，资源目录: " << m_resourceDir << std::endl;
    
    // 创建默认纹理
    createDefaultTexture();
    
    // 加载各类资源
    bool fontsOk = loadFonts();
    bool charsOk = loadCharacters();
    bool namesOk = loadNames();
    
    if (!fontsOk) {
        std::cerr << "[警告] 字体加载失败，将使用系统默认字体" << std::endl;
    }
    if (!charsOk) {
        std::cerr << "[警告] 立绘图片加载失败，将使用默认占位图" << std::endl;
    }
    if (!namesOk) {
        std::cerr << "[错误] 名字列表加载失败！" << std::endl;
        return false;
    }
    
    std::cout << "[资源管理器] 初始化完成" << std::endl;
    std::cout << "  - 字体数量: " << m_fonts.size() << std::endl;
    std::cout << "  - 立绘数量: " << m_characters.size() << std::endl;
    std::cout << "  - 名字数量: " << m_names.size() << std::endl;
    
    return true;
}

// ==================== 字体加载 ====================
bool ResourceManager::loadFonts() {
    std::string fontDir = m_resourceDir + "/fonts/";
    
    // 检查字体目录是否存在
    if (!fs::exists(fontDir)) {
        std::cerr << "[资源管理器] 字体目录不存在: " << fontDir << std::endl;
        return false;
    }
    
    bool loaded = false;
    
    // 遍历字体目录
    for (const auto& entry : fs::directory_iterator(fontDir)) {
        if (entry.is_regular_file()) {
            std::string ext = entry.path().extension().string();
            // 支持的字体格式
            if (ext == ".ttf" || ext == ".otf" || ext == ".TTF" || ext == ".OTF") {
                std::string fontName = entry.path().stem().string();
                sf::Font font;
                
                if (font.loadFromFile(entry.path().string())) {
                    m_fonts[fontName] = font;
                    std::cout << "[资源管理器] 加载字体: " << fontName << ext << std::endl;
                    loaded = true;
                    
                    // 第一个加载的字体作为默认字体
                    if (m_fonts.size() == 1) {
                        m_defaultFont = font;
                        m_fonts["default"] = font;
                    }
                }
            }
        }
    }
    
    return loaded;
}

// ==================== 立绘加载 ====================
bool ResourceManager::loadCharacters() {
    std::string charDir = m_resourceDir + "/characters/";
    
    // 检查立绘目录是否存在
    if (!fs::exists(charDir)) {
        std::cerr << "[资源管理器] 立绘目录不存在: " << charDir << std::endl;
        return false;
    }
    
    bool loaded = false;
    
    // 遍历立绘目录
    for (const auto& entry : fs::directory_iterator(charDir)) {
        if (entry.is_regular_file()) {
            std::string ext = entry.path().extension().string();
            // 支持的图片格式
            if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || 
                ext == ".bmp" || ext == ".PNG" || ext == ".JPG") {
                std::string charName = entry.path().stem().string();
                sf::Texture texture;
                
                if (texture.loadFromFile(entry.path().string())) {
                    // 设置平滑过滤，提升缩放质量
                    texture.setSmooth(true);
                    m_characters[charName] = texture;
                    std::cout << "[资源管理器] 加载立绘: " << charName << ext << std::endl;
                    loaded = true;
                }
            }
        }
    }
    
    return loaded;
}

// ==================== 名字列表加载 ====================
bool ResourceManager::loadNames() {
    std::string namesFile = m_resourceDir + "/config/names.txt";
    
    // 检查名字文件是否存在
    if (!fs::exists(namesFile)) {
        std::cerr << "[资源管理器] 名字列表文件不存在: " << namesFile << std::endl;
        
        // 创建默认名字列表作为备用
        m_names = {"张三", "李四", "王五", "赵六", "钱七", "孙八", "周九", "吴十"};
        std::cout << "[资源管理器] 使用默认名字列表" << std::endl;
        return true;
    }
    
    std::ifstream file(namesFile);
    if (!file.is_open()) {
        std::cerr << "[资源管理器] 无法打开名字文件: " << namesFile << std::endl;
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // 去除首尾空白
        size_t start = line.find_first_not_of(" \t\r\n");
        size_t end = line.find_last_not_of(" \t\r\n");
        
        if (start != std::string::npos && end != std::string::npos) {
            std::string name = line.substr(start, end - start + 1);
            // 跳过空行和注释行（以#开头）
            if (!name.empty() && name[0] != '#') {
                m_names.push_back(name);
            }
        }
    }
    
    file.close();
    
    if (m_names.empty()) {
        std::cerr << "[资源管理器] 名字列表为空！" << std::endl;
        return false;
    }
    
    return true;
}

// ==================== 默认纹理创建 ====================
void ResourceManager::createDefaultTexture() {
    // 创建一个200x300的默认占位图
    sf::Image image;
    image.create(200, 300, sf::Color(100, 100, 100));
    
    // 在中间画一个问号
    // 简单起见，用不同颜色的矩形表示
    for (int x = 80; x < 120; x++) {
        for (int y = 50; y < 150; y++) {
            if (x >= 0 && x < 200 && y >= 0 && y < 300) {
                image.setPixel(x, y, sf::Color::White);
            }
        }
    }
    for (int x = 90; x < 110; x++) {
        for (int y = 180; y < 250; y++) {
            if (x >= 0 && x < 200 && y >= 0 && y < 300) {
                image.setPixel(x, y, sf::Color::White);
            }
        }
    }
    
    m_defaultTexture.loadFromImage(image);
    m_defaultTexture.setSmooth(true);
}

// ==================== 获取方法 ====================
const sf::Font& ResourceManager::getFont(const std::string& name) const {
    auto it = m_fonts.find(name);
    if (it != m_fonts.end()) {
        return it->second;
    }
    return m_defaultFont;
}

const sf::Texture& ResourceManager::getCharacterTexture(const std::string& name) const {
    auto it = m_characters.find(name);
    if (it != m_characters.end()) {
        return it->second;
    }
    return m_defaultTexture;
}

const std::vector<std::string>& ResourceManager::getNames() const {
    return m_names;
}

std::string ResourceManager::getRandomName() const {
    if (m_names.empty()) {
        return "未知";
    }
    
    // 使用随机设备生成真正的随机数
    static std::random_device rd;
    static std::mt19937 gen(rd());
    
    std::uniform_int_distribution<> dis(0, m_names.size() - 1);
    return m_names[dis(gen)];
}

const std::string& ResourceManager::getIntroVideoPath() const {
    return m_introVideoPath;
}

const std::string& ResourceManager::getFrameAnimationDir() const {
    return m_frameAnimationDir;
}

bool ResourceManager::hasCharacter(const std::string& name) const {
    return m_characters.find(name) != m_characters.end();
}

const std::string& ResourceManager::getResourceDir() const {
    return m_resourceDir;
}
