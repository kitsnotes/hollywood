// Hollywood Wayland Compositor
// SPDX-FileCopyrightText: 2021-2024 Originull Software
// SPDX-License-Identifier: GPL-3.0-only

#include "compositor.h"
#include "view.h"
#include "surfaceobject.h"

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
        if(buf.isSharedMemory())
        {
            if(m_texture)
                delete m_texture;
            m_texture = new QOpenGLTexture(buf.image());
        }
        else
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
    auto pm = 5; // the margin (in px) on each side of the handle
    int NoEdge = 0x0;
    if(!m_surface)
        return NoEdge;

    if(m_surface->isSpecialShellObject())
        return NoEdge;

    //point is global to the compositor output
    int left = m_surface->decoratedRect().left();
    int top = m_surface->decoratedRect().top();

    int bottom = top + m_surface->decoratedRect().height();
    int right = left + m_surface->decoratedRect().width();

    if(point.y() >= top-pm*2 &&
        point.y() <= top+pm &&
        point.x() >= left-pm*2 &&
        point.x() <= left+pm)
        return 0x03;

    if(point.y() >= top-pm*2 &&
        point.y() <= top+pm &&
        point.x() >= right-pm &&
        point.x() <= right+pm)
        return 0x05;

    if(point.y() >= bottom-pm &&
        point.y() <= bottom+pm &&
        point.x() >= left-pm*2 &&
        point.x() <= left+pm)
        return 0x10;

    if(point.y() >= bottom-pm &&
        point.y() <= bottom+pm &&
        point.x() >= right-pm &&
        point.x() <= right+pm)
        return 0x12;

    if(point.y() >= top-pm*2 &&
       point.y() <= top+pm)
        return 0x01;

    if(point.x() >= left-pm*2 &&
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

