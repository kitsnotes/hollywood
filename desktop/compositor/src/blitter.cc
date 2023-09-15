// Copyright (C) 2023 Originull Software
// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "blitter.h"

#include <QtOpenGL/QOpenGLShaderProgram>
#include <QtOpenGL/QOpenGLVertexArrayObject>
#include <QtOpenGL/QOpenGLBuffer>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLExtraFunctions>

#ifndef GL_TEXTURE_EXTERNAL_OES
#define GL_TEXTURE_EXTERNAL_OES           0x8D65
#endif
#ifndef GL_TEXTURE_RECTANGLE
#define GL_TEXTURE_RECTANGLE              0x84F5
#endif
#ifndef GL_TEXTURE_WIDTH
#define GL_TEXTURE_WIDTH                  0x1000
#endif
#ifndef GL_TEXTURE_HEIGHT
#define GL_TEXTURE_HEIGHT                 0x1001
#endif

static const char vertex_shader150[] =
    "#version 150 core\n"
    "in vec3 vertexCoord;"
    "in vec2 textureCoord;"
    "out vec2 uv;"
    "uniform mat4 vertexTransform;"
    "uniform mat3 textureTransform;"
    "void main() {"
    "   uv = (textureTransform * vec3(textureCoord,1.0)).xy;"
    "   gl_Position = vertexTransform * vec4(vertexCoord,1.0);"
    "}";

static const char fragment_shader150[] =
    "#version 150 core\n"
    "in vec2 uv;"
    "out vec4 fragcolor;"
    "uniform sampler2D textureSampler;"
    "uniform bool swizzle;"
    "uniform float opacity;"
    "uniform bool isrgb;"
    "void main() {"
    "   if(isrgb) {"
    "     vec4 tmpFragColor; tmpFragColor.rgb = 1.0*texture(textureSampler,uv).rgb;"
    "     tmpFragColor.a = 1.0;"
    "     tmpFragColor.a *= opacity;"
    "     fragcolor = swizzle ? tmpFragColor.bgra : tmpFragColor;"
    "   } else {"
    "   vec4 tmpFragColor = texture(textureSampler, uv);"
    "   tmpFragColor.a *= opacity;"
    "   fragcolor = swizzle ? tmpFragColor.bgra : tmpFragColor;"
    "   } "
    "}";

static const char vertex_shader[] =
    "attribute highp vec3 vertexCoord;"
    "attribute highp vec2 textureCoord;"
    "varying highp vec2 uv;"
    "uniform highp mat4 vertexTransform;"
    "uniform highp mat3 textureTransform;"
    "void main() {"
    "   uv = (textureTransform * vec3(textureCoord,1.0)).xy;"
    "   gl_Position = vertexTransform * vec4(vertexCoord,1.0);"
    "}";

static const char fragment_shader[] =
    "varying highp vec2 uv;"
    "uniform sampler2D textureSampler;"
    "uniform bool swizzle;"
    "uniform bool isrgb;"
    "uniform highp float opacity;"
    "void main() {"
    "   if(isrgb) {"
    "     highp vec4 tmpFragColor; tmpFragColor.rgb = 1.0*texture2D(textureSampler,uv).rgb;"
    "     tmpFragColor.a *= opacity;"
    "     gl_FragColor = swizzle ? tmpFragColor.bgra : tmpFragColor;"
    "   } else {"
    "     highp vec4 tmpFragColor = texture2D(textureSampler,uv);"
    "     tmpFragColor.a *= opacity;"
    "     gl_FragColor = swizzle ? tmpFragColor.bgra : tmpFragColor;"
    "   }"
    "}";

static const char fragment_shader_external_oes[] =
    "#extension GL_OES_EGL_image_external : require\n"
    "varying highp vec2 uv;"
    "uniform samplerExternalOES textureSampler;\n"
    "uniform bool swizzle;"
    "uniform highp float opacity;"
    "void main() {"
    "   highp vec4 tmpFragColor = texture2D(textureSampler, uv);"
    "   tmpFragColor.a *= opacity;"
    "   gl_FragColor = swizzle ? tmpFragColor.bgra : tmpFragColor;"
    "}";

static const char fragment_shader_rectangle[] =
    "varying highp vec2 uv;"
    "uniform sampler2DRect textureSampler;"
    "uniform bool swizzle;"
    "uniform highp float opacity;"
    "void main() {"
    "   highp vec4 tmpFragColor = texture2DRect(textureSampler,uv);"
    "   tmpFragColor.a *= opacity;"
    "   gl_FragColor = swizzle ? tmpFragColor.bgra : tmpFragColor;"
    "}";

