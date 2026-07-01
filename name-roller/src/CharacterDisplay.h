#ifndef CHARACTER_DISPLAY_H
#define CHARACTER_DISPLAY_H

#include <SFML/Graphics.hpp>
#include <string>

/**
 * @brief 角色立绘显示类
 * 
 * 负责显示角色立绘图片和对应的名字，
 * 支持淡入淡出动画、缩放动画等视觉效果
 */
class CharacterDisplay {
public:
    /**
     * @brief 动画状态枚举
     */
    enum class AnimationState {
        Hidden,         ///< 隐藏状态
        FadingIn,       ///< 淡入中
        Visible,        ///< 完全显示
        FadingOut,      ///< 淡出中
        ZoomingIn       ///< 放大进入
    };

    CharacterDisplay();
    ~CharacterDisplay() = default;

    /**
     * @brief 设置角色信息并显示
     * @param name 角色名字
     * @param texture 立绘纹理
     * @param withAnimation 是否播放入场动画
     */
    void setCharacter(const std::string& name, const sf::Texture& texture, bool withAnimation = true);

    /**
     * @brief 隐藏角色显示
     * @param withAnimation 是否播放淡出动画
     */
    void hide(bool withAnimation = true);

    /**
     * @brief 更新动画状态（每帧调用）
     * @param deltaTime 距上一帧的时间（秒）
     */
    void update(float deltaTime);

    /**
     * @brief 渲染到目标窗口
     * @param target 渲染目标
     */
    void render(sf::RenderTarget& target);

    /**
     * @brief 设置显示位置（立绘底部中心位置）
     * @param x X坐标
     * @param y Y坐标
     */
    void setPosition(float x, float y);

    /**
     * @brief 设置立绘最大高度
     * @param height 最大高度（像素）
     */
    void setMaxHeight(float height);

    /**
     * @brief 设置名字文字大小
     * @param size 字体大小
     */
    void setNameFontSize(unsigned int size);

    /**
     * @brief 设置名字颜色
     * @param color 颜色
     */
    void setNameColor(const sf::Color& color);

    /**
     * @brief 设置动画持续时间
     * @param duration 持续时间（秒）
     */
    void setAnimationDuration(float duration);

    /**
     * @brief 检查是否完全显示
     * @return 是否完全显示
     */
    bool isFullyVisible() const;

    /**
     * @brief 检查是否完全隐藏
     * @return 是否完全隐藏
     */
    bool isFullyHidden() const;

    /**
     * @brief 获取当前角色名字
     * @return 角色名字
     */
    const std::string& getName() const;

private:
    /**
     * @brief 更新淡入淡出动画
     * @param deltaTime 时间增量
     */
    void updateFadeAnimation(float deltaTime);

    /**
     * @brief 更新缩放动画
     * @param deltaTime 时间增量
     */
    void updateZoomAnimation(float deltaTime);

    /**
     * @brief 更新立绘缩放以适应最大高度
     */
    void updateSpriteScale();

    // ==================== 成员变量 ====================
    // 显示对象
    sf::Sprite m_sprite;          ///< 立绘精灵
    sf::Text m_nameText;          ///< 名字文本
    sf::RectangleShape m_nameBackground;  ///< 名字背景框

    // 位置与尺寸
    sf::Vector2f m_position;      ///< 显示位置（底部中心）
    float m_maxHeight;            ///< 立绘最大高度
    unsigned int m_nameFontSize;  ///< 名字字体大小
    sf::Color m_nameColor;        ///< 名字颜色

    // 动画状态
    AnimationState m_animState;   ///< 当前动画状态
    float m_animDuration;         ///< 动画持续时间
    float m_animProgress;         ///< 动画进度（0.0 - 1.0）
    float m_currentScale;         ///< 当前缩放比例
    float m_currentAlpha;         ///< 当前透明度

    // 当前角色信息
    std::string m_currentName;    ///< 当前角色名字
};

#endif // CHARACTER_DISPLAY_H
