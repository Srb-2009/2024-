#ifndef VIDEO_PLAYER_H
#define VIDEO_PLAYER_H

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <memory>

// 条件编译：如果有sfeMovie库则使用，否则使用帧序列方案
#ifdef USE_SFE_MOVIE
#include <sfeMovie/Movie.hpp>
#endif

/**
 * @brief 视频播放器类
 * 
 * 支持两种视频播放模式：
 * 1. sfeMovie模式：播放真实的视频文件（mp4, avi等）
 * 2. 帧序列模式：使用图片序列模拟视频动画（兼容性更好）
 * 
 * 会自动根据编译环境选择可用的模式
 */
class VideoPlayer {
public:
    /**
     * @brief 播放状态枚举
     */
    enum class State {
        Stopped,    ///< 已停止
        Playing,    ///< 播放中
        Paused,     ///< 已暂停
        Finished    ///< 播放完成
    };

    VideoPlayer();
    ~VideoPlayer();

    /**
     * @brief 加载视频文件
     * @param videoPath 视频文件路径（sfeMovie模式）
     * @param framesDir 帧序列目录（降级模式）
     * @return 是否加载成功
     */
    bool load(const std::string& videoPath, const std::string& framesDir = "");

    /**
     * @brief 开始播放
     */
    void play();

    /**
     * @brief 暂停播放
     */
    void pause();

    /**
     * @brief 停止播放
     */
    void stop();

    /**
     * @brief 更新视频帧（每帧调用）
     * @param deltaTime 距上一帧的时间（秒）
     */
    void update(float deltaTime);

    /**
     * @brief 渲染视频到目标窗口
     * @param target 渲染目标
     */
    void render(sf::RenderTarget& target);

    /**
     * @brief 设置视频显示位置
     * @param x X坐标
     * @param y Y坐标
     */
    void setPosition(float x, float y);

    /**
     * @brief 设置视频缩放
     * @param scaleX 水平缩放
     * @param scaleY 垂直缩放
     */
    void setScale(float scaleX, float scaleY);

    /**
     * @brief 缩放到指定尺寸
     * @param width 目标宽度
     * @param height 目标高度
     */
    void fitToSize(float width, float height);

    /**
     * @brief 获取当前播放状态
     * @return 播放状态
     */
    State getState() const;

    /**
     * @brief 检查视频是否已加载
     * @return 是否已加载
     */
    bool isLoaded() const;

    /**
     * @brief 获取视频原始尺寸
     * @return 视频尺寸
     */
    sf::Vector2f getSize() const;

    /**
     * @brief 设置是否循环播放
     * @param loop 是否循环
     */
    void setLoop(bool loop);

    /**
     * @brief 设置播放速度
     * @param speed 播放速度（1.0为正常速度）
     */
    void setPlaybackSpeed(float speed);

private:
    /**
     * @brief 加载帧序列（降级方案）
     * @param framesDir 帧序列目录
     * @return 是否加载成功
     */
    bool loadFrameSequence(const std::string& framesDir);

    /**
     * @brief 更新帧序列动画
     * @param deltaTime 时间增量
     */
    void updateFrameAnimation(float deltaTime);

    // ==================== 成员变量 ====================
    State m_state;          ///< 当前播放状态
    bool m_loop;            ///< 是否循环播放
    float m_playbackSpeed;  ///< 播放速度
    float m_currentTime;    ///< 当前播放时间
    float m_duration;       ///< 总时长

    sf::Vector2f m_position;    ///< 显示位置
    sf::Vector2f m_scale;       ///< 缩放比例
    sf::Vector2f m_originalSize; ///< 原始尺寸

    // ========== sfeMovie模式（可选） ==========
#ifdef USE_SFE_MOVIE
    std::unique_ptr<sfe::Movie> m_movie;  ///< sfeMovie视频对象
#endif

    // ========== 帧序列模式（降级方案） ==========
    std::vector<sf::Texture> m_frames;    ///< 帧纹理列表
    sf::Sprite m_frameSprite;             ///< 帧精灵
    int m_currentFrame;                   ///< 当前帧索引
    float m_frameDuration;                ///< 每帧持续时间
    bool m_useFrameSequence;              ///< 是否使用帧序列模式
};

#endif // VIDEO_PLAYER_H
