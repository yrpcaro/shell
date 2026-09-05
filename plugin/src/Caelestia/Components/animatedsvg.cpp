#include "animatedsvg.hpp"

#include <qdatetime.h>
#include <qfile.h>
#include <qpainter.h>

#include <cmath>

namespace caelestia::components {

AnimatedSvg::AnimatedSvg(QQuickItem* parent)
    : QQuickPaintedItem(parent)
    , m_renderer(nullptr)
    , m_fillMode(FillMode::PreserveAspectFit)
    , m_playing(true)
    , m_speed(1.0)
    , m_lastWallTime(QDateTime::currentMSecsSinceEpoch())
    , m_virtualElapsed(0)
    , m_pausedFrame(0)
{
    setAntialiasing(true);
}

void AnimatedSvg::paint(QPainter* painter) {
    if (!m_renderer || !m_renderer->isValid())
        return;

    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setRenderHint(QPainter::SmoothPixmapTransform, true);

    // QSvgRenderer evaluates the SMIL/CSS timeline at paint time from
    // wall-clock time, so update() cadence only governs refresh rate, not
    // the animation position. Speed and pause are layered on top below.
    if (m_renderer->animated()) {
        if (!m_playing)
            freezeTimeline();
        else if (!qFuzzyCompare(m_speed, 1.0))
            applyVirtualTimeline();
    }

    m_renderer->render(painter, targetRect());
}

QUrl AnimatedSvg::source() const {
    return m_source;
}

void AnimatedSvg::setSource(const QUrl& source) {
    if (m_source == source)
        return;

    m_source = source;
    emit sourceChanged();
    reload();
}

AnimatedSvg::FillMode AnimatedSvg::fillMode() const {
    return m_fillMode;
}

void AnimatedSvg::setFillMode(FillMode fillMode) {
    if (m_fillMode == fillMode)
        return;

    m_fillMode = fillMode;
    emit fillModeChanged();
    update();
}

bool AnimatedSvg::isPlaying() const {
    return m_playing;
}

void AnimatedSvg::setPlaying(bool playing) {
    if (m_playing == playing)
        return;

    m_playing = playing;
    if (m_renderer && m_renderer->animated()) {
        if (!m_playing) {
            m_pausedFrame = m_renderer->currentFrame();
        } else {
            // Resume from the pinned frame
            m_lastWallTime = QDateTime::currentMSecsSinceEpoch();
            m_virtualElapsed = qreal(m_pausedFrame) / qMax(1, totalFrames()) * m_renderer->animationDuration();
        }
    }
    emit playingChanged();
    updatePlayState();
}

qreal AnimatedSvg::speed() const {
    return m_speed;
}

void AnimatedSvg::setSpeed(qreal speed) {
    if (qFuzzyCompare(m_speed, speed) || speed <= 0)
        return;

    m_speed = speed;
    if (m_renderer && m_renderer->animated() && m_playing) {
        // Rebase the virtual timeline onto the current playback position
        m_lastWallTime = QDateTime::currentMSecsSinceEpoch();
        m_virtualElapsed = qreal(m_renderer->currentFrame()) / qMax(1, totalFrames()) * m_renderer->animationDuration();
    }
    emit speedChanged();
}

QSizeF AnimatedSvg::svgDefaultSize() const {
    return m_renderer ? QSizeF(m_renderer->defaultSize()) : QSizeF();
}

void AnimatedSvg::reload() {
    if (m_renderer) {
        m_renderer->disconnect(this);
        m_renderer->deleteLater();
        m_renderer = nullptr;
    }

    const QString path = m_source.isLocalFile() ? m_source.toLocalFile() : m_source.toString();
    if (QFile::exists(path)) {
        m_renderer = new QSvgRenderer(this);
        m_renderer->setFramesPerSecond(60);
        connect(m_renderer, &QSvgRenderer::repaintNeeded, this, &AnimatedSvg::onRepaintNeeded);

        if (!m_renderer->load(path)) {
            m_renderer->deleteLater();
            m_renderer = nullptr;
        }
    }

    m_virtualElapsed = 0;
    m_pausedFrame = 0;
    m_lastWallTime = QDateTime::currentMSecsSinceEpoch();

    updateImplicitSize();
    updatePlayState();
    update();
}

void AnimatedSvg::onRepaintNeeded() {
    if (m_playing)
        update();
}

void AnimatedSvg::updatePlayState() {
    if (!m_renderer)
        return;

    // The renderer's internal timer drives repaintNeeded() cadence only;
    // stopping it freezes repaints, which is exactly the pause behaviour.
    m_renderer->setAnimationEnabled(m_playing && m_renderer->animated() && m_speed > 0);
    update();
}

void AnimatedSvg::updateImplicitSize() {
    const QSizeF defaultSize = svgDefaultSize();
    if (!defaultSize.isEmpty())
        setImplicitSize(defaultSize.width(), defaultSize.height());
}

int AnimatedSvg::totalFrames() const {
    return m_renderer ? m_renderer->framesPerSecond() * m_renderer->animationDuration() / 1000 : 0;
}

void AnimatedSvg::applyVirtualTimeline() {
    const qint64 now = QDateTime::currentMSecsSinceEpoch();
    m_virtualElapsed += static_cast<qreal>(now - m_lastWallTime) * m_speed;
    m_lastWallTime = now;

    const int frames = totalFrames();
    const int dur = m_renderer->animationDuration();
    if (frames > 0 && dur > 0) {
        const qreal wrapped = std::fmod(m_virtualElapsed, qreal(dur));
        m_renderer->setCurrentFrame(int(wrapped / dur * frames));
    }
}

void AnimatedSvg::freezeTimeline() {
    const int frames = totalFrames();
    if (frames > 0 && m_renderer->animationDuration() > 0)
        m_renderer->setCurrentFrame(m_pausedFrame);
}

QRectF AnimatedSvg::targetRect() const {
    const QRectF bounds = boundingRect();
    if (m_fillMode == FillMode::Stretch || m_renderer == nullptr)
        return bounds;

    const QSizeF srcSize = svgDefaultSize();
    if (srcSize.isEmpty())
        return bounds;

    const qreal scale = m_fillMode == FillMode::PreserveAspectFit
        ? qMin(bounds.width() / srcSize.width(), bounds.height() / srcSize.height())
        : qMax(bounds.width() / srcSize.width(), bounds.height() / srcSize.height());
    const QSizeF scaled = srcSize * scale;
    return QRectF(
        bounds.x() + (bounds.width() - scaled.width()) / 2,
        bounds.y() + (bounds.height() - scaled.height()) / 2,
        scaled.width(),
        scaled.height());
}

} // namespace caelestia::components
