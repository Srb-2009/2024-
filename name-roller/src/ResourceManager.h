#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <map>
#include <memory>

/**
 * @brief 资源管理器类
 * 
 * 负责统一管理所有游戏资源，包括：
 * - 字体文件
 * - 立绘图片
 * - 名字列表
 * - 视频/帧动画资源
 * 
 * 使用单例模式，确保全局只有一个资源管理器实例
 */
class ResourceManager {
public:
    /**
     * @brief 获取单例实例
     * @return 资源管理器引用
     */
    static ResourceManager& getInstance();

    // 禁止拷贝和赋值
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    /**
     * @brief 初始化资源管理器
     * @param resourceDir 资源目录路径
     * @return 是否初始化成功
     */
    bool init(const std::string& resourceDir = "resources");

    /**
     * @brief 获取字体
     * @param name 字体名称（不含扩展名）
     * @return 字体引用
     */
    const sf::Font& getFont(const std::string& name = "default") const;

    /**
     * @brief 获取立绘图片
     * @param name 角色名称
     * @return 纹理引用，如果不存在返回默认纹理
     */
    const sf::Texture& getCharacterTexture(const std::string& name) const;

    /**
     * @brief 获取所有名字列表
     * @return 名字向量
     */
    const std::vector<std::string>& getNames() const;

    /**
     * @brief 获取随机名字
     * @return 随机选中的名字
     */
    std::string getRandomName() const;

    /**
     * @brief 获取开场视频路径
     * @return 视频文件路径
     */
    const std::string& getIntroVideoPath() const;

    /**
     * @brief 获取帧动画目录（无sfeMovie时使用）
     * @return 帧动画目录路径
     */
    const std::string& getFrameAnimationDir() const;

    /**
     * @brief 检查角色是否有对应的立绘
     * @param name 角色名称
     * @return 是否存在立绘
     */
    bool hasCharacter(const std::string& name) const;

    /**
     * @brief 获取资源目录路径
     * @return 资源目录路径
     */
    const std::string& getResourceDir() const;

private:
    // 私有构造函数（单例模式）
    ResourceManager() = default;
    ~ResourceManager() = default;

    /**
     * @brief 加载字体文件
     * @return 是否加载成功
     */
    bool loadFonts();

    /**
     * @brief 加载立绘图片
     * @return 是否加载成功
     */
    bool loadCharacters();

    /**
     * @brief 加载名字列表
     * @return 是否加载成功
     */
    bool loadNames();

    /**
     * @brief 创建默认纹理（当找不到立绘时使用）
     */
    void createDefaultTexture();

    // ==================== 成员变量 ====================
    std::string m_resourceDir;          ///< 资源根目录
    std::string m_introVideoPath;       ///< 开场视频路径
    std::string m_frameAnimationDir;    ///< 帧动画目录

    std::map<std::string, sf::Font> m_fonts;        ///< 字体映射表
    std::map<std::string, sf::Texture> m_characters; ///< 立绘纹理映射表
    std::vector<std::string> m_names;               ///< 名字列表

    sf::Texture m_defaultTexture;   ///< 默认纹理（找不到立绘时使用）
    sf::Font m_defaultFont;         ///< 默认字体
};

#endif // RESOURCE_MANAGER_H
