// Hollywood Wayland Compositor
// SPDX-FileCopyrightText: 2024 Originull Software
// SPDX-License-Identifier: GPL-3.0-only

#include "decoration.h"
#include "outputwnd.h"
#include "surfaceobject.h"
#include <hollywood/hollywood.h>

#include <QObject>
#include <QVector>
#include <cmath>
#include <GLES3/gl3.h>
#include <QStaticText>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QPainter>

Q_LOGGING_CATEGORY(hwSSDRender, "compositor.ssd")

QOpenGLShaderProgram *g_shader = nullptr;

ServerSideDecoration::ServerSideDecoration(Surface *parent)
    : QObject(nullptr)
    , m_parent(parent)
    , m_dark_mode(false)
{
    if(g_shader == nullptr)
    {
        g_shader = new QOpenGLShaderProgram;
        g_shader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/surface.vsh");
        g_shader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/Shaders/surface.fsh");
        g_shader->link();
    }

    if(hwComp->viewMode() == 1)
        m_dark_mode = true;

    /*if(hwComp->viewMode() == 2 /*&& m_twilight*)
        m_dark_mode = true;*/

    m_fg_color = QColor(HOLLYWOOD_TEXTCLR_LIGHT);
    m_bg_color = QColor(HOLLYWOOD_WNDCLR_LIGHT);
    m_bg_color.setAlpha(255);
    m_stroke_color = QColor(HOLLYWOOD_WNDCLR_DARK);

    if(m_dark_mode)
    {
        m_fg_color = QColor(HOLLYWOOD_TEXTCLR_DARK);
        m_bg_color = QColor(HOLLYWOOD_WNDCLR_DARK);
        m_stroke_color = QColor(HOLLYWOOD_WNDCLR_LIGHT);
    }

    m_blitter.create();
    createWindowGemTextures();
    createWindowIconTexture();

    connect(m_parent, &Surface::iconChanged, this, &ServerSideDecoration::iconChanged);
    connect(m_parent, &Surface::titleChanged, this, &ServerSideDecoration::titleChanged);
}

ServerSideDecoration::~ServerSideDecoration()
{
    m_blitter.destroy();
    m_close_icon->destroy();
    m_minimize_icon->destroy();
    m_maximize_icon->destroy();
    m_restore_icon->destroy();
}

void ServerSideDecoration::paintGL(OutputWindow *window, QOpenGLFramebufferObject *fbo)
{
    renderDecoration(window);
    scissorContentArea(window);
    renderIcon(window);
    //renderTitle(window, fbo);
    renderGems(window);
}

bool ServerSideDecoration::hasWindowIcon() const
{
    return true;
}

bool ServerSideDecoration::hasMinimizeIcon() const
{
    return true;
}

bool ServerSideDecoration::hasMaximizeIcon() const
{
    return true;
}

bool ServerSideDecoration::hasCloseIcon() const
{
    return true;
}

void ServerSideDecoration::iconChanged()
{

}

void ServerSideDecoration::titleChanged()
{

}

QPoint ServerSideDecoration::decorationRenderStartPoint()
{
    return QPoint(m_parent->shadowSize()+hwComp->borderSize()
                                               *m_parent->surface()->bufferScale(),
                  m_parent->shadowSize()+hwComp->borderSize()
                                               *m_parent->surface()->bufferScale());
}

