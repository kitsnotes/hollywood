// This file based on code that was sourced from
// https://github.com/MarkVTech/CircularGauge
// MIT License

#include <QDebug>
#include <QPainter>
#include <QColor>
#include <QSizeF>
#include <QRectF>
#include <QVariantAnimation>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QFontInfo>
#include <QFontMetrics>
#include <QMarginsF>
#include <QPalette>
#include <QApplication>

#include "gaugeobject.h"

GPUGauge::GPUGauge(const QRectF &rect, QGraphicsItem *parent) :
    QGraphicsObject(parent),
    mRect(rect),
    mValue(100.0),
    mGaugeValue(mValue),
    mStartAngle(210*16),
    mAngleSpan(-240*16),
    mDialAnimation(new QVariantAnimation(this))
{
    setAcceptHoverEvents(true);
    mRect = mRect.marginsAdded(QMarginsF(0,-3,0,0));
    mRatio = static_cast<qreal>(mAngleSpan)/(mMaxValue-mMinValue);

    connect(mDialAnimation, SIGNAL(valueChanged(QVariant)), this, SLOT(handleDialAnimationValueChanged(QVariant)));
}

QRectF GPUGauge::boundingRect() const
{
    return mRect + QMarginsF(5, 5, 5, 5);
}

void GPUGauge::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->save();

    painter->setPen(mValueColor);
    painter->setBrush(mValueColor);
    qreal newSpan = (mGaugeValue-mMinValue)*mRatio;
    painter->drawPie(mRect, -180*16, -130*16);

    //
    // Draw center
    //
    QPalette p = qApp->palette();
    auto bg = p.color(QPalette::Current, QPalette::Window);
    painter->setPen(bg);
    painter->setBrush(bg);
    QSizeF centerSize(mRect.width()*0.50, mRect.height()*0.50);
    QRectF centerEllipse(QPointF(0.0, 0.0), centerSize);
    centerEllipse.moveCenter(mRect.center());
    painter->drawPie(centerEllipse, -180*16, -180*16);

    int starty = mRect.height()/2;
    starty = starty + 2;

    QString valueStr = QString("1.5GHz");//arg(mValue, 2, 'f', 2);
    QFont font = painter->font();
    font.setFixedPitch(true);
    font.setPointSize(9);
    painter->setFont(font);

    auto fg = p.color(QPalette::Current, QPalette::Text);
    painter->setPen(fg);
    QRectF txtRect(0, starty, mRect.width(), starty);
    painter->drawText(txtRect, Qt::AlignCenter, valueStr);

    painter->restore();
}

qreal GPUGauge::value() const
{
    return mValue;
}

QColor GPUGauge::valueColor() const
{
    return mValueColor;
}

void GPUGauge::setValue(qreal value)
{
    mDialAnimation->setStartValue(mGaugeValue);
    mDialAnimation->setEndValue(value);
    mDialAnimation->setDuration(500);
    mDialAnimation->start();

    mValue = value;
}

void GPUGauge::setValueColor(const QColor &color)
{
    if ( mValueColor == color )
        return;

    mValueColor = color;
    update();

    emit valueColorChanged(mValueColor);
}

void GPUGauge::handleDialAnimationValueChanged(const QVariant &newValue)
{
    mGaugeValue = newValue.toDouble();

    update();
}

int GPUGauge::type() const
{
    return QGraphicsItem::UserType + 1;
}
