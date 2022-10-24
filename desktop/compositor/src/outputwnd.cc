// Hollywood Wayland Compositor
// (C) 2021, 2022 Cat Stevenson <cat@originull.org>
// SPDX-License-Identifier: GPL-3.0-only

#include "outputwnd.h"

// include gles for x64
#include <GLES3/gl3.h>

#include <QMouseEvent>
#include <QOpenGLWindow>
#include <QOpenGLTexture>
#include <QOpenGLFunctions>
#include <QOpenGLFramebufferObject>
#include <QMatrix4x4>
#include <QPainter>
#include <QtWaylandCompositor/qwaylandseat.h>
#include <QDebug>
#include <QOpenGLPaintDevice>
#include <QGuiApplication>
#include <QPalette>
#include <QPainterPath>
#include <QStaticText>
#include <QOpenGLFramebufferObject>
#include <QOpenGLFramebufferObjectFormat>
#include <QRectF>
#include <sys/utsname.h>
#include <hollywood/hollywood.h>


#include "hwc.h"

#include "view.h"
#include "wallpaper.h"
#include "output.h"
#include "surfaceobject.h"

unsigned int VBO;

OutputWindow::OutputWindow(Output* parent)
    :m_output(parent)
    ,m_shadowShader(new QOpenGLShaderProgram(this))
    ,m_wpm(new WallpaperManager(this))
{
    connect(hwComp, &Compositor::startMove, this, &OutputWindow::startMove);
    connect(hwComp, &Compositor::startResize, this, &OutputWindow::startResize);
    connect(hwComp, &Compositor::dragStarted, this, &OutputWindow::startDrag);
}

int OutputWindow::width()
{
    return m_output->size().width();
}

int OutputWindow::height()
{
    return m_output->size().height();
}

void OutputWindow::initializeGL()
{
    m_textureBlitter.create();
    m_wpm->setup();
    m_shadowShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/shadow.vsh");
    m_shadowShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/Shaders/shadow.fsh");
    m_shadowShader->link();
    glGenBuffers(1,&VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    float data[] = {0,0,1,0,0,1,1,1};
    glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, false, 0, 0);
}

