// Hollywood Wayland Compositor
// SPDX-FileCopyrightText: 2021-2024 Originull Software
// SPDX-License-Identifier: GPL-3.0-only

#include "outputwnd.h"
#include "compositor.h"

// include gles for x64
#include <GLES3/gl3.h>

#include <QMouseEvent>
#include <QOpenGLWindow>
#include <QOpenGLTexture>
#include <QOpenGLFunctions>
#include <QOpenGLFramebufferObject>
#include <QOpenGLBuffer>
#include <QMatrix4x4>
#include <QPainter>
#include <QtWaylandCompositor/qwaylandseat.h>
#include <QOpenGLPaintDevice>
#include <QGuiApplication>
#include <QPalette>
#include <QPainterPath>
#include <QStaticText>
#include <QOpenGLFramebufferObject>
#include <QOpenGLFramebufferObjectFormat>
#include <QOpenGLFunctions>
#include <QOpenGLContext>

#include <QRectF>
#include <hollywood/hollywood.h>

#include "blitter.h"

#include "view.h"
#include "wallpaper.h"
#include "output.h"
#include "surfaceobject.h"
#include "shortcuts.h"
#include "relativepointer.h"
#include "screencopy.h"

Q_LOGGING_CATEGORY(hwRender, "compositor.render")

unsigned int VBO;

static const GLfloat vertex_buffer_data[] {
     -1,-1,0,
     -1,1,0,
     1,-1,0,
     -1,1,0,
     1,-1,0,
     1,1,0
};

static const GLfloat texture_buffer_data[] {
    0,0,
    0,1,
    1,0,
    0,1,
    1,0,
    1,1
};


OutputWindow::OutputWindow(Output* parent)
    :m_output(parent)
    ,m_shadowShader(new QOpenGLShaderProgram(this))
    ,m_rgbShader(new QOpenGLShaderProgram(this))
    ,m_rgbaShader(new QOpenGLShaderProgram(this))
    ,m_wpm(new WallpaperManager(this))
    ,m_rgb_vao(new QOpenGLVertexArrayObject)
{
    connect(hwComp, &Compositor::startMove, this, &OutputWindow::startMove);
    connect(hwComp, &Compositor::startResize, this, &OutputWindow::startResize);
    connect(hwComp, &Compositor::dragStarted, this, &OutputWindow::startDrag);
}

int OutputWindow::width()
{
    return size().width();
}

int OutputWindow::height()
{
    return size().height();
}

void OutputWindow::setupScreenCopyFrame(WlrScreencopyFrameV1 *frame)
{
    m_copy_frame = frame;
    connect(frame, &WlrScreencopyFrameV1::ready, this, &OutputWindow::readyForScreenCopy);
}

void OutputWindow::initializeGL()
{
    m_rgbShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/rgbconv.vsh");
    m_rgbShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/Shaders/rgbconv.fsh");
    m_rgbShader->link();
    m_rgbaShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/rgbconv.vsh");
    m_rgbaShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/Shaders/rgba.fsh");
    m_rgbaShader->link();
    m_rgbaShader->bind();
    QOpenGLVertexArrayObject::Binder vaoBinder(m_rgb_vao.data());

    m_vertexBuffer.create();
    m_vertexBuffer.bind();
    m_vertexBuffer.allocate(vertex_buffer_data, sizeof(vertex_buffer_data));
    m_vertexBuffer.release();

    m_textureBuffer.create();
    m_textureBuffer.bind();
    m_textureBuffer.allocate(texture_buffer_data, sizeof(texture_buffer_data));
    m_textureBuffer.release();
    m_rgbaShader->release();

    m_textureBlitter.create();
    m_wpm->setup();
    m_shadowShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/shadow.vsh");
    m_shadowShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/Shaders/shadow.fsh");
    m_shadowShader->link();
}