static const char fragment_shader150_rectangle[] =
    "#version 150 core\n"
    "in vec2 uv;"
    "out vec4 fragcolor;"
    "uniform sampler2DRect textureSampler;"
    "uniform bool swizzle;"
    "uniform float opacity;"
    "void main() {"
    "   vec4 tmpFragColor = texture(textureSampler, uv);"
    "   tmpFragColor.a *= opacity;"
    "   fragcolor = swizzle ? tmpFragColor.bgra : tmpFragColor;"
    "}";

static const GLfloat vertex_buffer_data2[] = {
    0, 0,
    0, 1,
    1, 0,
    0, 1
};

static const GLfloat vertex_buffer_data[] = {
        -1,-1, 0,
        -1, 1, 0,
         1,-1, 0,
        -1, 1, 0,
         1,-1, 0,
         1, 1, 0
};

static const GLfloat texture_buffer_data[] = {
        0, 0,
        0, 1,
        1, 0,
        0, 1,
        1, 0,
        1, 1
};

class QBlitterTextureBinder
{
public:
    explicit QBlitterTextureBinder(GLenum target, GLuint textureId) : m_target(target)
    {
        QOpenGLContext::currentContext()->functions()->glBindTexture(m_target, textureId);
    }
    ~QBlitterTextureBinder()
    {
        QOpenGLContext::currentContext()->functions()->glBindTexture(m_target, 0);
    }

private:
    GLenum m_target;
};

class BlitterPrivate
{
public:
    enum TextureMatrixUniform {
        User,
        Identity,
        IdentityFlipped
    };

    enum ProgramIndex {
        TEXTURE_2D,
        TEXTURE_EXTERNAL_OES,
        TEXTURE_RECTANGLE
    };

    BlitterPrivate(Blitter *q_ptr) :
        q(q_ptr),
        opacity(1.0f),
        vao(new QOpenGLVertexArrayObject),
        isrgb(false),
        currentTarget(TEXTURE_2D)
    { }

    bool buildProgram(ProgramIndex idx, const char *vs, const char *fs);
    bool ensureProgram(ProgramIndex idx);

    void blit(GLuint texture, const QMatrix4x4 &targetTransform, const QMatrix3x3 &sourceTransform, QOpenGLTexture::TextureFormat format);
    void blit(GLuint texture, const QMatrix4x4 &targetTransform, QOpenGLTextureBlitter::Origin origin, QOpenGLTexture::TextureFormat format);

    QMatrix3x3 toTextureCoordinates(const QMatrix3x3 &sourceTransform) const;

    bool prepareProgram(const QMatrix4x4 &vertexTransform);

    Blitter *q;
    QOpenGLBuffer vertexBuffer;
    QOpenGLBuffer textureBuffer;
    struct Program {
        Program() :
            vertexCoordAttribPos(0),
            vertexTransformUniformPos(0),
            textureCoordAttribPos(0),
            textureTransformUniformPos(0),
            opacityUniformPos(0),
            isrgb(false),
            opacity(0.0f),
            textureMatrixUniformState(User)
        { }
        QScopedPointer<QOpenGLShaderProgram> glProgram;
        GLuint vertexCoordAttribPos;
        GLuint vertexTransformUniformPos;
        GLuint textureCoordAttribPos;
        GLuint textureTransformUniformPos;
        GLuint opacityUniformPos;
        GLuint isrgbUniformPos;
        bool isrgb;
        float opacity;
        TextureMatrixUniform textureMatrixUniformState;
    } programs[3];
    float opacity;
    QScopedPointer<QOpenGLVertexArrayObject> vao;
    bool isrgb;
    GLenum currentTarget;
};

static inline BlitterPrivate::ProgramIndex targetToProgramIndex(GLenum target)
{
    switch (target) {
    case GL_TEXTURE_2D:
        return BlitterPrivate::TEXTURE_2D;
    case GL_TEXTURE_EXTERNAL_OES:
        return BlitterPrivate::TEXTURE_EXTERNAL_OES;
    case GL_TEXTURE_RECTANGLE:
        return BlitterPrivate::TEXTURE_RECTANGLE;
    default:
        qWarning("Unsupported texture target 0x%x", target);
        return BlitterPrivate::TEXTURE_2D;
    }
}

