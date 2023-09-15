#include "labeledslider.h"

#include <QStylePainter>
#include <QStyleOptionSlider>
#include <QMouseEvent>

LabeledSlider::LabeledSlider(QWidget *parent)
    :QSlider(parent)
{
    // TODO: test this class on touch
    QFont font;
    QFontMetrics fm(font);
    m_offset = fm.height();
    setContentsMargins(0,0,0,m_offset);
}

QSize LabeledSlider::minimumSizeHint() const
{
    auto size = QSlider::minimumSizeHint();
    if(orientation() == Qt::Horizontal)
        size.setHeight(size.height()+m_offset+5);

    return size;
}

void LabeledSlider::setTickLabels(QMap<int, QString> &labels)
{
    m_tickLabels = labels;
    m_drawNums = false;
}

void LabeledSlider::mouseMoveEvent(QMouseEvent *ev)
{
    if (tickPosition() == NoTicks)
    {
        QSlider::mouseMoveEvent(ev);
        return;
    }
    auto pos = ev->pos();
    pos.setY(pos.y()+m_offset);
    auto gpos = ev->globalPosition();
    gpos.setY(gpos.y()+m_offset);
    ev->accept();
    QMouseEvent *ev2 = new QMouseEvent(ev->type(), pos, gpos, ev->button(), ev->buttons(), ev->modifiers());
    QSlider::mouseMoveEvent(ev2);
}


void LabeledSlider::mousePressEvent(QMouseEvent *ev)
{
    if (tickPosition() == NoTicks)
    {
        QSlider::mousePressEvent(ev);
        return;
    }
    auto pos = ev->pos();
    pos.setY(pos.y()+m_offset);
    auto gpos = ev->globalPosition();
    gpos.setY(gpos.y()+m_offset);
    ev->accept();
    QMouseEvent *ev2 = new QMouseEvent(ev->type(), pos, gpos, ev->button(), ev->buttons(), ev->modifiers());
    QSlider::mousePressEvent(ev2);
}

void LabeledSlider::paintEvent(QPaintEvent *ev)
{
   if (tickPosition() == NoTicks)
   {
      QSlider::paintEvent(ev);
      return;
   }

   QStylePainter p1(this);
   QStyleOptionSlider opt;
   initStyleOption(&opt);

   int fontOffset = p1.fontMetrics().height();

   if (orientation() == Qt::Vertical)
   {
      opt.rect.adjust(15, 0, 0, 0);
      if (m_firstWidth == 0)
         m_firstWidth = width();
      setMinimumWidth(m_firstWidth * 2);
   }

   opt.rect.setHeight(opt.rect.height()-fontOffset+2);
   // draw the slider (this is basically copy/pasted from QSlider::paintEvent)
   opt.subControls = QStyle::SC_SliderTickmarks;
   p1.drawComplexControl(QStyle::CC_Slider, opt);


   opt.subControls = QStyle::SC_SliderGroove;
   p1.drawComplexControl(QStyle::CC_Slider, opt);

   // draw the slider handle
   opt.subControls = QStyle::SC_SliderHandle;
   p1.drawComplexControl(QStyle::CC_Slider, opt);

   QRect rr;
   int rightLast = 0;
   bool first, last = false;
   for (int v = this->minimum(); v <= this->maximum(); v += tickInterval())
   {

      QStyle *st = style();
      QStyleOptionSlider slider;
#if QT_VERSION >= 0x060000
      slider.initFrom(this);
#else
      slider.init(this);
#endif
      slider.orientation = orientation();
      int available = st->pixelMetric(QStyle::PM_SliderSpaceAvailable, &slider, this);

      int len = st->pixelMetric(QStyle::PM_SliderLength, &slider, this);
      int left = QStyle::sliderPositionFromValue(minimum(), maximum(), v, available);
      QString vs;
      if(m_drawNums)
      {
         vs = QString("%L1").arg(v);
      }
      else
      {
         vs = m_tickLabels[v];
      }
      if(vs.length() == 1 && first)
         left += 4;

      if(!first)
         left += 2;

      // last item
      if (qAbs(maximum() - v) < tickInterval())
      {
         last = true;
         if(m_drawNums)
             left -= 2;
         else
             left -= 20;
      }
      QPoint pos(left, rect().bottom());
      if (orientation() == Qt::Vertical)
      {
         pos = QPoint(rect().left(), left + len);
         vs = QString("%L1").arg(this->minimum() - v + this->maximum());
      }
      QRect r;
      if(!first || last || (left < available && rightLast <= left - len))
      {
         rr.setX(pos.x());
         rr.setY(pos.y());
         p1.drawText(rr, Qt::TextDontPrint, vs, &r);
         p1.drawText(pos, vs);
      }
      rightLast = left + r.width();
      first = true;
   }
}