void OutputWindow::paintGL()
{
    // see if we just fell asleep, if so, black out and stop
    if(hwComp->sleeping())
    {
        glClearColor(0.0f,0.0f,0.0f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        return;
    }

    hwComp->startRender();
    // render our background & wallpaper
    m_wpm->clearBackgroundColor();

    if(!hwComp->isRunningLoginManager() && !hwComp->miniMode())
        m_wpm->renderWallpaper();

    // draw our desktops (bottom most) - we can't do this recursively
    // because the pop-ups will be under z-order things
    for(Surface *obj : hwComp->backgroundLayerSurfaces())
        drawTextureForObject(obj);

    for(Surface *obj : hwComp->bottomLayerSurfaces())
        drawTextureForObject(obj);

    /*if(!hwComp->isRunningLoginManager() && !hwComp->miniMode())
        drawDesktopInfoString(); */

    // draw standard surfaces
    for(Surface *obj : hwComp->surfaceByZOrder())
        recursiveDrawTextureForObject(obj);

    // now let's draw our popups for the backgroundLayerSurfaces
    for(Surface *obj : hwComp->backgroundLayerSurfaces())
        drawPopupsForObject(obj);

    // now let's draw our popups for the bottomLayerSurfaces
    for(Surface *obj : hwComp->bottomLayerSurfaces())
        drawPopupsForObject(obj);

    for(Surface *obj : hwComp->topLayerSurfaces())
        recursiveDrawTextureForObject(obj);

    for(Surface *obj : hwComp->overlayLayerSurfaces())
        recursiveDrawTextureForObject(obj);

    // if we need to take a screenshot do that here
    if(m_do_copy_frame)
    {
        if(m_copy_frame != nullptr)
        {
            m_do_copy_frame = false;
            m_copy_frame->copy();
            disconnect(m_copy_frame, &WlrScreencopyFrameV1::ready, this, &OutputWindow::readyForScreenCopy);
            m_copy_frame = nullptr;
        }
    }
    hwComp->endRender();
}

QPointF OutputWindow::getAnchorPosition(const QPointF &position, int resizeEdge, const QSize &windowSize)
{
    float y = position.y();
    if (resizeEdge & Qt::TopEdge)
        y += windowSize.height();

    float x = position.x();
    if (resizeEdge & Qt::LeftEdge)
        x += windowSize.width();

    return QPointF(x, y);
}

QPointF OutputWindow::getAnchoredPosition(const QPointF &anchorPosition, int resizeEdge, const QSize &windowSize)
{
    return anchorPosition - getAnchorPosition(QPointF(), resizeEdge, windowSize);
}

void OutputWindow::recursiveDrawTextureForObject(Surface *obj)
{
    if(obj->isCursor())
        return;   /* we're not drawing a cursor here */

    if(obj->isMinimized())
        return;

    drawTextureForObject(obj);
    for(Surface *child : obj->childSurfaceObjects())
        recursiveDrawTextureForObject(child);
}

void OutputWindow::drawTextureForObject(Surface *obj)
{
    if(obj == nullptr)
        return;

    if(!obj->surfaceReadyToRender())
        return;

    if(!hwComp->surfaceObjects().contains(obj))
    {
        qCDebug(hwRender, "drawTextureForObject: attempting to render recycled object");
        return;
    }

    bool useShadow = obj->shadowSize() > 1 ? true : false;

    QRect dispRect(m_output->wlOutput()->position(), m_output->size());
    QRect objRect(obj->decoratedRect().toRect());

    if(!objRect.intersects(dispRect))
        return;

    GLenum currentTarget = GL_TEXTURE_2D;
    auto texture = obj->viewForOutput(m_output)->getTexture();
    if (!texture)
        return;

    if (texture->target() != currentTarget)
        currentTarget = texture->target();

    if ((obj->surface() && obj->surface()->hasContent()) || obj->viewForOutput(m_output)->isBufferLocked())
    {
        // this comes in surface device independent pixels (ie: 1x)
        QSize s = obj->surfaceSize();
        s = s*obj->surface()->bufferScale();
        if (!s.isEmpty())
        {
            QOpenGLFunctions *functions = context()->functions();

            auto surfaceOrigin = obj->primaryView()->textureOrigin();
            if(m_fbo != nullptr)
            {
                try {
                    delete m_fbo;
                    m_fbo = nullptr;
                }
                catch(int e)
                {
                    qCCritical(hwRender, "failed to delete FBO %i)", e);
                }
            }

            bool use_fbo = true;
            if(obj->isFullscreenShell() || obj->isFullscreen())
                use_fbo = false;

            if(obj->isSubsurface())
                use_fbo = false;

            if(use_fbo)
            {
                // we use FBO's to construct a final thing to ouptut
                // including shadow, server decorations
                QOpenGLFramebufferObjectFormat fbofmt;
                fbofmt.setInternalTextureFormat(QOpenGLTexture::RGBAFormat);
                fbofmt.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);

                QSize dsize = obj->renderSize();
                m_fbo = new QOpenGLFramebufferObject(dsize, fbofmt);
                m_fbo->bind();
                functions->glViewport(0,0,dsize.width(),dsize.height());
                functions->glClearColor(0.0f,0.0f,0.0f,0.0f);
                functions->glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

                if(useShadow)
                    drawShadowForObject(obj->shadowSize(), obj);

                if(obj->serverDecorated())
                    drawServerSideDecoration(obj);

                functions->glEnable(GL_BLEND);
                functions->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                m_fbo->release();

                // switch back to our primary scene and blit fbo contents onto it
                functions->glViewport(0,0,size().width(),size().height());
                m_textureBlitter.bind();
                functions->glEnable(GL_BLEND);
                functions->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

                QRectF source = QRectF(obj->renderPosition(), obj->renderSize());

                // blit the decorations/shadow from fbo
                QMatrix4x4 tt_fbo = QOpenGLTextureBlitter::targetTransform(source,
                                                        QRect(m_output->wlOutput()->position(), size()));

                m_textureBlitter.blit(m_fbo->texture(), tt_fbo,
                                      QOpenGLTextureBlitter::OriginBottomLeft,
                                      QOpenGLTexture::RGBAFormat);
                m_textureBlitter.release();
                functions->glDisable(GL_BLEND);
            }

            functions->glEnable(GL_BLEND);
            functions->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            // render the actual surface content
            QRect source = QRect(obj->surfacePosition().toPoint(), obj->surface()->destinationSize()*obj->surface()->bufferScale());

            QMatrix4x4 tt_surface = QOpenGLTextureBlitter::targetTransform(source,
                                      QRect(m_output->wlOutput()->position(), size()));

            auto sourceGeom = obj->surface()->sourceGeometry();
            if(sourceGeom.height() != obj->surface()->bufferSize().height())
            {
                // we have a viewport - offset properly
                auto offset = obj->surface()->bufferSize().height() - sourceGeom.height();
                sourceGeom.moveTop(offset);
            }

            QMatrix3x3 src_transform = QOpenGLTextureBlitter::sourceTransform(sourceGeom,
                                                                              obj->surface()->bufferSize()/obj->surface()->bufferScale(),
                                                                              surfaceOrigin);
            m_textureBlitter.bind(currentTarget);
            m_textureBlitter.blit(texture->textureId(), tt_surface, src_transform, texture->format());
            m_textureBlitter.release();

            functions->glDisable(GL_BLEND);
       }
    }
}