void OutputWindow::paintGL()
{
    hwComp->startRender();
    // render our background & wallpaper
    m_wpm->clearBackgroundColor();
    m_wpm->renderWallpaper();

    // draw our desktops (bottom most) - we can't do this recursively
    // because the pop-ups will be under z-order things
    for(Surface *obj : hwComp->backgroundLayerSurfaces())
        drawTextureForObject(obj, false);

    for(Surface *obj : hwComp->desktopSurfaces())
        drawTextureForObject(obj, false);

    for(Surface *obj : hwComp->bottomLayerSurfaces())
        drawTextureForObject(obj, false);

    //drawDesktopInfoString();

    bool useShadow = !hwComp->legacyRender();

    // draw standard surfaces
    for(Surface *obj : hwComp->surfaceByZOrder())
        recursiveDrawTextureForObject(obj, useShadow);

    // now let's draw our popups for the backgroundLayerSurfaces
    for(Surface *obj : hwComp->backgroundLayerSurfaces())
        drawPopupsForObject(obj);

    // now let's draw our popups for the desktops
    for(Surface *obj : hwComp->desktopSurfaces())
        drawPopupsForObject(obj);
    // now let's draw our popups for the bottomLayerSurfaces
    for(Surface *obj : hwComp->bottomLayerSurfaces())
        drawPopupsForObject(obj);

    for(Surface *obj : hwComp->topLayerSurfaces())
        recursiveDrawTextureForObject(obj, false);

    for(Surface *obj : hwComp->overlayLayerSurfaces())
        recursiveDrawTextureForObject(obj, false);

    // draw menu server (top most)
    if(hwComp->menuServerSurfaceObject())
        recursiveDrawTextureForObject(hwComp->menuServerSurfaceObject());

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

void OutputWindow::recursiveDrawTextureForObject(Surface *obj, bool useShadow)
{
    if(obj->isCursor())
        return;   /* we're not drawing a cursor here */

    if(obj->isMinimized())
        return;

    if(obj->isGtkSurface())
        useShadow = false;

    drawTextureForObject(obj, useShadow);
    for(Surface *child : obj->childSurfaceObjects())
        recursiveDrawTextureForObject(child, useShadow);
}

void OutputWindow::drawTextureForObject(Surface *obj, bool useShadow)
{
    glActiveTexture(GL_TEXTURE0);

    QRect dispRect(m_output->wlOutput()->position(), m_output->size());
    QRect objRect(obj->decoratedRect().toRect());
    if(!objRect.intersects(dispRect))
        return;

    GLenum currentTarget = GL_TEXTURE_2D;
    auto texture = obj->viewForOutput(m_output)->getTexture();
    if (!texture)
        return;

    if (texture->target() != currentTarget)
    {
        currentTarget = texture->target();
        m_textureBlitter.bind(currentTarget);
    }

    if ((obj->surface() && obj->surface()->hasContent()) || obj->viewForOutput(m_output)->isBufferLocked()) {
        QSize s = obj->size();
        if (!s.isEmpty())
        {
            QOpenGLFunctions *functions = context()->functions();

            if (m_mouseSelectedSurfaceObject == obj && m_grabState == ResizeGrab && m_resizeAnchored)
                obj->setPosition(getAnchoredPosition(m_resizeAnchorPosition, m_resizeEdge, s));

            // we use FBO's to construct a final thing to ouptut
            // including shadow, server decorations and the window contents
            QOpenGLFramebufferObjectFormat fbofmt;
            fbofmt.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);

            QSize dsize = obj->decoratedRect().size().toSize();
            if(!obj->serverDecorated())
                dsize = obj->size();
            uint sm = 0;
            if(useShadow)
            {
                sm = 45;
                if(obj->isXdgPopup())
                    sm = 15;

                dsize = dsize.grownBy(QMargins(sm,sm,sm,sm));
            }

            if(m_fbo != nullptr)
                delete m_fbo;

            m_fbo = new QOpenGLFramebufferObject(dsize, fbofmt);
            m_fbo->bind();
            functions->glViewport(0,0,dsize.width(),dsize.height());
            functions->glClearColor(0.0f,0.0f,0.0f,0.0f);
            functions->glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

            if(obj->layerSurface() != nullptr)
                useShadow = false;

            if(useShadow)
                drawShadowForObject(sm, obj);

            QPointF pos = QPoint(0,0);
            QRectF surfaceGeometry(pos, s);
            if(obj->serverDecorated())
                drawServerSideDecoration(dsize, sm, obj);

            auto surfaceOrigin = obj->primaryView()->textureOrigin();
            QRectF targetRect(QPoint(hwComp->borderSize(),
                       obj->serverDecorated() ? hwComp->decorationSize() : 0), surfaceGeometry.size());

            m_textureBlitter.bind();
            functions->glEnable(GL_BLEND);
            functions->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            if(useShadow)
            {
                targetRect.adjust(sm,sm,sm,sm);
            }
            QMatrix4x4 targetTransform = QOpenGLTextureBlitter::targetTransform(targetRect,
                                      QRect(QPoint(0,0), QSize(dsize.width(), dsize.height())));
            m_textureBlitter.blit(texture->textureId(), targetTransform, surfaceOrigin);
            m_textureBlitter.release();

            functions->glDisable(GL_BLEND);
            m_fbo->release();
            // we are done rendering our window into fbo

            // switch back to our primary scene and blit fbo contents onto it
            functions->glViewport(0,0,size().width(),size().height());
            m_textureBlitter.bind();
            functions->glEnable(GL_BLEND);
            functions->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            QRect target(QPointF(obj->position()+ obj->parentPosition()).toPoint(), dsize);

            // if we are rendering with a shadow we should offset our starting position
            if(useShadow)
            {
                auto point = target.topLeft();
                point.setX(point.x()-sm);
                point.setY(point.y()-sm);
                target.moveTopLeft(point);
            }

            QMatrix4x4 targetTransform2 = QOpenGLTextureBlitter::targetTransform(target,
                                                    QRect(m_output->wlOutput()->position(), size()));
            m_textureBlitter.blit(m_fbo->texture(), targetTransform2, QOpenGLTextureBlitter::OriginBottomLeft);
            m_textureBlitter.release();
            functions->glDisable(GL_BLEND);
        }
    }
}

