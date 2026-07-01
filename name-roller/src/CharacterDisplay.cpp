#include "CharacterDisplay.h"
#include "ResourceManager.h"
#include <iostream>
#include <cmath>

// ==================== 构造函数 ====================
CharacterDisplay::CharacterDisplay()
    : m_position(0.0f, 0.0f)
    , m_maxHeight(400.0f)
    , m_nameFontSize(48)
    , m_nameColor(sf::Color::White)
    , m_animState(AnimationState::Hidden)
    , m_animDuration(0.5f)
    , m_animProgress(0.0f)
    , m_currentScale(1.0f)
    , m_currentAlpha(0.0f)
{
    // 设置名字文本属性
    m_nameText.setFont(ResourceManager::getInstance().getFont());
    m_nameText.setCharacterSize(m_nameFontSize);
    m_nameText.setFillColor(m_nameColor);
    m_nameText.setStyle(sf::Text::Bold);

    // 设置名字背景框
    m_nameBackground.setFillColor(sf::Color(0, 0, 0, 150));
    m_nameBackground.setOutlineColor(sf::Color(255, 255, 255, 100));
    m_nameBackground.setOutlineThickness(2.0f);
    m_nameBackground.setCornerPointCount(8);
}

// ==================== 设置角色 ====================
void CharacterDisplay::setCharacter(const std::string& name, const sf::Texture& texture, bool withAnimation) {
    m_currentName = name;

    // 设置立绘纹理
    m_sprite.setTexture(texture);
    m_sprite.setOrigin(
        static_cast<float>(texture.getSize().x) / 2.0f,
        static_cast<float>(texture.getSize().y)
    );  // 原点设置在底部中心

    // 更新缩放
    updateSpriteScale();

    // 设置名字文本
    m_nameText.setString(name);
    m_nameText.setCharacterSize(m_nameFontSize);
    m_nameText.setFillColor(m_nameColor);

    // 计算名字文本的边界
    sf::FloatRect textBounds = m_nameText.getLocalBounds();
    m_nameText.setOrigin(
        textBounds.left + textBounds.width / 2.0f,
        textBounds.top + textBounds.height / 2.0f
    );

    // 设置名字背景框
    float padding = 20.0f;
    m_nameBackground.setSize(sf::Vector2f(
        textBounds.width + padding * 2,
        textBounds.height + padding * 1.5f
    ));
    m_nameBackground.setOrigin(
        m_nameBackground.getSize().x / 2.0f,
        m_nameBackground.getSize().y / 2.0f
    );

    // 开始入场动画
    if (withAnimation) {
        m_animState = AnimationState::ZoomingIn;
        m_animProgress = 0.0f;
        m_currentScale = 0.5f;
        m_currentAlpha = 0.0f;
    } else {
        m_animState = AnimationState::Visible;
        m_animProgress = 1.0f;
        m_currentScale = 1.0f;
        m_currentAlpha = 1.0f;
    }
}

// ==================== 隐藏角色 ====================
void CharacterDisplay::hide(bool withAnimation) {
    if (withAnimation) {
        m_animState = AnimationState::FadingOut;
        m_animProgress = 0.0f;
    } else {
        m_animState = AnimationState::Hidden;
        m_animProgress = 0.0f;
        m_currentAlpha = 0.0f;
    }
}

// ==================== 更新动画 ====================
void CharacterDisplay::update(float deltaTime) {
    switch (m_animState) {
        case AnimationState::FadingIn:
        case AnimationState::FadingOut:
            updateFadeAnimation(deltaTime);
            break;

        case AnimationState::ZoomingIn:
            updateZoomAnimation(deltaTime);
            break;

        default:
            break;
    }
}