void OutputWindow::drawPopupsForObject(Surface *obj)
{
    if(obj->isCursor())
        return;   /* we're not drawing a cursor here */

    if(obj->isMinimized())
        return;

    for(Surface *child : obj->childSurfaceObjects())
        recursiveDrawTextureForObject(child);
}

void OutputWindow::drawShadowForObject(uint shadowOffset, Surface *obj)
{
    if(obj->isCursor())
        return;

    if(obj == m_dragIconSurfaceObject)
        return;

    uint sm = shadowOffset;

    QOpenGLFunctions *functions = context()->functions();
    functions->glEnable(GL_BLEND);
    functions->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    auto angle = 95;
    auto z = 1+sin(angle*4);
    auto r = 28 + z;
    float corner = r * (0.5 + 0.5 * cos(2*angle));
    float sigma = z*10;

    functions->glUseProgram(m_shadowShader->programId());
    m_shadowShader->bind();
    glGenBuffers(1,&VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    float data[] = {0,0,1,0,0,1,1,1};
    glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, false, 0, 0);

    auto bs = hwComp->borderSize();
    auto ds = hwComp->decorationSize();

    uint hsm = sm/2;
    // box: x, y, width, height
    // opengl uses a bottom left point
    //int starty = m_fbo->height() - hsm - obj->decoratedRect().height() - hsm;
    int startx = hsm;
    int starty = hsm;
    int width = m_fbo->width() - hsm;
    int height = m_fbo->height() - hsm;
    // xmin ymin xmax ymax
    m_shadowShader->setUniformValue("box", startx, starty,
                                    height,width);
    if(obj->isXdgPopup())
        m_shadowShader->setUniformValue("color", 0, 0, 0, 0.30);
    else
        m_shadowShader->setUniformValue("color", 0, 0, 0, 0.45);
    m_shadowShader->setUniformValue("sigma", sigma);
    m_shadowShader->setUniformValue("corner", corner);
    m_shadowShader->setUniformValue("window", m_fbo->height(), m_fbo->width());
    functions->glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    m_shadowShader->release();

    auto rect = QRect(sm+bs,sm+bs,obj->surfaceSize().width(),obj->surfaceSize().height());
    // offset our rect start for titlebars
    if(obj->serverDecorated())
        rect.setHeight(rect.height()+ds);

    functions->glEnable(GL_SCISSOR_TEST);
    functions->glScissor(rect.x(), rect.y(), rect.width(), rect.height());
    functions->glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    functions->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    functions->glDisable(GL_SCISSOR_TEST);
    functions->glDisable(GL_BLEND);
}

