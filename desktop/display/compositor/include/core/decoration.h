// Hollywood Wayland Compositor
// SPDX-FileCopyrightText: 2024 Originull Software
// SPDX-License-Identifier: GPL-3.0-only

#pragma once
#include <QOpenGLWindow>
#include <QPointer>
#include <QOpenGLFramebufferObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLPaintDevice>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLTextureBlitter>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>

#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(hwSSDRender)

class Surface;
class OutputWindow;
class ServerSideDecoration : public QObject
{
    Q_OBJECT
public:
    explicit ServerSideDecoration(Surface *parent);
    ~ServerSideDecoration();
    void paintGL(OutputWindow *window, QOpenGLFramebufferObject *fbo);
    bool hasWindowIcon() const;
    bool hasMinimizeIcon() const;
    bool hasMaximizeIcon() const;
    bool hasCloseIcon() const;
private slots:
    void iconChanged();
private:
    QPoint decorationRenderStartPoint();
    void renderDecoration(OutputWindow *window);
    void renderGems(OutputWindow *window);
    void renderIcon(OutputWindow *window);
    void renderTitle(QOpenGLFramebufferObject *fbo);
    void scissorContentArea(OutputWindow *window);
    void createWindowGemTextures();
    void renderGem(QOpenGLFunctions *functions,
                   const QPoint &startPoint, const QSize &size, QOpenGLTexture *texture);
    void createWindowIconTexture();
    uint paddingSize() const;
    uint iconSize() const;
    uint fontSize() const;

private:
    Surface *m_parent;
    bool m_dark_mode = false;
    QColor m_fg_color;
    QColor m_bg_color;
    QColor m_stroke_color;

    QOpenGLTextureBlitter m_blitter;
    QOpenGLTexture *m_close_icon = nullptr;
    QOpenGLTexture *m_minimize_icon = nullptr;
    QOpenGLTexture *m_maximize_icon = nullptr;
    QOpenGLTexture *m_restore_icon = nullptr;
    QOpenGLTexture *m_window_icon = nullptr;
};
