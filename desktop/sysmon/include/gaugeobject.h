// This file based on code that was sourced from
// https://github.com/MarkVTech/CircularGauge
// MIT License

#ifndef CPUGAUGE_H
#define CPUGAUGE_H

#include <QGraphicsObject>
#include <QRectF>
#include <QRadialGradient>
#include <QBrush>
#include <QPen>
#include <QColor>

class QVariantAnimation;

class GPUGauge : public QGraphicsObject
{
    Q_OBJECT
    Q_PROPERTY(qreal value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(QColor valueColor READ valueColor WRITE setValueColor NOTIFY valueColorChanged)
public:
    GPUGauge(const QRectF& rect, QGraphicsItem* parent = nullptr);

public:
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    qreal value() const;
    QColor valueColor() const;
public slots:
    void setValue(qreal value);
    void setValueColor(const QColor& color);
private slots:
    void handleDialAnimationValueChanged(const QVariant& newValue);

signals:
    void valueChanged(qreal newValue);
    void valueColorChanged(const QColor& newColor);

private:
    QRectF mRect;
    qreal mPenWidth;
    qreal mMinValue, mMaxValue;
    qreal mRatio;
    qreal mValue;
    qreal mGaugeValue;
    int mStartAngle, mAngleSpan;

    //QRadialGradient mQrg;
    QBrush mBrush;
    QColor mValueColor = QColor(74,199,186);
    bool mGlowRingEnabled = false;

    QVariantAnimation* mDialAnimation;
public:
    int type() const override;
};

#endif // CPUGAUGE_H
