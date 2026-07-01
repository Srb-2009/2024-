#ifndef NAME_ROLLER_H
#define NAME_ROLLER_H

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <memory>

#include "VideoPlayer.h"
#include "CharacterDisplay.h"

/**
 * @brief 点名器核心类
 * 
 * 负责管理整个点名流程：
 * 1. 播放开场视频动画
 * 2. 随机滚动名字
 * 3. 显示最终选中的名字和立绘
 * 4. 处理用户交互
 */
class NameRoller {
public:
    /**
     * @brief 游戏状态枚举
     */
    enum class State {
        Intro,          ///< 开场动画阶段
        Ready,          ///< 准备就绪（等待开始点名）
        Rolling,        ///< 名字滚动中
        Result,         ///< 显示结果
        Paused          ///< 暂停状态
    };

    NameRoller();
    ~NameRoller() = default;

    /**
     * @brief 初始化点名器
     * @param windowSize 窗口大小
     * @return 是否初始化成功
     */
    bool init(const sf::Vector2u& windowSize);

    /**
     * @brief 处理事件
     * @param event SFML事件
     */
    void handleEvent(const sf::Event& event);

    /**
     * @brief 更新游戏逻辑（每帧调用）
     * @param deltaTime 距上一帧的时间（秒）
     */
    void update(float deltaTime);

    /**
     * @brief 渲染到目标窗口
     * @param target 渲染目标
     */
    void render(sf::RenderTarget& target);

    /**
     * @brief 开始点名
     */
    void startRolling();

    /**
     * @brief 停止滚动并显示结果
     */
    void stopRolling();

    /**
     * @brief 重新开始（回到准备状态）
     */
    void reset();

    /**
     * @brief 跳过开场动画
     */
    void skipIntro();

    /**
     * @brief 获取当前状态
     * @return 当前状态
     */
    State getState() const;

    /**
     * @brief 设置滚动速度
     * @param speed 滚动速度（名字/秒）
     */
    void setRollSpeed(float speed);

    /**
     * @brief 设置是否允许重复点名
     * @param allow 是否允许重复
     */
    void setAllowRepeat(bool allow);

private:
    /**
     * @brief 更新开场动画阶段
     * @param deltaTime 时间增量
     */
    void updateIntro(float deltaTime);

    /**
     * @brief 更新名字滚动阶段
     * @param deltaTime 时间增量
     */
    void updateRolling(float deltaTime);

    /**
     * @brief 更新结果显示阶段
     * @param deltaTime 时间增量
     */
    void updateResult(float deltaTime);

    /**
     * @brief 渲染背景
     * @param target 渲染目标
     */
    void renderBackground(sf::RenderTarget& target);

    /**
     * @brief 渲染UI元素
     * @param target 渲染目标
     */
    void renderUI(sf::RenderTarget& target);

    /**
     * @brief 选择下一个随机名字
     */
    void selectNextName();

    /**
     * @brief 确定最终选中的名字
     */
    void finalizeSelection();

    // ==================== 成员变量 ====================
    // 状态管理
    State m_state;              ///< 当前游戏状态
    sf::Vector2u m_windowSize;  ///< 窗口大小

    // 视频播放器
    VideoPlayer m_videoPlayer;  ///< 开场视频播放器
    bool m_hasIntroVideo;       ///< 是否有开场视频

    // 角色显示
    CharacterDisplay m_characterDisplay;  ///< 角色立绘显示

    // 名字滚动
    float m_rollSpeed;          ///< 滚动速度（名字/秒）
    float m_rollTimer;          ///< 滚动计时器
    std::string m_currentDisplayedName;  ///< 当前显示的名字
    sf::Text m_rollingNameText; ///< 滚动中的名字文本

    // 结果显示
    std::string m_selectedName; ///< 最终选中的名字
    float m_resultTimer;        ///< 结果显示计时器

    // UI元素
    sf::Text m_titleText;       ///< 标题文本
    sf::Text m_hintText;        ///< 提示文本
    sf::Text m_statusText;      ///< 状态文本

    // 背景
    sf::RectangleShape m_background;  ///< 背景矩形
    sf::Color m_bgColor;              ///< 背景颜色

    // 点名设置
    bool m_allowRepeat;        ///< 是否允许重复点名
    std::vector<std::string> m_availableNames;  ///< 可用名字列表
    std::vector<std::string> m_calledNames;     ///< 已点名列表

    // 滚动动画
    float m_nameScale;          ///< 名字缩放动画
    bool m_nameGrowing;         ///< 名字是否在放大
};

#endif // NAME_ROLLER_H
