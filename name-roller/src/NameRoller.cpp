#include "NameRoller.h"
#include "ResourceManager.h"
#include <iostream>
#include <algorithm>
#include <random>
#include <cmath>

// ==================== 构造函数 ====================
NameRoller::NameRoller()
    : m_state(State::Intro)
    , m_windowSize(800, 600)
    , m_hasIntroVideo(false)
    , m_rollSpeed(10.0f)  // 默认每秒10个名字
    , m_rollTimer(0.0f)
    , m_resultTimer(0.0f)
    , m_bgColor(30, 30, 50)  // 深蓝紫色背景
    , m_allowRepeat(true)
    , m_nameScale(1.0f)
    , m_nameGrowing(true)
{
}

// ==================== 初始化 ====================
bool NameRoller::init(const sf::Vector2u& windowSize) {
    m_windowSize = windowSize;

    // 获取资源管理器实例
    ResourceManager& rm = ResourceManager::getInstance();

    // 初始化可用名字列表
    m_availableNames = rm.getNames();
    m_calledNames.clear();

    // 初始化背景
    m_background.setSize(sf::Vector2f(
        static_cast<float>(windowSize.x),
        static_cast<float>(windowSize.y)
    ));
    m_background.setFillColor(m_bgColor);

    // 初始化标题文本
    m_titleText.setFont(rm.getFont());
    m_titleText.setString("随机点名器");
    m_titleText.setCharacterSize(48);
    m_titleText.setFillColor(sf::Color::White);
    m_titleText.setStyle(sf::Text::Bold);
    
    // 居中标题
    sf::FloatRect titleBounds = m_titleText.getLocalBounds();
    m_titleText.setOrigin(
        titleBounds.left + titleBounds.width / 2.0f,
        titleBounds.top + titleBounds.height / 2.0f
    );
    m_titleText.setPosition(
        static_cast<float>(windowSize.x) / 2.0f,
        80.0f
    );

    // 初始化提示文本
    m_hintText.setFont(rm.getFont());
    m_hintText.setString("按空格键开始点名 / 再次按下停止");
    m_hintText.setCharacterSize(24);
    m_hintText.setFillColor(sf::Color(200, 200, 200));
    
    sf::FloatRect hintBounds = m_hintText.getLocalBounds();
    m_hintText.setOrigin(
        hintBounds.left + hintBounds.width / 2.0f,
        hintBounds.top + hintBounds.height / 2.0f
    );
    m_hintText.setPosition(
        static_cast<float>(windowSize.x) / 2.0f,
        static_cast<float>(windowSize.y) - 60.0f
    );

    // 初始化状态文本
    m_statusText.setFont(rm.getFont());
    m_statusText.setCharacterSize(20);
    m_statusText.setFillColor(sf::Color(150, 150, 150));
    m_statusText.setPosition(20.0f, 20.0f);

    // 初始化滚动名字文本
    m_rollingNameText.setFont(rm.getFont());
    m_rollingNameText.setCharacterSize(72);
    m_rollingNameText.setFillColor(sf::Color::White);
    m_rollingNameText.setStyle(sf::Text::Bold);

    // 初始化角色显示
    m_characterDisplay.setMaxHeight(350.0f);
    m_characterDisplay.setPosition(
        static_cast<float>(windowSize.x) / 2.0f,
        static_cast<float>(windowSize.y) - 100.0f
    );
    m_characterDisplay.setNameFontSize(56);
    m_characterDisplay.setAnimationDuration(0.6f);

    // 尝试加载开场视频
    m_hasIntroVideo = m_videoPlayer.load(
        rm.getIntroVideoPath(),
        rm.getFrameAnimationDir()
    );

    if (m_hasIntroVideo) {
        // 视频居中显示，适配窗口大小
        m_videoPlayer.fitToSize(
            static_cast<float>(windowSize.x) * 0.8f,
            static_cast<float>(windowSize.y) * 0.8f
        );
        
        sf::Vector2f videoSize = m_videoPlayer.getSize();
        m_videoPlayer.setPosition(
            (static_cast<float>(windowSize.x) - videoSize.x) / 2.0f,
            (static_cast<float>(windowSize.y) - videoSize.y) / 2.0f
        );

        // 开始播放开场视频
        m_videoPlayer.play();
        m_state = State::Intro;
    } else {
        // 没有开场视频，直接进入准备状态
        std::cout << "[点名器] 未找到开场视频，直接进入准备状态" << std::endl;
        m_state = State::Ready;
    }

    // 更新状态文本
    updateStatusText();

    std::cout << "[点名器] 初始化完成" << std::endl;
    std::cout << "  窗口大小: " << windowSize.x << "x" << windowSize.y << std::endl;
    std::cout << "  可用人数: " << m_availableNames.size() << std::endl;
    std::cout << "  开场视频: " << (m_hasIntroVideo ? "已加载" : "未加载") << std::endl;

    return true;
}