void ServerSideDecoration::renderDecoration(OutputWindow *window)
{
    auto functions = window->context()->functions();
    functions->glEnable(GL_BLEND);
    functions->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    functions->glUseProgram(g_shader->programId());

    GLuint vao, vbo, ebo;
    int originalWidth = m_parent->renderSize().width();
    int originalHeight = m_parent->renderSize().height();

    int leftReduction = m_parent->shadowSize();
    int rightReduction = m_parent->shadowSize();
    int bottomReduction = m_parent->shadowSize();
    int topReduction = m_parent->shadowSize();

    // NDC (Normalized Device Coordinates)
    float leftOffset = (leftReduction / (float)originalWidth) * 2.0f;   // For left
    float rightOffset = (rightReduction / (float)originalWidth) * 2.0f; // For right
    float bottomOffset = (bottomReduction / (float)originalHeight) * 2.0f; // For bottom
    float topOffset = (topReduction / (float)originalHeight) * 2.0f;    // For top

    float data[] = {
        // Positions          // Texture Coords
        -1.0f + leftOffset, -1.0f + bottomOffset, 0.0f, 0.0f, // Bottom left
        1.0f - rightOffset, -1.0f + bottomOffset, 1.0f, 0.0f, // Bottom right
        -1.0f + leftOffset,  1.0f - topOffset, 0.0f, 1.0f, // Top left
        1.0f - rightOffset,  1.0f - topOffset, 1.0f, 1.0f  // Top right
    };

    unsigned int quadIndices[] = { 0, 1, 2, 1, 3, 2 };
    g_shader->bind();
    g_shader->setUniformValue("radius", 0.6f);
    auto color = QColor(HOLLYWOOD_WNDCLR_LIGHT);
    if(m_dark_mode)
        color = QColor(HOLLYWOOD_WNDCLR_DARK);

    if(hwComp->activatedSurface() != m_parent)
        color = color.lighter(30);

    g_shader->setUniformValue("fragColor", color.blueF(), color.greenF(), color.redF(), color.alphaF());
    //g_shader->setUniformValue("fragColor", 0.9, 0.5, 0.1, 1.0f);

    //g_shader->setUniformValue("iResolution", m_parent->decoratedSize().width()/2,
    //                          m_parent->decoratedSize().height()/2, 0.0f);
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(data),
                 data, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,4*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    g_shader->release();
}

void ServerSideDecoration::renderGems(OutputWindow *window)
{
    auto functions = window->context()->functions();
    auto start = decorationRenderStartPoint();
    auto decoheight = hwComp->decorationSize();
    auto is = iconSize();
    auto start_y = (decoheight-is)/2;
    start.setY(start.y()+start_y);
    auto padding = paddingSize();
    start.setX(start.x()+m_parent->surfaceSize().width()-padding-is);
    if(hasCloseIcon())
    {
        renderGem(functions, start, QSize(is,is), m_close_icon);
        start.setX(start.x()-padding-is);
    }

    if(hasMaximizeIcon() && m_parent->canMaximize())
    {
        if(m_parent->isMaximized())
            renderGem(functions, start, QSize(is,is), m_restore_icon);
        else
            renderGem(functions, start, QSize(is,is), m_maximize_icon);
        start.setX(start.x()-padding-is);
    }

    if(hasMinimizeIcon() && m_parent->canMinimize())
        renderGem(functions, start, QSize(is,is), m_minimize_icon);
}

void ServerSideDecoration::renderIcon(OutputWindow *window)
{
    if(!m_window_icon)
        return;

    auto functions = window->context()->functions();
    auto start = decorationRenderStartPoint();
    auto decoheight = hwComp->decorationSize();
    auto is = iconSize();
    auto start_y = (decoheight-is)/2;
    start.setY(start.y()+start_y);
    auto padding = paddingSize();
    start.setX(start.x()+padding);
    renderGem(functions, start, QSize(is,is), m_window_icon);
}

void ServerSideDecoration::renderTitle(OutputWindow *window, QOpenGLFramebufferObject *fbo)
{
    QString windowTitleText = m_parent->windowTitle();
    if (!windowTitleText.isEmpty())
    {
        QStaticText m_windowTitle;
        if (m_windowTitle.text() != windowTitleText) {
            m_windowTitle.setText(windowTitleText);
            m_windowTitle.prepare();
        }

        QOpenGLPaintDevice pd;
        QPainter p(&pd);
        QFont font = p.font();
        auto psz = 12*m_parent->surface()->bufferScale();
        font.setPixelSize(psz);
        p.setFont(font);
        QFontMetrics m(p.font());
        QPointF start = QPoint(0,0);//decorationRenderStartPoint();
        /*start.setY(start.y()+m.height()+2);
        if(hasWindowIcon())
            start.setX(start.x()+32+5);
        else
            start.setX(start.x()+5);*/

        p.setPen(m_fg_color);
        p.drawText(start, windowTitleText);
    }
}

void ServerSideDecoration::scissorContentArea(OutputWindow *window)
{
    auto functions = window->context()->functions();
    auto bs = hwComp->borderSize();
    auto sm = m_parent->shadowSize();
    auto rect = QRect(sm+bs,sm+bs,m_parent->surfaceSize().width()
                      ,m_parent->surfaceSize().height());

    functions->glEnable(GL_SCISSOR_TEST);
    functions->glScissor(rect.x(), rect.y(), rect.width(), rect.height());
    functions->glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    functions->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    functions->glDisable(GL_SCISSOR_TEST);
    functions->glDisable(GL_BLEND);
}