bool BlitterPrivate::prepareProgram(const QMatrix4x4 &vertexTransform)
{
    ProgramIndex programIndex = targetToProgramIndex(currentTarget);
    if (!ensureProgram(programIndex))
        return false;

    Program *program = &programs[programIndex];

    vertexBuffer.bind();
    program->glProgram->setAttributeBuffer(program->vertexCoordAttribPos, GL_FLOAT, 0, 3, 0);
    program->glProgram->enableAttributeArray(program->vertexCoordAttribPos);
    vertexBuffer.release();

    program->glProgram->setUniformValue(program->vertexTransformUniformPos, vertexTransform);

    textureBuffer.bind();
    program->glProgram->setAttributeBuffer(program->textureCoordAttribPos, GL_FLOAT, 0, 2, 0);
    program->glProgram->enableAttributeArray(program->textureCoordAttribPos);
    textureBuffer.release();

    if (opacity != program->opacity) {
        program->glProgram->setUniformValue(program->opacityUniformPos, opacity);
        program->opacity = opacity;
    }

    program->glProgram->setUniformValue(program->isrgbUniformPos, false);

    return true;
}

QMatrix3x3 BlitterPrivate::toTextureCoordinates(const QMatrix3x3 &sourceTransform) const
{
    if (currentTarget == GL_TEXTURE_RECTANGLE) {
        // Non-normalized coordinates
        QMatrix4x4 textureTransform(sourceTransform);
        if (auto *glFunctions = QOpenGLContext::currentContext()->extraFunctions()) {
            int width, height;
            glFunctions->glGetTexLevelParameteriv(currentTarget, 0, GL_TEXTURE_WIDTH, &width);
            glFunctions->glGetTexLevelParameteriv(currentTarget, 0, GL_TEXTURE_HEIGHT, &height);
            textureTransform.scale(width, height);
        }
        return textureTransform.toGenericMatrix<3, 3>();
   }

    return sourceTransform; // Normalized coordinates
}

void BlitterPrivate::blit(GLuint texture,
                                        const QMatrix4x4 &targetTransform,
                                        const QMatrix3x3 &sourceTransform,
                                        QOpenGLTexture::TextureFormat format)
{
    QBlitterTextureBinder binder(currentTarget, texture);
    if (!prepareProgram(targetTransform))
        return;

    Program *program = &programs[targetToProgramIndex(currentTarget)];
    program->glProgram->setUniformValue(program->isrgbUniformPos, false);

    const QMatrix3x3 textureTransform = toTextureCoordinates(sourceTransform);
    program->glProgram->setUniformValue(program->textureTransformUniformPos, textureTransform);
    program->textureMatrixUniformState = User;

    if(format == QOpenGLTexture::RGB)
        program->glProgram->setUniformValue(program->isrgbUniformPos, true);

    QOpenGLContext::currentContext()->functions()->glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);
}

void BlitterPrivate::blit(GLuint texture,
                          const QMatrix4x4 &targetTransform,
                          QOpenGLTextureBlitter::Origin origin,
                          QOpenGLTexture::TextureFormat format)
{

    QBlitterTextureBinder binder(currentTarget, texture);
    if (!prepareProgram(targetTransform))
        return;

    Program *program = &programs[targetToProgramIndex(currentTarget)];
    program->glProgram->setUniformValue(program->isrgbUniformPos, false);

    if (origin == QOpenGLTextureBlitter::OriginTopLeft) {
        if (program->textureMatrixUniformState != IdentityFlipped) {
            QMatrix3x3 sourceTransform;
            sourceTransform(1,1) = -1;
            sourceTransform(1,2) = 1;
            const QMatrix3x3 textureTransform = toTextureCoordinates(sourceTransform);
            program->glProgram->setUniformValue(program->textureTransformUniformPos, textureTransform);
            program->textureMatrixUniformState = IdentityFlipped;
        }
    } else if (program->textureMatrixUniformState != Identity) {
        const QMatrix3x3 textureTransform = toTextureCoordinates(QMatrix3x3());
        program->glProgram->setUniformValue(program->textureTransformUniformPos, textureTransform);
        program->textureMatrixUniformState = Identity;
    }

    if(format == QOpenGLTexture::RGB)
        program->glProgram->setUniformValue(program->isrgbUniformPos, true);

    QOpenGLContext::currentContext()->functions()->glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);
}