// ==================== 事件处理 ====================
void NameRoller::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        switch (event.key.code) {
            case sf::Keyboard::Space:
                // 空格键控制
                if (m_state == State::Intro) {
                    // 开场动画中按空格跳过
                    skipIntro();
                } else if (m_state == State::Ready) {
                    // 准备状态开始点名
                    startRolling();
                } else if (m_state == State::Rolling) {
                    // 滚动中停止并显示结果
                    stopRolling();
                } else if (m_state == State::Result) {
                    // 结果状态重新开始
                    reset();
                }
                break;

            case sf::Keyboard::Escape:
                // ESC键退出或重置
                if (m_state == State::Result) {
                    reset();
                }
                break;

            case sf::Keyboard::R:
                // R键重置所有已点名
                if (event.key.control) {
                    m_calledNames.clear();
                    m_availableNames = ResourceManager::getInstance().getNames();
                    std::cout << "[点名器] 已重置点名记录" << std::endl;
                    updateStatusText();
                }
                break;

            default:
                break;
        }
    }
}

// ==================== 更新逻辑 ====================
void NameRoller::update(float deltaTime) {
    switch (m_state) {
        case State::Intro:
            updateIntro(deltaTime);
            break;

        case State::Rolling:
            updateRolling(deltaTime);
            break;

        case State::Result:
            updateResult(deltaTime);
            break;

        default:
            break;
    }

    // 更新角色显示动画
    m_characterDisplay.update(deltaTime);
}

// ==================== 开场动画更新 ====================
void NameRoller::updateIntro(float deltaTime) {
    if (!m_hasIntroVideo) {
        m_state = State::Ready;
        return;
    }

    // 更新视频播放
    m_videoPlayer.update(deltaTime);

    // 检查视频是否播放完成
    if (m_videoPlayer.getState() == VideoPlayer::State::Finished) {
        // 视频播放完毕，进入准备状态
        m_state = State::Ready;
        std::cout << "[点名器] 开场动画播放完毕" << std::endl;
    }
}

// ==================== 名字滚动更新 ====================
void NameRoller::updateRolling(float deltaTime) {
    m_rollTimer += deltaTime;

    // 计算名字切换间隔
    float interval = 1.0f / m_rollSpeed;

    // 每隔一段时间切换一个名字
    if (m_rollTimer >= interval) {
        m_rollTimer -= interval;
        selectNextName();
    }

    // 名字缩放动画（呼吸效果）
    if (m_nameGrowing) {
        m_nameScale += deltaTime * 2.0f;
        if (m_nameScale >= 1.1f) {
            m_nameScale = 1.1f;
            m_nameGrowing = false;
        }
    } else {
        m_nameScale -= deltaTime * 2.0f;
        if (m_nameScale <= 0.9f) {
            m_nameScale = 0.9f;
            m_nameGrowing = true;
        }
    }
}

// ==================== 结果显示更新 ====================
void NameRoller::updateResult(float deltaTime) {
    m_resultTimer += deltaTime;
}

// ==================== 渲染 ====================
void NameRoller::render(sf::RenderTarget& target) {
    // 渲染背景
    renderBackground(target);

    // 根据状态渲染不同内容
    switch (m_state) {
        case State::Intro:
            // 渲染开场视频
            if (m_hasIntroVideo) {
                m_videoPlayer.render(target);
            }
            
            // 渲染跳过提示
            {
                sf::Text skipText;
                skipText.setFont(ResourceManager::getInstance().getFont());
                skipText.setString("按空格键跳过");
                skipText.setCharacterSize(20);
                skipText.setFillColor(sf::Color(200, 200, 200, 150));
                
                sf::FloatRect bounds = skipText.getLocalBounds();
                skipText.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);
                skipText.setPosition(
                    static_cast<float>(m_windowSize.x) / 2.0f,
                    static_cast<float>(m_windowSize.y) - 40.0f
                );
                
                target.draw(skipText);
            }
            break;

        case State::Ready:
            // 渲染标题
            target.draw(m_titleText);
            
            // 渲染提示
            target.draw(m_hintText);
            break;

        case State::Rolling:
            // 渲染标题
            target.draw(m_titleText);

            // 渲染滚动中的名字
            {
                m_rollingNameText.setString(m_currentDisplayedName);
                m_rollingNameText.setScale(m_nameScale, m_nameScale);
                
                sf::FloatRect bounds = m_rollingNameText.getLocalBounds();
                m_rollingNameText.setOrigin(
                    bounds.left + bounds.width / 2.0f,
                    bounds.top + bounds.height / 2.0f
                );
                m_rollingNameText.setPosition(
                    static_cast<float>(m_windowSize.x) / 2.0f,
                    static_cast<float>(m_windowSize.y) / 2.0f - 50.0f
                );
                
                target.draw(m_rollingNameText);
            }
            break;

        case State::Result:
            // 渲染标题
            target.draw(m_titleText);

            // 渲染角色立绘和名字
            m_characterDisplay.render(target);
            break;

        default:
            break;
    }

    // 渲染UI（状态文本等）
    renderUI(target);
}

// ==================== 背景渲染 ====================
void NameRoller::renderBackground(sf::RenderTarget& target) {
    target.draw(m_background);

    // 可以在这里添加一些装饰性背景元素
    // 比如渐变效果、粒子效果等
}