void OutputWindow::drawDesktopInfoString()
{
    QImage* img = hwComp->desktopLabelImage();

    QOpenGLFunctions *functions = context()->functions();
    m_textureBlitter.bind();
    functions->glEnable(GL_BLEND);
    functions->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    QOpenGLTexture texture(*img);
    texture.bind();
    QPoint startPoint(m_output->wlOutput()->availableGeometry().right()-img->width()
                      ,m_output->wlOutput()->availableGeometry().bottom()-img->height());

    QMatrix4x4 tf = QOpenGLTextureBlitter::targetTransform(QRect(startPoint, img->size()),
                                                 QRect(QPoint(0,0), size()));
    m_textureBlitter.blit(texture.textureId(), tf, QOpenGLTextureBlitter::OriginTopLeft, texture.format());
    texture.release();
    texture.destroy();
    m_textureBlitter.release();
}

void OutputWindow::drawServerSideDecoration(Surface *obj)
{
    if(obj->isSpecialShellObject())
        return;

    if(obj->xdgTopLevel() == nullptr && obj->qtSurface() == nullptr)
        return;

    if(obj->decorationImage() == nullptr)
        obj->renderDecoration();

    QImage* img = obj->decorationImage();

    QOpenGLFunctions *functions = context()->functions();
    m_textureBlitter.bind();
    functions->glEnable(GL_BLEND);
    functions->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    QOpenGLTexture texture(*img);
    texture.bind();
    auto fbo_sz = obj->renderSize();
    auto ss = obj->shadowSize();
    QMatrix4x4 targetTransform = QOpenGLTextureBlitter::targetTransform(QRect(ss,ss,obj->decoratedSize().width(), obj->decoratedSize().height()),
                              QRect(0,0,fbo_sz.width(),fbo_sz.height()));
    m_textureBlitter.blit(texture.textureId(), targetTransform, QOpenGLTextureBlitter::OriginTopLeft, texture.format());
    texture.release();
    texture.destroy();
    m_textureBlitter.release();
}

SurfaceView *OutputWindow::viewAt(const QPointF &point)
{
    // TODO: remove this ugly function
    Surface *surface = surfaceAt(point);
    if(surface)
    {
        SurfaceView* view = surface->viewForOutput(m_output);
        if(view)
            return view;
    }
    return nullptr;
}