void OutputWindow::drawPopupsForObject(Surface *obj, bool useShadow)
{
    if(obj->isCursor())
        return;   /* we're not drawing a cursor here */

    if(obj->isMinimized())
        return;

    for(Surface *child : obj->childSurfaceObjects())
        recursiveDrawTextureForObject(child, useShadow);
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

    m_shadowShader->bind();

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

    auto rect = QRect(sm+bs,sm+bs,obj->size().width(),obj->size().height());
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

void OutputWindow::drawDesktopInfoString(QPainter *p)
{
    Q_UNUSED(p);
    QPainter px(this);
    px.endNativePainting();
    QStringList lines;
    auto kernel = QString();
    struct utsname unb;
    if(uname(&unb) != 0)
        kernel = QLatin1String("Unknown");
    else
        kernel = QLatin1String(unb.release);

    lines << QString("Hollywood %1 (API %2, Kernel %3)")
        .arg(QLatin1String(HOLLYWOOD_OS_VERSION))
        .arg(QLatin1String(QT_VERSION_STR))
        .arg(kernel);

    auto env = qgetenv("HOLLYWOOD_GENUINE");
    if(!env.isEmpty())
        lines << QLatin1String("This copy of Linux is not genuine.");
    QTextOption to;
    to.setAlignment(Qt::AlignRight);
    px.setRenderHint(QPainter::TextAntialiasing);
    px.setPen(Qt::white);
    QFont font(HOLLYWOOD_DEF_STDFONT, 9);
    px.setFont(font);
    QFontMetrics m = QFontMetrics(font);
    int textHeight = m.height();
    for(int i = 0; i < lines.count(); i++)
    {
        auto text = lines.at(i);
        auto rect = m.boundingRect(text, to);
        int y = size().height() - (textHeight * lines.count()) +
                (textHeight * (i+1)) - 5;
        int x = size().width() - rect.width() - 5;
        px.drawText(QPoint(x,y), text);
    }
    px.end();
    px.beginNativePainting();
}

void OutputWindow::drawServerSideDecoration(const QSize buf, uint shadowOffset, Surface *obj)
{
    if(obj->isSpecialShellObject())
        return;

    if(obj->xdgTopLevel() == nullptr && obj->qtSurface() == nullptr)
        return;

    if(obj->decorationImage() == nullptr)
        return;

    QImage* img = obj->decorationImage();

    QOpenGLFunctions *functions = context()->functions();
    m_textureBlitter.bind();
    functions->glEnable(GL_BLEND);
    functions->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    QOpenGLTexture texture(*img);
    texture.bind();
    QMatrix4x4 targetTransform = QOpenGLTextureBlitter::targetTransform(QRect(shadowOffset,shadowOffset,obj->decoratedRect().width(), obj->decoratedRect().height()),
                              QRect(0,0, buf.width(), buf.height()));
    m_textureBlitter.blit(texture.textureId(), targetTransform, QOpenGLTextureBlitter::OriginTopLeft);
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
        SurfaceView* view = surface->primaryView();
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
    // see if we're hitting menuserver
    auto ms = hwComp->menuServerSurfaceObject();
    if(ms != nullptr)
    {
        QRectF geom(ms->position(), ms->size());
        if (geom.contains(point))
            ret = ms;

        // check children of the menuserver
        for(auto *s : ms->childSurfaceObjects())
        {
            QRectF geom(s->position(), s->size());
            if (geom.contains(adjustedPoint))
                ret = s;
        }
    }

    for(auto surf : hwComp->overlayLayerSurfaces())
    {
        QRectF geom(surf->position(), surf->size());
        if (geom.contains(point))
            ret = surf;

        // check children of the menuserver
        for(auto *s : surf->childSurfaceObjects())
        {
            QRectF geom(s->position(), s->size());
            if (geom.contains(adjustedPoint))
                ret = s;
        }
    }

    for(auto surf : hwComp->topLayerSurfaces())
    {
        QRectF geom(surf->position(), surf->size());
        if (geom.contains(point))
            ret = surf;

        // check children of the menuserver
        for(auto *s : surf->childSurfaceObjects())
        {
            QRectF geom(s->position(), s->size());
            if (geom.contains(adjustedPoint))
                ret = s;
        }
    }

    // we should check for children of the desktop now
    if(ret == nullptr)
    {
        const auto views = hwComp->desktopSurfaces();
        for (auto *s : views) {
            for(auto *surface : s->childSurfaceObjects())
            {
                if (surface == m_dragIconSurfaceObject)
                    continue;
                QRectF geom(surface->position(), surface->size());
                if (geom.contains(adjustedPoint))
                    ret = surface;
            }
        }
    }

    // we should check for children of the background layer surfaces
    if(ret == nullptr)
    {
        const auto views = hwComp->backgroundLayerSurfaces();
        for (auto *s : views) {
            for(auto *surface : s->childSurfaceObjects())
            {
                if (surface == m_dragIconSurfaceObject)
                    continue;
                QRectF geom(surface->position(), surface->size());
                if (geom.contains(adjustedPoint))
                    ret = surface;
            }
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
                QRectF geom(surface->position(), surface->size());
                if (geom.contains(adjustedPoint))
                    ret = surface;
            }
        }
    }

    // finally go through our standard surfaces by zorder
    if(ret == nullptr)
    {
        const auto views = hwComp->surfaceByZOrder();
        for (auto *surface : views) {
            if (surface == m_dragIconSurfaceObject)
                continue;
            if(surface->isMinimized())
                continue;

            QSizeF sz;
            if(surface->serverDecorated())
                sz = surface->decoratedRect().size();
            else
                sz = surface->size();

            QRectF geom(surface->position(), sz);
            if (geom.contains(point))
                ret = surface;
            for(auto s : surface->childSurfaceObjects())
            {
                QRectF geom(s->position(), s->size());
                if (geom.contains(adjustedPoint))
                    ret = s;
            }
        }
    }

    // if we're still null see if we're hitting desktop/layer shells
    if(ret == nullptr)
    {
        const auto views = hwComp->desktopSurfaces();
        for (auto *surface : views) {
            if (surface == m_dragIconSurfaceObject)
                continue;
            QRectF geom(surface->position(), surface->size());
            if (geom.contains(adjustedPoint))
                ret = surface;
        }
    }

    if(ret == nullptr)
    {
        const auto views = hwComp->bottomLayerSurfaces();
        for (auto *surface : views) {
            if (surface == m_dragIconSurfaceObject)
                continue;
            QRectF geom(surface->position(), surface->size());
            if (geom.contains(adjustedPoint))
                ret = surface;
        }
    }

    if(ret == nullptr)
    {
        const auto views = hwComp->backgroundLayerSurfaces();
        for (auto *surface : views) {
            if (surface == m_dragIconSurfaceObject)
                continue;
            QRectF geom(surface->position(), surface->size());
            if (geom.contains(adjustedPoint))
                ret = surface;
        }
    }
    return ret;
}

