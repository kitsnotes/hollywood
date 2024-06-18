// Hollywood Wayland Compositor
// (C) 2022 Cat Stevenson <cat@originull.org>
// SPDX-License-Identifier: GPL-3.0-only

#include "wallpaper.h"
#include "outputwnd.h"
#include "hwc.h"

#include <hollywood/hollywood.h>
#include <QSettings>
#include <QOpenGLFunctions>
#include <QFile>
#include <QDir>

unsigned int VBOWP;

static const float pos[12] = { -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f};

WallpaperManager::WallpaperManager(OutputWindow *parent)
    : QObject{0}
    , m_parent(parent) {}

void WallpaperManager::setup()
{
    querySettings();
    connect(hwComp, &Compositor::settingsChanged, this, &WallpaperManager::querySettings);

    m_shader = new QOpenGLShaderProgram(this);
    m_shader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/transition.vsh");
    m_shader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/Shaders/transition.fsh");
    m_shader->link();
}

void WallpaperManager::wallpaperChanged()
{
    // setup our wallpaper gl texture
    if(m_texture != nullptr)
    {
        delete m_texture;
        m_texture = nullptr;
    }

    if(m_wallpaper.isNull())
        return;

    QImage bg(m_wallpaper);
    if(bg.isNull())
    {
        m_wallpaper = QString("");
        return;
    }

    bg.mirror(false, true);
    m_wpStartPoint = QPoint(0,0);
    qDebug() << "wallpaper: m_parent size" << m_parent->size();
    if(bg.size() != m_parent->size())
    {
        // only if we are equal in size do we not need
        // to perform any scaling.  but we're not
        bool lgw = bg.size().width() > m_parent->size().width();
        bool lgh = bg.size().height() > m_parent->size().height();
        switch(m_displayMode)
        {
        case FillScreen:
        default:
            // preserve aspect ratio
            // preserve original size if larger
            // enlarge if smaller
            // center in screen
            if(lgw || lgh)
            {
                // our background is larger than our
                // screen surface so let's
            }
            break;
        case Center:
            // center in the screen
            // shrink if needed, do not enlarge
            if(!lgw && !lgh)
            {
                // both sides are smaller - set a center point
                auto wd = m_parent->size().width() - bg.size().width();
                auto hd = m_parent->size().height() - bg.size().height();
                m_wpStartPoint = QPoint(wd/2,hd/2);
                break;
            }
            else
            {
                // we have a side larger than our screen so shrink
                if(!lgw && lgh)
                {
                    // height higher
                    // crop out our height from the middle
                    auto sh = bg.size().height() - m_parent->size().height();
                    sh = sh / 2;
                    bg = bg.copy(sh, 0, bg.size().width(), m_parent->size().height());
                    m_wpStartPoint = QPoint(sh,0);
                }
                else if(lgw && !lgh)
                {
                    // width heigher
                    auto sw = bg.size().width() - m_parent->size().width();
                    sw = sw / 2;
                    bg = bg.copy(0, sw, m_parent->size().width(), bg.size().height());
                    m_wpStartPoint = QPoint(0, sw);
                }
                else
                {
                    // both are bigger!
                    if(bg.width() > bg.height())
                    {
                        // wider image, scale the height
                        bg = bg.scaledToHeight(m_parent->height(), Qt::SmoothTransformation);
                        if(bg.width() > m_parent->width())
                        {
                            // width is still bigger so crop
                            auto midpoint = (bg.width() - m_parent->width())/2;
                            bg = bg.copy(midpoint, 0, m_parent->width(), bg.height());
                        }
                    }
                    else
                    {
                        // tall - scale to width
                        bg = bg.scaledToWidth(m_parent->width(), Qt::SmoothTransformation);
                        auto midpoint = (bg.height() - m_parent->height())/2;
                        bg = bg.copy(0,midpoint, bg.width(), m_parent->height());
                    }
                }
            }
            break;
        case FitToScreen:
            // preserve aspect ratio
            // fit in the screen
            // resize up/down if needed


        case Stretch:
            // fit to screen
            // then strech out in the
            // other direction
            break;
        }
    }
    m_texture = new QOpenGLTexture(QOpenGLTexture::Target2D);
    m_texture->create();
    m_texture->setSize(bg.size().width(), bg.size().height());
    m_texture->setMagnificationFilter(QOpenGLTexture::Nearest);
    m_texture->setMinificationFilter(QOpenGLTexture::Linear);
    m_texture->setWrapMode(QOpenGLTexture::ClampToEdge);
    m_texture->setData(bg);

    m_bgSize = bg.size();
}

