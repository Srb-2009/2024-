#include "VideoPlayer.h"
#include <iostream>
#include <filesystem>
#include <algorithm>

namespace fs = std::filesystem;

// ==================== 构造与析构 ====================
VideoPlayer::VideoPlayer()
    : m_state(State::Stopped)
    , m_loop(false)
    , m_playbackSpeed(1.0f)
    , m_currentTime(0.0f)
    , m_duration(0.0f)
    , m_position(0.0f, 0.0f)
    , m_scale(1.0f, 1.0f)
    , m_originalSize(0.0f, 0.0f)
    , m_currentFrame(0)
    , m_frameDuration(1.0f / 30.0f)  // 默认30fps
    , m_useFrameSequence(false)
{
#ifdef USE_SFE_MOVIE
    m_movie = std::make_unique<sfe::Movie>();
#endif
}

VideoPlayer::~VideoPlayer() {
    stop();
}

// ==================== 视频加载 ====================
bool VideoPlayer::load(const std::string& videoPath, const std::string& framesDir) {
    // 先尝试使用sfeMovie模式
#ifdef USE_SFE_MOVIE
    if (m_movie && !videoPath.empty()) {
        if (m_movie->openFromFile(videoPath)) {
            m_useFrameSequence = false;
            m_originalSize = sf::Vector2f(
                static_cast<float>(m_movie->getSize().x),
                static_cast<float>(m_movie->getSize().y)
            );
            m_duration = m_movie->getDuration().asSeconds();
            m_state = State::Stopped;
            m_currentTime = 0.0f;
            
            std::cout << "[视频播放器] 使用sfeMovie加载视频成功: " << videoPath << std::endl;
            std::cout << "  分辨率: " << m_originalSize.x << "x" << m_originalSize.y << std::endl;
            std::cout << "  时长: " << m_duration << "秒" << std::endl;
            return true;
        }
        std::cerr << "[视频播放器] sfeMovie加载失败: " << videoPath << std::endl;
    }
#endif

    // sfeMovie不可用或加载失败，尝试帧序列模式
    if (!framesDir.empty()) {
        if (loadFrameSequence(framesDir)) {
            m_useFrameSequence = true;
            m_state = State::Stopped;
            m_currentTime = 0.0f;
            m_currentFrame = 0;
            
            std::cout << "[视频播放器] 使用帧序列模式: " << framesDir << std::endl;
            std::cout << "  帧数: " << m_frames.size() << std::endl;
            std::cout << "  帧率: " << (1.0f / m_frameDuration) << "fps" << std::endl;
            std::cout << "  总时长: " << m_duration << "秒" << std::endl;
            return true;
        }
    }

    std::cerr << "[视频播放器] 所有视频加载方式均失败" << std::endl;
    return false;
}

// ==================== 帧序列加载 ====================
bool VideoPlayer::loadFrameSequence(const std::string& framesDir) {
    if (!fs::exists(framesDir)) {
        std::cerr << "[视频播放器] 帧序列目录不存在: " << framesDir << std::endl;
        return false;
    }

    // 收集所有图片文件
    std::vector<std::string> frameFiles;
    
    for (const auto& entry : fs::directory_iterator(framesDir)) {
        if (entry.is_regular_file()) {
            std::string ext = entry.path().extension().string();
            if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || 
                ext == ".bmp" || ext == ".PNG" || ext == ".JPG") {
                frameFiles.push_back(entry.path().string());
            }
        }
    }

    if (frameFiles.empty()) {
        std::cerr << "[视频播放器] 帧序列目录中没有图片文件" << std::endl;
        return false;
    }

    // 按文件名排序（确保帧顺序正确）
    std::sort(frameFiles.begin(), frameFiles.end());

    // 加载所有帧
    m_frames.clear();
    m_frames.reserve(frameFiles.size());

    for (const auto& file : frameFiles) {
        sf::Texture texture;
        if (texture.loadFromFile(file)) {
            texture.setSmooth(true);
            m_frames.push_back(texture);
        }
    }

    if (m_frames.empty()) {
        std::cerr << "[视频播放器] 帧加载失败" << std::endl;
        return false;
    }

    // 设置第一帧为当前帧
    m_frameSprite.setTexture(m_frames[0]);
    m_originalSize = sf::Vector2f(
        static_cast<float>(m_frames[0].getSize().x),
        static_cast<float>(m_frames[0].getSize().y)
    );

    // 计算总时长（默认30fps）
    m_duration = static_cast<float>(m_frames.size()) * m_frameDuration;

    return true;
}