void OutputWindow::startMove()
{
    m_grabState = MoveGrab;
    m_mouseSelectedSurfaceObject->startMove();
}

void OutputWindow::startResize(int edge, bool anchored)
{
    m_initialSize = m_mouseSelectedSurfaceObject->windowSize();
    m_grabState = ResizeGrab;
    m_resizeEdge = edge;
    m_resizeAnchored = anchored;
    m_resizeAnchorPosition = getAnchorPosition(m_mouseSelectedSurfaceObject->position(), edge, m_mouseSelectedSurfaceObject->surface()->destinationSize());
}

void OutputWindow::startDrag(Surface *dragIcon)
{
    m_grabState = DragGrab;
    m_dragIconSurfaceObject = dragIcon;
    hwComp->raise(dragIcon);
}

void OutputWindow::mousePressEvent(QMouseEvent *e)
{
    if (mouseGrab())
        return;

    QPoint adjustedPoint(m_output->wlOutput()->position().x()+e->position().x(),
                 m_output->wlOutput()->position().y()+e->position().y());

    QMouseEvent e2(e->type(), adjustedPoint, e->button(), e->buttons(), e->modifiers(), e->pointingDevice());

    if (m_mouseSelectedSurfaceObject.isNull())
    {
        m_mouseSelectedSurfaceObject = surfaceAt(e->position());
        if (!m_mouseSelectedSurfaceObject) {
            hwComp->closePopups();
            return;
        }

        auto mouseedge = m_mouseSelectedSurfaceObject->primaryView()->nearEdge(adjustedPoint);
        if (e->modifiers() == Qt::AltModifier || e->modifiers() == Qt::MetaModifier)
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
                m_mouseOffset = adjustedPoint - m_mouseSelectedSurfaceObject->position();
                m_resizeEdge = mouseedge;
                m_initialSize = m_mouseSelectedSurfaceObject->size();
                return;
            }
        }
        else if(m_mouseSelectedSurfaceObject->serverDecorated())
        {
            if(mouseedge != 0x0)
            {
                m_grabState = ResizeGrab;
                m_initialMousePos =adjustedPoint;
                m_mouseOffset = adjustedPoint - m_mouseSelectedSurfaceObject->position();
                m_resizeEdge = mouseedge;
                m_initialSize = m_mouseSelectedSurfaceObject->size();
                return;
            }
            if(m_mouseSelectedSurfaceObject->surfaceType() == Surface::TopLevel)
                hwComp->raise(m_mouseSelectedSurfaceObject);
        }
        else
        {
            if(m_mouseSelectedSurfaceObject->surfaceType() == Surface::TopLevel)
                hwComp->raise(m_mouseSelectedSurfaceObject);
        }

        m_initialMousePos = adjustedPoint;
        m_mouseOffset = adjustedPoint - m_mouseSelectedSurfaceObject->position();

        qDebug() << "btn1" << e->position() << e->globalPosition();
        QMouseEvent moveEvent(QEvent::MouseMove, adjustedPoint, e->globalPosition(), Qt::NoButton, Qt::NoButton, e->modifiers());
        sendMouseEvent(&moveEvent, m_mouseSelectedSurfaceObject->primaryView());
    }
    qDebug() << "btn2" << e->position() << e->globalPosition();
    sendMouseEvent(&e2, m_mouseSelectedSurfaceObject->primaryView()); //(m_output));
}