void ServerSideDecoration::createWindowGemTextures()
{
    auto close = m_dark_mode ? ":/Icons/Dark/window-close"
                             : ":/Icons/Light/window-close";
    auto min   = m_dark_mode ? ":/Icons/Dark/window-minimize"
                             : ":/Icons/Light/window-minimize";
    auto max   = m_dark_mode ? ":/Icons/Dark/window-maximize"
                             : ":/Icons/Light/window-maximize";
    auto rest  = m_dark_mode ? ":/Icons/Dark/window-restore"
                             : ":/Icons/Light/window-restore";

    QImage c(close);
    m_close_icon = new QOpenGLTexture(QOpenGLTexture::Target2D);
    m_close_icon->create();
    m_close_icon->setSize(c.size().width(),
                     c.size().height());
    m_close_icon->setMagnificationFilter(QOpenGLTexture::Nearest);
    m_close_icon->setMinificationFilter(QOpenGLTexture::Linear);
    m_close_icon->setWrapMode(QOpenGLTexture::ClampToEdge);
    m_close_icon->setData(c);

    QImage m(min);
    m_minimize_icon = new QOpenGLTexture(QOpenGLTexture::Target2D);
    m_minimize_icon->create();
    m_minimize_icon->setSize(m.size().width(),
                          m.size().height());
    m_minimize_icon->setMagnificationFilter(QOpenGLTexture::Nearest);
    m_minimize_icon->setMinificationFilter(QOpenGLTexture::Linear);
    m_minimize_icon->setWrapMode(QOpenGLTexture::ClampToEdge);
    m_minimize_icon->setData(m);

    QImage x(max);
    m_maximize_icon = new QOpenGLTexture(QOpenGLTexture::Target2D);
    m_maximize_icon->create();
    m_maximize_icon->setSize(x.size().width(),
                          x.size().height());
    m_maximize_icon->setMagnificationFilter(QOpenGLTexture::Nearest);
    m_maximize_icon->setMinificationFilter(QOpenGLTexture::Linear);
    m_maximize_icon->setWrapMode(QOpenGLTexture::ClampToEdge);
    m_maximize_icon->setData(x);

    QImage r(rest);
    m_restore_icon = new QOpenGLTexture(QOpenGLTexture::Target2D);
    m_restore_icon->create();
    m_restore_icon->setSize(r.size().width(),
                          r.size().height());
    m_restore_icon->setMagnificationFilter(QOpenGLTexture::Nearest);
    m_restore_icon->setMinificationFilter(QOpenGLTexture::Linear);
    m_restore_icon->setWrapMode(QOpenGLTexture::ClampToEdge);
    m_restore_icon->setData(r);
}

void ServerSideDecoration::renderGem(QOpenGLFunctions *functions, const QPoint &startPoint, const QSize &size, QOpenGLTexture *texture)
{
    if(!texture)
        return;

    functions->glEnable(GL_BLEND);
    functions->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    texture->bind();
    m_blitter.bind();
    QMatrix4x4 tf = QOpenGLTextureBlitter::targetTransform(QRect(startPoint, size),
                                                           QRect(QPoint(0,0),
                                                           m_parent->renderSize()));

    m_blitter.blit(texture->textureId(), tf, QOpenGLTextureBlitter::OriginTopLeft);
    m_blitter.release();
    texture->release();
}

void ServerSideDecoration::createWindowIconTexture()
{
    auto icon = m_parent->icon();
    if(icon.isNull())
    {
        if(m_parent->getXWaylandShellSurface() != nullptr)
            icon = QIcon::fromTheme("xorg");
        else
            icon = QIcon::fromTheme("wayland");
    }

    if(m_window_icon)
        delete m_window_icon;

    QImage r = icon.pixmap(QSize(iconSize(),iconSize()), 1.0).toImage();
    m_window_icon = new QOpenGLTexture(QOpenGLTexture::Target2D);
    m_window_icon->create();
    m_window_icon->setSize(r.size().width(),
                            r.size().height());
    m_window_icon->setMagnificationFilter(QOpenGLTexture::Nearest);
    m_window_icon->setMinificationFilter(QOpenGLTexture::Linear);
    m_window_icon->setWrapMode(QOpenGLTexture::ClampToEdge);
    m_window_icon->setData(r);
}

uint ServerSideDecoration::paddingSize() const
{
    return 3*m_parent->surface()->bufferScale();
}

uint ServerSideDecoration::iconSize() const
{
    return 22;
}