// ==================== 淡入淡出动画 ====================
void CharacterDisplay::updateFadeAnimation(float deltaTime) {
    // 更新动画进度
    m_animProgress += deltaTime / m_animDuration;

    if (m_animProgress >= 1.0f) {
        m_animProgress = 1.0f;

        if (m_animState == AnimationState::FadingIn) {
            m_animState = AnimationState::Visible;
            m_currentAlpha = 1.0f;
        } else {
            m_animState = AnimationState::Hidden;
            m_currentAlpha = 0.0f;
        }
    } else {
        // 使用缓动函数让动画更平滑
        float eased = 1.0f - std::pow(1.0f - m_animProgress, 3.0f);

        if (m_animState == AnimationState::FadingIn) {
            m_currentAlpha = eased;
        } else {
            m_currentAlpha = 1.0f - eased;
        }
    }
}

// ==================== 缩放动画 ====================
void CharacterDisplay::updateZoomAnimation(float deltaTime) {
    // 更新动画进度
    m_animProgress += deltaTime / m_animDuration;

    if (m_animProgress >= 1.0f) {
        m_animProgress = 1.0f;
        m_animState = AnimationState::Visible;
        m_currentScale = 1.0f;
        m_currentAlpha = 1.0f;
    } else {
        // 使用弹性缓动函数
        float t = m_animProgress;
        float elastic = 1.0f - std::pow(1.0f - t, 3.0f) * std::cos(t * 3.14159f * 2.0f);
        
        // 从0.5倍缩放到1.0倍
        m_currentScale = 0.5f + 0.5f * elastic;
        // 透明度从0到1
        m_currentAlpha = std::min(1.0f, m_animProgress * 2.0f);
    }
}

// ==================== 渲染 ====================
void CharacterDisplay::render(sf::RenderTarget& target) {
    if (m_animState == AnimationState::Hidden || m_currentAlpha <= 0.0f) {
        return;
    }

    // 设置透明度
    sf::Uint8 alpha = static_cast<sf::Uint8>(m_currentAlpha * 255);

    // 渲染立绘
    m_sprite.setPosition(m_position);
    m_sprite.setScale(m_currentScale, m_currentScale);
    m_sprite.setColor(sf::Color(255, 255, 255, alpha));
    target.draw(m_sprite);

    // 渲染名字（在立绘上方）
    float nameY = m_position.y - m_maxHeight - 30.0f;
    sf::Vector2f namePos(m_position.x, nameY);

    // 渲染名字背景
    m_nameBackground.setPosition(namePos);
    sf::Color bgColor = m_nameBackground.getFillColor();
    bgColor.a = static_cast<sf::Uint8>(alpha * 150.0f / 255.0f * 255.0f);
    m_nameBackground.setFillColor(bgColor);
    target.draw(m_nameBackground);

    // 渲染名字文本
    m_nameText.setPosition(namePos);
    sf::Color textColor = m_nameText.getFillColor();
    textColor.a = alpha;
    m_nameText.setFillColor(textColor);
    target.draw(m_nameText);
}

// ==================== 位置与尺寸设置 ====================
void CharacterDisplay::setPosition(float x, float y) {
    m_position = sf::Vector2f(x, y);
}

void CharacterDisplay::setMaxHeight(float height) {
    m_maxHeight = height;
    updateSpriteScale();
}

void CharacterDisplay::setNameFontSize(unsigned int size) {
    m_nameFontSize = size;
    m_nameText.setCharacterSize(size);
}

void CharacterDisplay::setNameColor(const sf::Color& color) {
    m_nameColor = color;
    m_nameText.setFillColor(color);
}

void CharacterDisplay::setAnimationDuration(float duration) {
    m_animDuration = duration;
}

// ==================== 状态查询 ====================
bool CharacterDisplay::isFullyVisible() const {
    return m_animState == AnimationState::Visible;
}

bool CharacterDisplay::isFullyHidden() const {
    return m_animState == AnimationState::Hidden;
}

const std::string& CharacterDisplay::getName() const {
    return m_currentName;
}

// ==================== 辅助方法 ====================
void CharacterDisplay::updateSpriteScale() {
    const sf::Texture* texture = m_sprite.getTexture();
    if (!texture) return;

    float textureHeight = static_cast<float>(texture->getSize().y);
    if (textureHeight > 0) {
        float scale = m_maxHeight / textureHeight;
        m_sprite.setScale(scale, scale);
    }
}