bool BlitterPrivate::buildProgram(ProgramIndex idx, const char *vs, const char *fs)
{
    Program *p = &programs[idx];

    p->glProgram.reset(new QOpenGLShaderProgram);

    p->glProgram->addCacheableShaderFromSourceCode(QOpenGLShader::Vertex, vs);
    p->glProgram->addCacheableShaderFromSourceCode(QOpenGLShader::Fragment, fs);
    p->glProgram->link();
    if (!p->glProgram->isLinked()) {
        qWarning() << "Could not link shader program:\n" << p->glProgram->log();
        return false;
    }

    p->glProgram->bind();

    p->vertexCoordAttribPos = p->glProgram->attributeLocation("vertexCoord");
    p->vertexTransformUniformPos = p->glProgram->uniformLocation("vertexTransform");
    p->textureCoordAttribPos = p->glProgram->attributeLocation("textureCoord");
    p->textureTransformUniformPos = p->glProgram->uniformLocation("textureTransform");
    p->opacityUniformPos = p->glProgram->uniformLocation("opacity");
    p->isrgbUniformPos = p->glProgram->uniformLocation("isrgb");

    p->glProgram->setUniformValue(p->isrgbUniformPos, false);

    // minmize state left set after a create()
    p->glProgram->release();

    return true;
}

bool BlitterPrivate::ensureProgram(ProgramIndex idx)
{
    if (programs[idx].glProgram)
        return true;

    QOpenGLContext *currentContext = QOpenGLContext::currentContext();
    if (!currentContext)
        return false;

    QSurfaceFormat format = currentContext->format();
    if (format.profile() == QSurfaceFormat::CoreProfile && format.version() >= qMakePair(3,2)) {
        if (idx == BlitterPrivate::TEXTURE_RECTANGLE && q->supportsRectangleTarget()) {
            if (!buildProgram(idx, vertex_shader150, fragment_shader150_rectangle))
                return false;
        }
    } else {
        if (idx == BlitterPrivate::TEXTURE_RECTANGLE && q->supportsRectangleTarget()) {
            if (!buildProgram(idx, vertex_shader, fragment_shader_rectangle))
                return false;
        }
        if (idx == BlitterPrivate::TEXTURE_EXTERNAL_OES && q->supportsExternalOESTarget()) {
            if (!buildProgram(idx, vertex_shader, fragment_shader_external_oes))
                return false;
        }
    }

    return !programs[idx].glProgram.isNull();
}

Blitter::Blitter()
    : d_ptr(new BlitterPrivate(this))
{
}

Blitter::~Blitter()
{
    destroy();
}

bool Blitter::create()
{
    QOpenGLContext *currentContext = QOpenGLContext::currentContext();
    if (!currentContext)
        return false;

    Q_D(Blitter);

    if (d->programs[BlitterPrivate::TEXTURE_2D].glProgram)
        return true;

    QSurfaceFormat format = currentContext->format();
    // Build the most common, 2D texture shader variant.
    // The other special ones are deferred and compiled only when first needed.
    if (format.profile() == QSurfaceFormat::CoreProfile && format.version() >= qMakePair(3,2)) {
        if (!d->buildProgram(BlitterPrivate::TEXTURE_2D, vertex_shader150, fragment_shader150))
            return false;
    } else {
        if (!d->buildProgram(BlitterPrivate::TEXTURE_2D, vertex_shader, fragment_shader))
            return false;
    }

    // Create and bind the VAO, if supported.
    QOpenGLVertexArrayObject::Binder vaoBinder(d->vao.data());

    d->vertexBuffer.create();
    d->vertexBuffer.bind();
    d->vertexBuffer.allocate(vertex_buffer_data, sizeof(vertex_buffer_data));
    d->vertexBuffer.release();

    d->textureBuffer.create();
    d->textureBuffer.bind();
    d->textureBuffer.allocate(texture_buffer_data, sizeof(texture_buffer_data));
    d->textureBuffer.release();

    return true;
}

bool Blitter::isCreated() const
{
    Q_D(const Blitter);
    return !d->programs[BlitterPrivate::TEXTURE_2D].glProgram.isNull();
}

void Blitter::destroy()
{
    if (!isCreated())
        return;
    Q_D(Blitter);
    d->programs[BlitterPrivate::TEXTURE_2D].glProgram.reset();
    d->programs[BlitterPrivate::TEXTURE_EXTERNAL_OES].glProgram.reset();
    d->programs[BlitterPrivate::TEXTURE_RECTANGLE].glProgram.reset();
    d->vertexBuffer.destroy();
    d->textureBuffer.destroy();
    d->vao.reset();
}