void WallpaperManager::clearBackgroundColor()
{
    auto ac = QColor(m_color);
    if(!ac.isValid())
        ac = QColor(HOLLYWOOD_DEF_DESKTOP_BG);

    QOpenGLFunctions *functions = m_parent->context()->functions();
    if(!hwComp->isRunningLoginManager())
        functions->glClearColor(ac.redF(), ac.greenF(), ac.blueF(), ac.alphaF());
    else
        functions->glClearColor(0.0f,0.0f,0.0f,1.0f);

    functions->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void WallpaperManager::renderWallpaper()
{
    if(m_texture == nullptr)
        return;

    if(m_intrans)
    {
        renderTransition();
        return;
    }

    QOpenGLFunctions *functions = m_parent->context()->functions();
    m_parent->m_textureBlitter.bind();
    functions->glEnable(GL_BLEND);
    functions->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_texture->bind();
    QMatrix4x4 tf = QOpenGLTextureBlitter::targetTransform(QRect(m_wpStartPoint, m_bgSize),
                                                QRect(QPoint(0,0), m_parent->size()));

    m_parent->m_textureBlitter.blit(m_texture->textureId(), tf, QOpenGLTextureBlitter::OriginBottomLeft, m_texture->format());
    m_texture->release();
    functions->glDisable(GL_BLEND);
    m_parent->m_textureBlitter.release();
}

void WallpaperManager::setupRotationTimer()
{
    if(m_rotationTimer != nullptr)
    {
        m_rotationTimer->stop();
        m_rotationTimer->deleteLater();
        m_rotationTimer = nullptr;
    }
    m_rotationTimer = new QTimer(this);
    connect(m_rotationTimer, &QTimer::timeout, this, &WallpaperManager::transitionWallpaper);
    m_rotationTimer->setSingleShot(true);
    uint time = (60*1000);
    switch(m_rotateTime)
    {
    case 3:
        time = ((60*5)*1000);
        break;
    case 4:
        time = ((60*10)*1000);
        break;
    case 5:
        time = ((60*30)*1000);
        break;
    case 6:
        time = ((60*60)*1000);
        break;
    case 7:
        time = ((120*60)*1000);
        break;
    case 8:
        time = ((240*60)*1000);
        break;
    case 2:
    default:
        time = (60*1000);
        break;
    }

    m_rotationTimer->start(time);
}

void WallpaperManager::rotateNow()
{
    if(m_rotate == false)
        return;

    if(m_rotationTimer != nullptr)
    {
        auto interval = m_rotationTimer->interval();
        m_rotationTimer->stop();
        m_rotationTimer->start(interval);
    }
    transitionWallpaper();
}

void WallpaperManager::transitionWallpaper()
{
    m_shader->bind();

    m_intrans = true;
    m_oldtexture = m_texture;
    m_texture = nullptr;
    m_transprogress = 0.0f;
    m_wallpaper = findNextWallpaperInOrder();
    wallpaperChanged();
    setNewWallpaperPath(m_wallpaper);

    // clang analyzer complains about this
    // we should make sure we are bound and
    // TODO: write a fall back measure

    float ratio = m_parent->size().width() / m_parent->size().height();
    m_shader->setUniformValue("from", 0);
    m_shader->setUniformValue("to",   1);
    m_shader->setUniformValue("ratio",   ratio);
    m_shader->setUniformValue("_fromR",  ratio);
    m_shader->setUniformValue("_toR",   ratio);
    m_shader->setUniformValue("progress", m_transprogress);

    // TODO: disable transitions for legacy mode?
    hwComp->triggerRender();
}

void WallpaperManager::querySettings()
{
    auto oldbg = m_wallpaper;
    auto oldrotate = m_rotate;
    auto oldfolder = m_rotateFolder;
    auto olddisp = m_displayMode;
    QSettings settings("originull", "hollywood");
    settings.beginGroup("PrimaryWallpaper");
    m_rotate = settings.value("Rotate", false).toBool();
    if(!m_rotate && m_rotationTimer != nullptr && m_rotationTimer->isActive())
        m_rotationTimer->stop();

    m_rotateFolder = settings.value("RotateFolder").toString();
    m_wallpaper = settings.value("Wallpaper").toString();
    m_color = settings.value("BackgroundColor", HOLLYWOOD_DEF_DESKTOP_BG).toString().toUpper();
    auto dm = settings.value("DisplayMode", 0).toUInt();
    if(dm > 3)
            dm = 0;
    m_displayMode = (Layout)dm;

    QDir dir(m_rotateFolder);
    if(!dir.exists())
    {
        settings.setValue("Rotate", false);
        m_rotate = false;
    }

    m_rotateTime = settings.value("RotateTime", 6).toUInt();
    if(m_rotateTime > 9)
    {
        m_rotateTime = 6;
        settings.setValue("RotateTime", 6);
    }
    m_rotateMode = settings.value("RotateMode", 0).toUInt();
    if(m_rotateMode > 1)
    {
        m_rotateMode = 0;
        settings.setValue("RotateMode", 0);
    }
    settings.endGroup();

    if(m_wallpaper != oldbg ||
       m_rotate != oldrotate ||
       m_displayMode != olddisp ||
       m_rotateFolder != oldfolder)
    {
        wallpaperChanged();
    }
    if(m_rotate && (m_rotateTime > 1 && m_rotateTime < 9))
        setupRotationTimer();
}

QString WallpaperManager::findNextWallpaperInOrder()
{
    QDir dir(m_rotateFolder);
    if(!dir.exists())
        return QString();

    QString firstValid;

    auto entries = dir.entryList(QDir::Files, QDir::Name);
    for(auto e : entries)
    {
        QImage i;
        if(i.load(QString("%1/%2").arg(m_rotateFolder, e)))
        {
            firstValid = QString("%1/%2").arg(m_rotateFolder, e);
            break;
        }
    }

    if(firstValid.isEmpty())
        return QString();


    QString nextWallpaper;
    auto currWallpaper = m_wallpaper.split("/").last();
    auto idx = entries.indexOf(currWallpaper);
    if(idx == -1)
        return QString();

    for(int i = idx+1; i < entries.count(); ++i)
    {
        QImage img;
        auto imgpath = QString("%1/%2").arg(m_rotateFolder, entries[i]);
        if(img.load(imgpath))
        {
            nextWallpaper = QString("%1/%2").arg(m_rotateFolder, entries[i]);
            break;
        }
    }

    if(nextWallpaper.isEmpty())
        return firstValid;

    return nextWallpaper;
}

void WallpaperManager::renderTransition()
{
    QOpenGLFunctions *functions = m_parent->context()->functions();
    functions->glUseProgram(m_shader->programId());

    functions->glGenBuffers(1,&VBOWP);
    functions->glBindBuffer(GL_ARRAY_BUFFER, VBOWP);
    functions->glBufferData(GL_ARRAY_BUFFER, sizeof(pos), pos, GL_STATIC_DRAW);
    functions->glEnableVertexAttribArray(m_shader->attributeLocation("position"));
    functions->glVertexAttribPointer(m_shader->attributeLocation("position"), 2, GL_FLOAT, GL_FALSE, 0, 0);

    m_shader->bind();
    m_shader->setUniformValue("progress", m_transprogress);

    functions->glActiveTexture(GL_TEXTURE0);
    m_oldtexture->bind(GL_TEXTURE_2D);
    functions->glActiveTexture(GL_TEXTURE1);
    m_texture->bind(GL_TEXTURE_2D);
    functions->glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);
    m_transprogress += 0.02f;

    m_oldtexture->release();
    functions->glActiveTexture(GL_TEXTURE0);
    m_texture->release();
    m_shader->release();

    if(m_transprogress >=  1.0f)
    {
        completeTransition();
        return;
    }

    QTimer *t = new QTimer(this);
    t->setSingleShot(true);

    connect(t, &QTimer::timeout, [=]() {
      hwComp->triggerRender();
      t->deleteLater();
    } );
    t->start(2);
}

void WallpaperManager::completeTransition()
{
    QOpenGLFunctions *functions = m_parent->context()->functions();
    functions->glActiveTexture(GL_TEXTURE0);

    m_shader->release();
    delete m_oldtexture;
    m_transprogress = 0.0f;
    m_intrans = false;
    hwComp->triggerRender();
    setupRotationTimer();
}

void WallpaperManager::setNewWallpaperPath(const QString &wallpaper)
{
    QSettings settings("originull", "hollywood");
    settings.beginGroup("PrimaryWallpaper");
    settings.setValue("Wallpaper", wallpaper);
}