// ==================== UI渲染 ====================
void NameRoller::renderUI(sf::RenderTarget& target) {
    // 渲染状态文本
    target.draw(m_statusText);

    // 右下角显示已点名/总人数
    if (m_state != State::Intro) {
        sf::Text countText;
        countText.setFont(ResourceManager::getInstance().getFont());
        countText.setString(
            "已点名: " + std::to_string(m_calledNames.size()) + 
            " / " + std::to_string(ResourceManager::getInstance().getNames().size())
        );
        countText.setCharacterSize(18);
        countText.setFillColor(sf::Color(150, 150, 150));
        
        sf::FloatRect bounds = countText.getLocalBounds();
        countText.setPosition(
            static_cast<float>(m_windowSize.x) - bounds.width - 20.0f,
            static_cast<float>(m_windowSize.y) - 30.0f
        );
        
        target.draw(countText);
    }
}

// ==================== 开始点名 ====================
void NameRoller::startRolling() {
    if (m_state != State::Ready) return;

    // 检查是否还有可用的名字
    if (!m_allowRepeat && m_availableNames.empty()) {
        std::cout << "[点名器] 所有人都已点过，重置点名记录" << std::endl;
        m_calledNames.clear();
        m_availableNames = ResourceManager::getInstance().getNames();
    }

    m_state = State::Rolling;
    m_rollTimer = 0.0f;
    m_currentDisplayedName = "";
    m_nameScale = 1.0f;
    m_nameGrowing = true;

    // 立即显示第一个名字
    selectNextName();

    // 隐藏角色显示
    m_characterDisplay.hide(false);

    updateStatusText();
    std::cout << "[点名器] 开始点名..." << std::endl;
}

// ==================== 停止点名 ====================
void NameRoller::stopRolling() {
    if (m_state != State::Rolling) return;

    // 确定最终选中的名字
    finalizeSelection();

    m_state = State::Result;
    m_resultTimer = 0.0f;

    updateStatusText();
    std::cout << "[点名器] 选中: " << m_selectedName << std::endl;
}

// ==================== 重置 ====================
void NameRoller::reset() {
    m_state = State::Ready;
    m_selectedName = "";
    m_characterDisplay.hide(true);
    updateStatusText();
}

// ==================== 跳过开场 ====================
void NameRoller::skipIntro() {
    if (m_state != State::Intro) return;

    m_videoPlayer.stop();
    m_state = State::Ready;
    updateStatusText();
    std::cout << "[点名器] 跳过开场动画" << std::endl;
}

// ==================== 选择下一个名字 ====================
void NameRoller::selectNextName() {
    const auto& names = ResourceManager::getInstance().getNames();
    if (names.empty()) return;

    // 随机选择一个名字
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, names.size() - 1);

    m_currentDisplayedName = names[dis(gen)];
}

// ==================== 确定最终选择 ====================
void NameRoller::finalizeSelection() {
    ResourceManager& rm = ResourceManager::getInstance();

    if (m_allowRepeat) {
        // 允许重复，直接随机选一个
        m_selectedName = rm.getRandomName();
    } else {
        // 不允许重复，从可用列表中选
        if (!m_availableNames.empty()) {
            static std::random_device rd;
            static std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(0, m_availableNames.size() - 1);

            size_t index = static_cast<size_t>(dis(gen));
            m_selectedName = m_availableNames[index];

            // 从可用列表移除，添加到已点名列表
            m_availableNames.erase(m_availableNames.begin() + index);
            m_calledNames.push_back(m_selectedName);
        } else {
            // 可用列表为空，重置
            m_calledNames.clear();
            m_availableNames = rm.getNames();
            m_selectedName = rm.getRandomName();
        }
    }

    // 显示角色立绘
    const sf::Texture& texture = rm.getCharacterTexture(m_selectedName);
    m_characterDisplay.setCharacter(m_selectedName, texture, true);
}

// ==================== 状态获取 ====================
NameRoller::State NameRoller::getState() const {
    return m_state;
}

// ==================== 设置滚动速度 ====================
void NameRoller::setRollSpeed(float speed) {
    m_rollSpeed = std::max(1.0f, speed);
}

// ==================== 设置是否允许重复 ====================
void NameRoller::setAllowRepeat(bool allow) {
    m_allowRepeat = allow;
    if (!allow) {
        // 如果切换到不允许重复模式，重置可用列表
        m_calledNames.clear();
        m_availableNames = ResourceManager::getInstance().getNames();
    }
}

// ==================== 更新状态文本 ====================
// 注意：这个是私有辅助方法，在头文件中没有声明，
// 但我们在cpp中实现它来减少代码重复
void NameRoller::updateStatusText() {
    std::string statusStr;
    
    switch (m_state) {
        case State::Intro:
            statusStr = "开场动画";
            break;
        case State::Ready:
            statusStr = "准备就绪";
            break;
        case State::Rolling:
            statusStr = "点名中...";
            break;
        case State::Result:
            statusStr = "点名结果";
            break;
        case State::Paused:
            statusStr = "已暂停";
            break;
    }

    m_statusText.setString("状态: " + statusStr);
}