Surface* OutputWindow::surfaceAt(const QPointF &point)
{
    Surface *ret = nullptr;
    QPoint adjustedPoint(m_output->wlOutput()->position().x()+point.x(),
                 m_output->wlOutput()->position().y()+point.y());

    for(auto surf : hwComp->overlayLayerSurfaces())
    {
        QRectF geom(surf->surfacePosition(), surf->surface()->bufferSize()*surf->surface()->bufferScale());
        if (geom.contains(point))
            ret = surf;

        // check children of the menuserver
        for(auto *s : surf->childSurfaceObjects())
        {
            QRectF geom(s->surfacePosition(), s->surface()->bufferSize()*surf->surface()->bufferScale());
            if (geom.contains(adjustedPoint))
                ret = s;
        }
    }

    for(auto surf : hwComp->topLayerSurfaces())
    {
        QRectF geom(surf->surfacePosition(), surf->surface()->bufferSize()*surf->surface()->bufferScale());
        if (geom.contains(point))
            ret = surf;

        // check children of the tolayer surfaces
        for(auto *s : surf->childSurfaceObjects())
        {
            QRectF geom(s->surfacePosition(), s->surface()->bufferSize()*surf->surface()->bufferScale());
            if (geom.contains(adjustedPoint))
                ret = s;
        }
    }

    // we should check for children of the bottom layer surfaces
    if(ret == nullptr)
    {
        const auto views = hwComp->bottomLayerSurfaces();
        for (auto *s : views) {
            for(auto *surface : s->childSurfaceObjects())
            {
                if (surface == m_dragIconSurfaceObject)
                    continue;
                QRectF geom(surface->surfacePosition(), surface->surface()->bufferSize()*surface->surface()->bufferScale());
                if (geom.contains(adjustedPoint))
                    ret = surface;
            }
        }
    }

    // we should check for children of the background layer surfaces
    if(ret == nullptr)
    {
        const auto views = hwComp->backgroundLayerSurfaces();
        for (auto *s : views)
        {
            for(auto *surface : s->childSurfaceObjects())
            {
                if (surface == m_dragIconSurfaceObject)
                    continue;
                QRectF geom(surface->surfacePosition(), surface->surface()->bufferSize()*surface->surface()->bufferScale());
                if (geom.contains(adjustedPoint))
                    ret = surface;
            }
        }
    }

    // go through our standard surfaces by zorder
    if(ret == nullptr)
    {
        const auto views = hwComp->surfaceByZOrder();
        for (auto *surface : views) {
            if (surface == m_dragIconSurfaceObject)
                continue;
            if(surface->isMinimized())
                continue;
            if(!surface->surface())
                continue;
            if (surface->decoratedRect().contains(point))
                ret = surface;
            for(auto s : surface->childSurfaceObjects())
            {
                if (s->decoratedRect().contains(adjustedPoint))
                    ret = s;
            }
        }
    }

    // target the actual bottom layer surface
    if(ret == nullptr)
    {
        const auto views = hwComp->bottomLayerSurfaces();
        for (auto *surface : views) {
            if (surface == m_dragIconSurfaceObject)
                continue;
            QRectF geom(surface->surfacePosition(), surface->surface()->bufferSize()*surface->surface()->bufferScale());
            if (geom.contains(adjustedPoint))
                ret = surface;
        }
    }

    // target the actual background surface
    if(ret == nullptr)
    {
        const auto views = hwComp->backgroundLayerSurfaces();
        for (auto *surface : views) {
            QRectF geom(surface->surfacePosition(), surface->surface()->bufferSize()*surface->surface()->bufferScale());
            if (geom.contains(adjustedPoint))
                ret = surface;
        }
    }

    return ret;
}

void OutputWindow::startMove()
{
    if(m_mouseSelectedSurfaceObject == nullptr)
    {
        qCDebug(hwRender, "startMove: mouseSelectedSurfaceObject is null");
        return;
    }
    m_grabState = MoveGrab;
    m_mouseSelectedSurfaceObject->startMove();
}

void OutputWindow::startResize(int edge, bool anchored)
{
    if(m_mouseSelectedSurfaceObject == nullptr)
    {
        qCDebug(hwRender, "startResize: mouseSelectedSurfaceObject is null");
        return;
    }
    m_initialSize = m_mouseSelectedSurfaceObject->surfaceSize();
    m_grabState = ResizeGrab;
    m_resizeEdge = edge;
    m_resizeAnchored = anchored;
    m_resizeAnchorPosition = getAnchorPosition(m_mouseSelectedSurfaceObject->surfacePosition(),
                       edge, m_mouseSelectedSurfaceObject->surface()->destinationSize());
}

