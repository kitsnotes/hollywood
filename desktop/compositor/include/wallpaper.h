// Hollywood Wayland Compositor
// (C) 2022 Cat Stevenson <cat@originull.org>
// SPDX-License-Identifier: GPL-3.0-only

#ifndef WALLPAPER_H
#define WALLPAPER_H

#include <QObject>
#include <QTimer>
#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>
#include <QOpenGLFramebufferObject>

class OutputWindow;
class WallpaperManager : public QObject
{
    Q_OBJECT
public:
    enum Layout {
        FillScreen,
        FitToScreen,
        Stretch,
        Center
    };
    explicit WallpaperManager(OutputWindow *parent = nullptr);
    void setup();
public slots:
    void wallpaperChanged();
    void clearBackgroundColor();
    void renderWallpaper();
    void setupRotationTimer();
    void rotateNow();
signals:
private:
    void querySettings();
    QString findNextWallpaperInOrder();
    QString findNextWallpaperRandom();
    void renderTransition();
    void completeTransition();
    void setNewWallpaperPath(const QString &wallpaper);
private slots:
    void transitionWallpaper();
private:
    OutputWindow *m_parent;
    QTimer *m_rotationTimer = nullptr;
    QOpenGLTexture *m_texture = nullptr;
    const uchar *m_texturebits = nullptr;

    QOpenGLTexture *m_oldtexture = nullptr;
    const uchar *m_oldtexturebits = nullptr;
    QOpenGLShaderProgram *m_shader = nullptr;
    QOpenGLFramebufferObject *m_fbo = nullptr;
    float m_transprogress = 0.0f;
    QString m_wallpaper;
    QString m_color;
    Layout m_displayMode;
    QPoint m_wpStartPoint;
    bool m_rotate;
    uint m_rotateTime;
    uint m_rotateMode;
    QString m_rotateFolder;
    QSize m_bgSize;

    bool m_intrans = false;
};

#endif // WALLPAPER_H
