#pragma once

#include <qobject.h>
#include <qqmlintegration.h>
#include <qquickpainteditem.h>
#include <qsvgrenderer.h>
#include <qurl.h>

namespace caelestia::components {

// Renders animated (SMIL/CSS) and static SVGs. QML's built-in Image element
// only shows the first frame of an animated SVG, so QSvgRenderer is used to
// drive frame playback instead.
class AnimatedSvg : public QQuickPaintedItem {
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged FINAL)
    Q_PROPERTY(FillMode fillMode READ fillMode WRITE setFillMode NOTIFY fillModeChanged FINAL)
    Q_PROPERTY(bool playing READ isPlaying WRITE setPlaying NOTIFY playingChanged FINAL)
    // Playback rate multiplier; 1 is natural speed.
    Q_PROPERTY(qreal speed READ speed WRITE setSpeed NOTIFY speedChanged FINAL)

public:
    // Values deliberately mirror QtQuick.Image's fill modes
    enum class FillMode : quint8 {
        Stretch = 0,
        PreserveAspectFit = 1,
        PreserveAspectCrop = 2,
    };
    Q_ENUM(FillMode)

    explicit AnimatedSvg(QQuickItem* parent = nullptr);

    void paint(QPainter* painter) override;

    [[nodiscard]] QUrl source() const;
    void setSource(const QUrl& source);

    [[nodiscard]] FillMode fillMode() const;
    void setFillMode(FillMode fillMode);

    [[nodiscard]] bool isPlaying() const;
    void setPlaying(bool playing);

    [[nodiscard]] qreal speed() const;
    void setSpeed(qreal speed);

    // Natural size of the loaded SVG in user units
    [[nodiscard]] QSizeF svgDefaultSize() const;

signals:
    void sourceChanged();
    void fillModeChanged();
    void playingChanged();
    void speedChanged();

private slots:
    void onRepaintNeeded();

private:
    void reload();
    void updatePlayState();
    void updateImplicitSize();
    void applyVirtualTimeline();
    void freezeTimeline();
    [[nodiscard]] QRectF targetRect() const;
    [[nodiscard]] int totalFrames() const;

    QSvgRenderer* m_renderer;
    QUrl m_source;
    FillMode m_fillMode;
    bool m_playing;
    qreal m_speed;

    // SMIL playback runs on wall-clock time inside QSvgRenderer. To support
    // pause and speed we maintain a virtual timeline: while playing at a
    // non-natural speed each paint maps virtual elapsed time onto the
    // renderer's frame index; while paused the frame is pinned.
    qint64 m_lastWallTime;
    qreal m_virtualElapsed;
    int m_pausedFrame;
};

} // namespace caelestia::components
