// Hollywood Wayland Compositor
// SPDX-FileCopyrightText: 2021-2024 Originull Software
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

#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(hwRender)

class QOpenGLTexture;
class Compositor;
class SurfaceView;
class Surface;
class Output;
class WallpaperManager;
class WlrScreencopyFrameV1;
class OutputManager;
class OutputWindow : public QOpenGLWindow
{
    Q_OBJECT
public:
    OutputWindow();
    int width();
    int height();
    WallpaperManager* wallpaperManager();
    void setupScreenCopyFrame(WlrScreencopyFrameV1 *frame);
    void setOutput(Output *output);
protected:
    friend class OutputManager;
    friend class ServerSideDecoration;
    void initializeGL() override;
    void paintGL() override;

    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;

    void keyPressEvent(QKeyEvent *e) override;
    void keyReleaseEvent(QKeyEvent *e) override;
    void wheelEvent(QWheelEvent *) override;

    void touchEvent(QTouchEvent *) override;
    void mouseDoubleClickEvent(QMouseEvent *) override;
private slots:
    void startMove();
    void startResize(int edge, bool anchored);
    void startDrag(Surface *dragIcon);
    void readyForScreenCopy();
private:
    enum GrabState { NoGrab, MoveGrab, ResizeGrab, DragGrab };
    SurfaceView *viewAt(const QPointF &point);
    Surface* surfaceAt(const QPointF &point);
    bool mouseGrab() const { return m_grabState != NoGrab ;}
    void sendMouseEvent(QMouseEvent *e, SurfaceView *target);
    static QPointF getAnchoredPosition(const QPointF &anchorPosition, int resizeEdge, const QSize &windowSize);
    static QPointF getAnchorPosition(const QPointF &position, int resizeEdge, const QSize &windowSize);
    void recursiveDrawTextureForObject(Surface *obj);
    void drawTextureForObject(Surface *obj);
    void drawPopupsForObject(Surface *obj);
    void drawShadowForObject(uint shadowOffset, Surface *obj);
    void drawDesktopInfoString();
    void drawServerSideDecoration(Surface *obj);
private:
    friend class WallpaperManager;
    Output *m_output;
    QOpenGLTextureBlitter m_textureBlitter;
    QOpenGLShaderProgram *m_shadowShader;
    QOpenGLShaderProgram *m_rgbaShader;
    QOpenGLFramebufferObject *m_fbo = nullptr;

    QPointer<Surface> m_mouseSelectedSurfaceObject;
    GrabState m_grabState = NoGrab;
    QSize m_initialSize;
    int m_resizeEdge;
    bool m_resizeCursor = false;
    bool m_resizeAnchored;
    QPointF m_resizeAnchorPosition;
    QPointF m_mouseOffset;
    QPointF m_initialMousePos;
    Surface *m_dragIconSurfaceObject = nullptr;
    WallpaperManager *m_wpm = nullptr;

    QOpenGLBuffer m_textureBuffer;
    QOpenGLBuffer m_vertexBuffer;

    WlrScreencopyFrameV1 *m_copy_frame = nullptr;
    bool m_do_copy_frame = false;
    bool m_blackout = false;
};