void OutputWindow::mouseReleaseEvent(QMouseEvent *e)
{
    QPoint adjustedPoint(m_output->wlOutput()->position().x()+e->position().x(),
                 m_output->wlOutput()->position().y()+e->position().y());

    QMouseEvent *e2 = new QMouseEvent(e->type(), adjustedPoint, e->button(), e->buttons(), e->modifiers(), e->pointingDevice());
    if (!mouseGrab())
    {
        if(m_mouseSelectedSurfaceObject)
        {
            SurfaceView *view = m_mouseSelectedSurfaceObject->primaryView();
            if(view)
            {
                sendMouseEvent(e2, view);
            }
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
}

void OutputWindow::mouseMoveEvent(QMouseEvent *e)
{
    QPoint adjustedPoint(m_output->wlOutput()->position().x()+e->position().x(),
                 m_output->wlOutput()->position().y()+e->position().y());

    QMouseEvent e2(e->type(), adjustedPoint, e->button(), e->buttons(), e->modifiers(), e->pointingDevice());
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
                if(view->surfaceObject()->surfaceType() == Surface::TopLevel ||
                        view->surfaceObject()->surfaceType() == Surface::TopLevelTool )
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
        m_mouseSelectedSurfaceObject->setPosition(adjustedPoint - m_mouseOffset);
        update();
    }
        break;
    case ResizeGrab: {
        QPoint delta = (adjustedPoint - m_initialMousePos).toPoint();
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
}

void OutputWindow::sendMouseEvent(QMouseEvent *e, SurfaceView *target)
{
    QPoint adjustedPoint(m_output->wlOutput()->position().x()+e->position().x(),
                 m_output->wlOutput()->position().y()+e->position().y());

    QPointF mappedPos = adjustedPoint;
    if (target)
    {
        mappedPos -= target->surfaceObject()->position();
        if(target->surfaceObject()->serverDecorated())
        {
            // re-map our position to account for SSD
            mappedPos.setY(mappedPos.y() - hwComp->decorationSize());
            mappedPos.setX(mappedPos.x() - hwComp->borderSize());
        }
    }
    QMouseEvent viewEvent(e->type(), mappedPos, adjustedPoint, e->button(), e->buttons(), e->modifiers());
    hwComp->handleMouseEvent(target, &viewEvent);
}

void OutputWindow::keyPressEvent(QKeyEvent *e)
{
    hwComp->defaultSeat()->sendKeyPressEvent(e->nativeScanCode());
}

void OutputWindow::keyReleaseEvent(QKeyEvent *e)
{
    hwComp->defaultSeat()->sendKeyReleaseEvent(e->nativeScanCode());
}

void OutputWindow::wheelEvent(QWheelEvent *e)
{
    bool vert = false;
    int delta = e->pixelDelta().x();
    if(e->angleDelta().y() != 0)
    {
        delta = e->pixelDelta().y();
        vert = true;
    }

    hwComp->defaultSeat()->sendMouseWheelEvent(vert ? Qt::Vertical : Qt::Horizontal,
                                               delta);
}