void OutputWindow::startDrag(Surface *dragIcon)
{
    if(m_mouseSelectedSurfaceObject == nullptr)
    {
        qCDebug(hwRender, "startDrag: mouseSelectedSurfaceObject is null");
        return;
    }
    m_grabState = DragGrab;
    m_dragIconSurfaceObject = dragIcon;
    hwComp->raise(dragIcon);
}

void OutputWindow::readyForScreenCopy()
{
    m_do_copy_frame = true;
    hwComp->triggerRender();
}

void OutputWindow::mousePressEvent(QMouseEvent *e)
{
    if(hwComp->sleeping())
        hwComp->wake();

    if (mouseGrab())
        return;

    QPoint adjustedPoint(m_output->wlOutput()->position().x()+e->position().x(),
                 m_output->wlOutput()->position().y()+e->position().y());

    QMouseEvent e2(e->type(), adjustedPoint, adjustedPoint, e->button(), e->buttons(),
                   e->modifiers(), e->pointingDevice());

    if (m_mouseSelectedSurfaceObject.isNull())
    {
        m_mouseSelectedSurfaceObject = surfaceAt(e->position());
        if (!m_mouseSelectedSurfaceObject) {
            // TODO: close xdg_popups
            return;
        }

        auto mouseedge = m_mouseSelectedSurfaceObject->viewForOutput(m_output)
                             ->nearEdge(adjustedPoint);
        if (e->modifiers() == Qt::AltModifier
            || e->modifiers() == Qt::MetaModifier)
        {
            m_grabState = MoveGrab; //start move
            m_mouseSelectedSurfaceObject->startMove();
        }
        else if(m_mouseSelectedSurfaceObject->serverDecorated() &&
                m_mouseSelectedSurfaceObject->titleBarRect().contains(adjustedPoint))
        {
            bool buttonHit = false;
            auto s = m_mouseSelectedSurfaceObject;
            // do some things to see if we're hitting
            // minimize, max, close, etc
            if(s->canMinimize())
            {
                if(s->minimizeButtonRect().contains(adjustedPoint))
                {
                    s->setMinimized();
                    buttonHit = true;
                }
            }

            if(s->canMaximize())
            {
                if(s->maximizeButtonRect().contains(adjustedPoint))
                {
                    if(s->isMaximized())
                        s->unsetMaximized();
                    else
                        s->setMaximized();
                    buttonHit = true;
                }
            }

            if(s->canClose())
            {
                if(s->closeButtonRect().contains(adjustedPoint))
                {
                    buttonHit = true;
                    s->sendClose();
                }
            }

            // Handle a server side decoration move
            // only move if we're in a normal (not maximized/fullscreen) state
            if(!buttonHit && !s->isMaximized()
                    &&  !s->isFullscreen()
                    && mouseedge == 0x0)
            {
                hwComp->raise(m_mouseSelectedSurfaceObject);
                m_grabState = MoveGrab;
                m_mouseSelectedSurfaceObject->startMove();

            }
            if(mouseedge != 0x0)
            {
                m_grabState = ResizeGrab;
                m_initialMousePos = adjustedPoint;
                m_mouseOffset = adjustedPoint - m_mouseSelectedSurfaceObject->surfacePosition();
                m_resizeEdge = mouseedge;
                m_initialSize = m_mouseSelectedSurfaceObject->surfaceSize();
                return;
            }
        }
        else if(m_mouseSelectedSurfaceObject->serverDecorated())
        {
            if(mouseedge != 0x0)
            {
                m_grabState = ResizeGrab;
                m_initialMousePos =adjustedPoint;
                m_mouseOffset = adjustedPoint - m_mouseSelectedSurfaceObject->surfacePosition();
                m_resizeEdge = mouseedge;
                m_initialSize = m_mouseSelectedSurfaceObject->surfaceSize();
                return;
            }
            if(m_mouseSelectedSurfaceObject->surfaceType() == Surface::TopLevel &&
                !m_mouseSelectedSurfaceObject->isShellDesktop())
                hwComp->raise(m_mouseSelectedSurfaceObject);
        }
        else
        {
            if(m_mouseSelectedSurfaceObject->surfaceType() == Surface::TopLevel &&
                !m_mouseSelectedSurfaceObject->isShellDesktop())
                hwComp->raise(m_mouseSelectedSurfaceObject);
            if(m_mouseSelectedSurfaceObject->surfaceType() == Surface::Desktop ||
                m_mouseSelectedSurfaceObject->isShellDesktop())
                hwComp->activate(m_mouseSelectedSurfaceObject);
        }

        m_initialMousePos = adjustedPoint;
        m_mouseOffset = adjustedPoint - m_mouseSelectedSurfaceObject->surfacePosition();

        QMouseEvent moveEvent(QEvent::MouseMove, adjustedPoint, e->globalPosition(),
                              Qt::NoButton, Qt::NoButton, e->modifiers());
        sendMouseEvent(&moveEvent, m_mouseSelectedSurfaceObject->viewForOutput(m_output));
    }
    sendMouseEvent(&e2, m_mouseSelectedSurfaceObject->viewForOutput(m_output));
    hwComp->resetIdle();
}

