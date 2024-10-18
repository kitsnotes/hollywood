// Copyright (C) 2023 Originull Software
// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

#ifndef BLITTER_H
#define BLITTER_H

#include <QtOpenGL/qtopenglglobal.h>

#include <QtGui/qopengl.h>
#include <QtGui/QMatrix3x3>
#include <QtGui/QMatrix4x4>
#include <QtOpenGL/qopengltextureblitter.h>
#include <QtOpenGL/qopengltexture.h>

class BlitterPrivate;
class Q_OPENGL_EXPORT Blitter
{
public:
    Blitter();
    ~Blitter();

    bool create();
    bool isCreated() const;
    void destroy();

    bool supportsExternalOESTarget() const;
    bool supportsRectangleTarget() const;

    void bind(GLenum target = GL_TEXTURE_2D);
    void release();

    void setRedBlueSwizzle(bool swizzle);
    void setOpacity(float opacity);

    void blit(GLuint texture, const QMatrix4x4 &targetTransform, QOpenGLTextureBlitter::Origin sourceOrigin, QOpenGLTexture::TextureFormat format);
    void blit(GLuint texture, const QMatrix4x4 &targetTransform, const QMatrix3x3 &sourceTransform, QOpenGLTexture::TextureFormat format);

    static QMatrix4x4 targetTransform(const QRectF &target, const QRect &viewport);
    static QMatrix3x3 sourceTransform(const QRectF &subTexture, const QSize &textureSize, QOpenGLTextureBlitter::Origin origin);

private:
    Q_DISABLE_COPY(Blitter)
    Q_DECLARE_PRIVATE(Blitter)
    QScopedPointer<BlitterPrivate> d_ptr;
};

#endif //BLITTER_H
