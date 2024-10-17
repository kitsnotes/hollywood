// Hollywood Wayland Compositor
// (C) 2021, 2022 Cat Stevenson <cat@originull.org>
// SPDX-License-Identifier: GPL-3.0-only

#ifndef VIEW_H
#define VIEW_H

#include <QWaylandCompositor>
#include <QWaylandSurface>
#include <QWaylandView>
#include <QWaylandWlShellSurface>
#include <QWaylandXdgSurface>
#include <QWaylandXdgShell>
#include <QTimer>
#include <QOpenGLTextureBlitter>
#include <QOpenGLContext>

#include "hwc.h"

#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(hwView)

class QWaylandWlShell;
class QWaylandWlShellSurface;
class QWaylandXdgShell;
class QOpenGLTexture;
class Surface;
class SurfaceView : public QWaylandView
{
    Q_OBJECT
public:
    SurfaceView(Surface *surface);
    Surface* surfaceObject() { return m_surface; }
    QOpenGLTexture *getTexture();
    QOpenGLTextureBlitter::Origin textureOrigin() const;
    bool isSharedMem();
    int nearEdge(QPointF point) const;

private:
    friend class Compositor;
    friend class Surface;
    Surface *m_surface = nullptr;
    GLenum m_textureTarget = GL_TEXTURE_2D;
    QOpenGLTexture *m_texture = nullptr;
    QOpenGLTextureBlitter::Origin m_origin;

    QSize m_size;
};

#endif // VIEW_H
