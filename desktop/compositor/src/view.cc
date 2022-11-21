// Hollywood Wayland Compositor
// (C) 2021, 2022 Cat Stevenson <cat@originull.org>
// SPDX-License-Identifier: GPL-3.0-only

#include "view.h"
#include "surfaceobject.h"
#include "hwc.h"

#include <QOpenGLTexture>

#ifndef GL_TEXTURE_EXTERNAL_OES
#define GL_TEXTURE_EXTERNAL_OES 0x8D65
#endif

SurfaceView::SurfaceView(Surface *surface)
{
    m_surface = surface;
    setSurface(surface->surface());
}

QOpenGLTexture *SurfaceView::getTexture()
{
    bool newContent = advance();
    QWaylandBufferRef buf = currentBuffer();
    if (!buf.hasContent())
        m_texture = nullptr;
    if (newContent) {
        m_texture = buf.toOpenGLTexture();
        if (surface()) {
            m_size = surface()->destinationSize();
            m_origin = buf.origin() == QWaylandSurface::OriginTopLeft
                    ? QOpenGLTextureBlitter::OriginTopLeft
                    : QOpenGLTextureBlitter::OriginBottomLeft;
        }
    }

    return m_texture;
}

QOpenGLTextureBlitter::Origin SurfaceView::textureOrigin() const
{
    return m_origin;
}

bool SurfaceView::isSharedMem()
{
    QWaylandBufferRef buf = currentBuffer();
    return buf.isSharedMemory();
}

int SurfaceView::nearEdge(QPointF point) const
{
    auto pm = 3; // the margin (in px) on each side of the handle
    int NoEdge = 0x0;
    if(!m_surface)
        return NoEdge;

    if(m_surface->isSpecialShellObject())
        return NoEdge;

    //point is global to the compositor output
    int left = m_surface->position().x();
    int top = m_surface->position().y();

    int bottom = top + m_surface->size().height();
    int right = left + m_surface->size().width();

    if(m_surface->serverDecorated())
    {
        bottom = top + m_surface->decoratedRect().height();
        right = left + m_surface->decoratedRect().width();

    }
    if(point.y() >= top-pm &&
       point.y() <= top+pm)
        return 0x01;

    if(point.x() >= left-pm &&
       point.x() <= left+pm)
        return 0x02;

    if(point.x() >= right-pm &&
       point.x() <= right+pm)
        return 0x04;

    if(point.y() >= bottom-pm &&
       point.y() <= bottom+pm)
        return 0x08;

    return NoEdge;
}