void OutputWindow::mouseReleaseEvent(QMouseEvent *e)
{
    QPoint adjustedPoint(m_output->wlOutput()->position().x()+e->position().x(),
                 m_output->wlOutput()->position().y()+e->position().y());

    QMouseEvent *e2 = new QMouseEvent(e->type(), adjustedPoint, adjustedPoint, e->button(), e->buttons(), e->modifiers(), e->pointingDevice());
    if (!mouseGrab())
    {
        if(m_mouseSelectedSurfaceObject)
        {
            SurfaceView *view = m_mouseSelectedSurfaceObject->primaryView();
            if(view)
                sendMouseEvent(e2, view);
        }
    }
    if (e->buttons() == Qt::NoButton) {
        if (m_grabState == DragGrab) {
            SurfaceView *view = viewAt(e->position());
            hwComp->handleDrag(view, e2);
        }
        if(m_grabState == MoveGrab)
            m_mouseSelectedSurfaceObject->endMove();
        m_mouseSelectedSurfaceObject = nullptr;
        m_grabState = NoGrab;

        setCursor(Qt::ArrowCursor);
    }
    hwComp->resetIdle();
}

void OutputWindow::mouseMoveEvent(QMouseEvent *e)
{
    if(hwComp->sleeping())
        hwComp->wake();

    QPointF adjustedPoint(m_output->wlOutput()->position().x()+e->position().x(),
                          m_output->wlOutput()->position().y()+e->position().y());

    // handle relative pointers (if bound)
    auto rp = hwComp->relativePointerManager()->relativePointerForWaylandPointer(hwComp->defaultSeat()->pointer());
    if(rp.count() > 1)
    {
        auto ts = e->timestamp();
        auto oldpoint = hwComp->globalCursorPosition();
        auto relx = adjustedPoint.x() - oldpoint.x();
        auto rely = adjustedPoint.y() - oldpoint.y();
        for(auto p : rp)
        {
            p->sendRelativeMotion(relx, rely, relx, rely, ts);
        }
    }

    // handle the actual default seat pointer
    QMouseEvent e2(e->type(), adjustedPoint, adjustedPoint, e->button(), e->buttons(), e->modifiers(), e->pointingDevice());
    hwComp->m_globalCursorPos = adjustedPoint;
    switch (m_grabState) {
    case NoGrab: {
        hwComp->m_resizeOldVal = 0;
        SurfaceView *view = m_mouseSelectedSurfaceObject ? m_mouseSelectedSurfaceObject->viewForOutput(m_output) : viewAt(adjustedPoint);

        sendMouseEvent(&e2, view);

        if (!view)
            setCursor(Qt::ArrowCursor);
        else
        {
            auto mouseedge = view->nearEdge(adjustedPoint);
            if(mouseedge == 0x0)
            {
                if(m_resizeCursor)
                {
                    m_resizeCursor = false;
                    setCursor(Qt::ArrowCursor);
                }
            }
            else
            {
                if((view->surfaceObject()->surfaceType() == Surface::TopLevel ||
                        view->surfaceObject()->surfaceType() == Surface::TopLevelTool) &&
                    !view->surfaceObject()->isMaximized())
                {
                    m_resizeCursor = true;
                    if(mouseedge == 0x01 || mouseedge == 0x08)
                        setCursor(Qt::SizeVerCursor);
                    else
                        setCursor(Qt::SizeHorCursor);
                }
            }
        }
    }
        break;
    case MoveGrab: {
        if(m_mouseSelectedSurfaceObject)
            m_mouseSelectedSurfaceObject->setPosition(adjustedPoint - m_mouseOffset);
        update();
    }
        break;
    case ResizeGrab: {
        QPoint delta = (adjustedPoint - m_initialMousePos).toPoint();
        if(m_mouseSelectedSurfaceObject)
            hwComp->handleResize(m_mouseSelectedSurfaceObject->primaryView(), m_initialSize, delta, m_resizeEdge);
    }
        break;
    case DragGrab: {
        SurfaceView *view = viewAt(e->position());
        hwComp->handleDrag(view, &e2);
        if (m_dragIconSurfaceObject) {
            // TODO: fix this offset
            //m_dragIconSurfaceObject->setPosition(e->position() + m_dragIconSurfaceObject->offset());
            m_dragIconSurfaceObject->setPosition(adjustedPoint);
            update();
        }
    }
        break;
    }
    hwComp->resetIdle();
}