bool Blitter::supportsExternalOESTarget() const
{
    QOpenGLContext *ctx = QOpenGLContext::currentContext();
    return ctx && ctx->isOpenGLES() && ctx->hasExtension("GL_OES_EGL_image_external");
}

bool Blitter::supportsRectangleTarget() const
{
    QOpenGLContext *ctx = QOpenGLContext::currentContext();
    if (!ctx || ctx->isOpenGLES())
        return false;

    if (ctx->hasExtension("GL_ARB_texture_rectangle"))
        return true;

    if (ctx->hasExtension("GL_EXT_texture_rectangle"))
        return true;

    QSurfaceFormat f = ctx->format();
    const auto version = qMakePair(f.majorVersion(), f.minorVersion());
    if (version >= qMakePair(3, 1))
        return true;

    return false;
}

void Blitter::bind(GLenum target)
{
    Q_D(Blitter);

    if (d->vao->isCreated())
        d->vao->bind();

    d->currentTarget = target;
    BlitterPrivate::ProgramIndex programIndex = targetToProgramIndex(target);
    if (!d->ensureProgram(programIndex))
        return;

    BlitterPrivate::Program *p = &d->programs[programIndex];
    p->glProgram->bind();

    d->vertexBuffer.bind();
    p->glProgram->setAttributeBuffer(p->vertexCoordAttribPos, GL_FLOAT, 0, 2, 0);
    p->glProgram->enableAttributeArray(p->vertexCoordAttribPos);
    d->vertexBuffer.release();

    d->textureBuffer.bind();
    p->glProgram->setAttributeBuffer(p->textureCoordAttribPos, GL_FLOAT, 0, 2, 0);
    p->glProgram->enableAttributeArray(p->textureCoordAttribPos);
    d->textureBuffer.release();
}

void Blitter::release()
{
    Q_D(Blitter);
    BlitterPrivate::Program *p = &d->programs[targetToProgramIndex(d->currentTarget)];
    if (p->glProgram)
        p->glProgram->release();
    if (d->vao->isCreated())
        d->vao->release();
}

void Blitter::setOpacity(float opacity)
{
    Q_D(Blitter);
    d->opacity = opacity;
}

void Blitter::blit(GLuint texture,
                                 const QMatrix4x4 &targetTransform,
                                 QOpenGLTextureBlitter::Origin sourceOrigin,
                                 QOpenGLTexture::TextureFormat format)
{
    Q_D(Blitter);
    d->blit(texture, targetTransform, sourceOrigin, format);
}

void Blitter::blit(GLuint texture,
                                 const QMatrix4x4 &targetTransform,
                                 const QMatrix3x3 &sourceTransform,
                                 QOpenGLTexture::TextureFormat format)
{
    Q_D(Blitter);
    d->blit(texture, targetTransform, sourceTransform, format);
}

QMatrix4x4 Blitter::targetTransform(const QRectF &target,
                                                  const QRect &viewport)
{
    qreal x_scale = target.width() / viewport.width();
    qreal y_scale = target.height() / viewport.height();

    const QPointF relative_to_viewport = target.topLeft() - viewport.topLeft();
    qreal x_translate = x_scale - 1 + ((relative_to_viewport.x() / viewport.width()) * 2);
    qreal y_translate = -y_scale + 1 - ((relative_to_viewport.y() / viewport.height()) * 2);

    QMatrix4x4 matrix;
    matrix(0,3) = x_translate;
    matrix(1,3) = y_translate;

    matrix(0,0) = x_scale;
    matrix(1,1) = y_scale;

    return matrix;
}

QMatrix3x3 Blitter::sourceTransform(const QRectF &subTexture,
                                                  const QSize &textureSize,
                                                  QOpenGLTextureBlitter::Origin origin)
{
    qreal x_scale = subTexture.width() / textureSize.width();
    qreal y_scale = subTexture.height() / textureSize.height();

    const QPointF topLeft = subTexture.topLeft();
    qreal x_translate = topLeft.x() / textureSize.width();
    qreal y_translate = topLeft.y() / textureSize.height();

    if (origin == QOpenGLTextureBlitter::OriginTopLeft) {
        y_scale = -y_scale;
        y_translate = 1 - y_translate;
    }

    QMatrix3x3 matrix;
    matrix(0,2) = x_translate;
    matrix(1,2) = y_translate;

    matrix(0,0) = x_scale;
    matrix(1,1) = y_scale;

    return matrix;
}