// ==================== 播放控制 ====================
void VideoPlayer::play() {
    if (m_state == State::Playing) return;

    m_state = State::Playing;

#ifdef USE_SFE_MOVIE
    if (!m_useFrameSequence && m_movie) {
        m_movie->play();
    }
#endif
}

void VideoPlayer::pause() {
    if (m_state != State::Playing) return;

    m_state = State::Paused;

#ifdef USE_SFE_MOVIE
    if (!m_useFrameSequence && m_movie) {
        m_movie->pause();
    }
#endif
}

void VideoPlayer::stop() {
    m_state = State::Stopped;
    m_currentTime = 0.0f;
    m_currentFrame = 0;

#ifdef USE_SFE_MOVIE
    if (!m_useFrameSequence && m_movie) {
        m_movie->stop();
    }
#endif

    if (m_useFrameSequence && !m_frames.empty()) {
        m_frameSprite.setTexture(m_frames[0]);
    }
}

// ==================== 更新逻辑 ====================
void VideoPlayer::update(float deltaTime) {
    if (m_state != State::Playing) return;

    // 更新播放时间
    m_currentTime += deltaTime * m_playbackSpeed;

    // 检查是否播放完成
    if (m_currentTime >= m_duration) {
        if (m_loop) {
            // 循环播放
            m_currentTime = std::fmod(m_currentTime, m_duration);
#ifdef USE_SFE_MOVIE
            if (!m_useFrameSequence && m_movie) {
                m_movie->setPlayingOffset(sf::seconds(m_currentTime));
            }
#endif
        } else {
            // 播放结束
            m_currentTime = m_duration;
            m_state = State::Finished;
#ifdef USE_SFE_MOVIE
            if (!m_useFrameSequence && m_movie) {
                m_movie->pause();
            }
#endif
            return;
        }
    }

    // 帧序列模式更新
    if (m_useFrameSequence) {
        updateFrameAnimation(deltaTime);
    }
}

void VideoPlayer::updateFrameAnimation(float deltaTime) {
    if (m_frames.empty()) return;

    // 计算当前应该显示哪一帧
    int frameIndex = static_cast<int>(m_currentTime / m_frameDuration);
    frameIndex = frameIndex % static_cast<int>(m_frames.size());

    // 切换帧（避免重复设置）
    if (frameIndex != m_currentFrame) {
        m_currentFrame = frameIndex;
        m_frameSprite.setTexture(m_frames[m_currentFrame]);
    }
}

// ==================== 渲染 ====================
void VideoPlayer::render(sf::RenderTarget& target) {
    if (m_useFrameSequence) {
        // 帧序列模式
        m_frameSprite.setPosition(m_position);
        m_frameSprite.setScale(m_scale);
        target.draw(m_frameSprite);
    }
#ifdef USE_SFE_MOVIE
    else if (m_movie) {
        // sfeMovie模式
        m_movie->setPosition(m_position);
        m_movie->setScale(m_scale);
        target.draw(*m_movie);
    }
#endif
}

// ==================== 位置与缩放 ====================
void VideoPlayer::setPosition(float x, float y) {
    m_position = sf::Vector2f(x, y);
}

void VideoPlayer::setScale(float scaleX, float scaleY) {
    m_scale = sf::Vector2f(scaleX, scaleY);
}

void VideoPlayer::fitToSize(float width, float height) {
    if (m_originalSize.x == 0 || m_originalSize.y == 0) return;

    float scaleX = width / m_originalSize.x;
    float scaleY = height / m_originalSize.y;
    
    // 保持宽高比，取较小的缩放比例
    float scale = std::min(scaleX, scaleY);
    m_scale = sf::Vector2f(scale, scale);
}

// ==================== 状态查询 ====================
VideoPlayer::State VideoPlayer::getState() const {
    return m_state;
}

bool VideoPlayer::isLoaded() const {
#ifdef USE_SFE_MOVIE
    if (!m_useFrameSequence && m_movie) {
        return m_movie->getSize().x > 0;
    }
#endif
    return m_useFrameSequence && !m_frames.empty();
}

sf::Vector2f VideoPlayer::getSize() const {
    return sf::Vector2f(
        m_originalSize.x * m_scale.x,
        m_originalSize.y * m_scale.y
    );
}

void VideoPlayer::setLoop(bool loop) {
    m_loop = loop;
}

void VideoPlayer::setPlaybackSpeed(float speed) {
    m_playbackSpeed = speed;
}