void OutputWindow::sendMouseEvent(QMouseEvent *e, SurfaceView *target)
{
    QPoint adjustedPoint(m_output->wlOutput()->position().x()+e->position().x(),
                 m_output->wlOutput()->position().y()+e->position().y());

    QPointF mappedPos = adjustedPoint;
    if (target)
        mappedPos -= target->surfaceObject()->surfacePosition();

    QMouseEvent viewEvent(e->type(), mappedPos,
                          adjustedPoint, e->button(), e->buttons(), e->modifiers());
    hwComp->handleMouseEvent(target, &viewEvent);
}

void OutputWindow::keyPressEvent(QKeyEvent *e)
{
    if(hwComp->sleeping())
        hwComp->wake();

    hwComp->resetIdle();
    if(!hwComp->shortcuts()->checkAndHandleCombo(e->keyCombination()))
        hwComp->defaultSeat()->sendKeyPressEvent(e->nativeScanCode());
}

void OutputWindow::keyReleaseEvent(QKeyEvent *e)
{
    hwComp->defaultSeat()->sendKeyReleaseEvent(e->nativeScanCode());
    hwComp->resetIdle();
}

void OutputWindow::wheelEvent(QWheelEvent *e)
{
    if(hwComp->sleeping())
        hwComp->wake();

    // all wheels send an angle delta
    bool vert = false;
    int delta = e->angleDelta().x();
    if(e->angleDelta().y() != 0)
    {
        delta = e->angleDelta().y();
        vert = true;
    }

    // some high precision touchpads send a pixel delta
    if(e->hasPixelDelta())
    {
        if(vert)
            delta = e->pixelDelta().y();
        else
            delta = e->pixelDelta().x();
    }

    // TODO: flip-flop the values if we do not want 'natural' scrolling
    hwComp->defaultSeat()->sendMouseWheelEvent(vert ? Qt::Vertical : Qt::Horizontal,
                                               delta);
    hwComp->resetIdle();
}

void OutputWindow::touchEvent(QTouchEvent *e)
{
    if(hwComp->sleeping())
        hwComp->wake();

    QPoint adjustedPoint(m_output->wlOutput()->position().x()+e->points().first().position().x(),
                 m_output->wlOutput()->position().y()+e->points().first().position().y());
    SurfaceView *view = m_mouseSelectedSurfaceObject ?
                m_mouseSelectedSurfaceObject->viewForOutput(m_output) : viewAt(adjustedPoint);

    hwComp->defaultSeat()->sendFullTouchEvent(view->surface(), e);
    hwComp->resetIdle();
}
