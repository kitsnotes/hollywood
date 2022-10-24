#include "chartobject.h"

#define PORTAL_ORANGE   "#FF9A00"
#define PORTAL_BLUE     "#00A2FF"
#define BUFFERCACHE     "#4AC7BA"

ChartObject::ChartObject(ChartType type, QObject *parent, qulonglong ramsize)
    : QObject(parent),
      m_type(type),
      m_chart(new QChart(0)),
      m_series(new QAreaSeries(new QLineSeries(this), new QLineSeries(this)))
{
    m_chart->setAnimationOptions(QChart::NoAnimation);
    m_chart->setBackgroundVisible(false);
    m_chart->setMargins(QMargins(0,0,0,0));
    m_x = new QCategoryAxis(this);
    m_x->setMin(0);
    m_x->setMax(60);
    m_x->setReverse(true);
    m_x->setLabelsVisible(true);
    m_x->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);
    m_x->append(tr("Now"),1);
    m_x->append(tr("60 Seconds"),59);
    m_currentXString = tr("60 Seconds");


    m_series->upperSeries()->setUseOpenGL(true);
    m_series->lowerSeries()->setUseOpenGL(true);

    m_y = new QValueAxis(this);
    float gib = (float)ramsize/1024;
    if(type == CPU)
    {
        m_y->setRange(0,100); // default 0-100%
        m_y->setLabelFormat("%d");
        m_series->setName(tr("Combined CPU Average"));
    }
    if(type == RAM)
    {
        m_series->setName(tr("Application"));

        // we get sent MiB so convert to GiB
        gib = gib/1024;
        m_y->setRange(0,gib);
        m_y->setMax(gib);
        m_y->setLabelFormat("%.2f");

        m_y2 = new QValueAxis(this);
        m_y2->setRange(0,gib);
        m_y2->setMax(gib);
        m_y2->setLabelFormat("%.2f");
        m_series2 = new QAreaSeries(new QLineSeries(this), new QLineSeries(this));
        m_series2->upperSeries()->setUseOpenGL(true);
        m_series2->lowerSeries()->setUseOpenGL(true);
        m_series2->setName(tr("Buffers/Cache"));
        m_y->applyNiceNumbers();
        setupRAMColors();
    }
    if(type == Disk)
    {
        m_y->setRange(0,100);
        m_y->setLabelFormat("%i KB");
        m_y2 = new QValueAxis(this);
        m_y2->setRange(0,100);
        m_y2->setLabelFormat("%i KB");
        m_series2 = new QAreaSeries(new QLineSeries(this), new QLineSeries(this));
        m_series2->upperSeries()->setUseOpenGL(true);
        m_series2->lowerSeries()->setUseOpenGL(true);
        setIOColors();
        m_series->setName(tr("Disk Read"));
        m_series2->setName(tr("Disk Write"));
        m_y->applyNiceNumbers();
    }

    m_chart->addAxis(m_x, Qt::AlignBottom);
    m_chart->addAxis(m_y, Qt::AlignLeft);
    if(type == Disk || type == RAM)
    {
        m_chart->addAxis(m_y2, Qt::AlignRight);
        m_y2->setLabelsVisible(false);
    }
    m_chart->legend()->hide();
    m_series->setPointsVisible(false);

    // clear our our sample vector
    // it is so ghetto that we have to init random
    // values but qt charts hates it otherwise
    if (m_values.isEmpty())
    {
        m_values.reserve(samples);
        for (int i = 0; i < samples; ++i)
        {
            if(type == RAM)
            {
                float r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX/gib);
                m_values.append(QPointF((float)i, r));
            }
            else
                m_values.append(QPoint(i, rand()%100));
        }
    }
    m_series->upperSeries()->append(m_values);
    m_series->lowerSeries()->append(m_values);
    m_chart->addSeries(m_series);
    for(int i = 0; i < samples; ++i)
        m_values[i].setY(0);
    m_series->upperSeries()->replace(m_values);
    m_series->lowerSeries()->replace(m_values);

    if(type == Disk)
    {
        m_values2.reserve(samples);
        for (int i = 0; i < samples; ++i)
            m_values2.append(QPoint(i, rand()%100));

        m_series2->upperSeries()->append(m_values2);
        m_series2->lowerSeries()->append(m_values2);
        m_chart->addSeries(m_series2);
        for(int i = 0; i < samples; ++i)
            m_values2[i].setY(0);
        m_series2->lowerSeries()->replace(m_values2);
        m_series2->upperSeries()->replace(m_values2);
        m_y2->applyNiceNumbers();
    }

    if(type == RAM)
    {
        m_values2.reserve(samples);
        for (int i = 0; i < samples; ++i)
        {
            float r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX/gib);
            m_values2.append(QPointF((float)i, r));
        }

        m_series2->upperSeries()->append(m_values2);
        m_series2->lowerSeries()->append(m_values2);
        m_chart->addSeries(m_series2);
        for(int i = 0; i < samples; ++i)
            m_values2[i].setY(0);
        m_series2->lowerSeries()->replace(m_values2);
        m_series2->upperSeries()->replace(m_values2);
   }

    m_chart->legend()->setVisible(true);
    m_chart->legend()->setAlignment(Qt::AlignBottom);
}

QChart *ChartObject::chart()
{
    return m_chart;
}

void ChartObject::updateXAxis(const QString &speed)
{
    m_x->replaceLabel(m_currentXString, speed);
    m_currentXString = speed;
}

void ChartObject::pushSample(float value)
{
    for(int i = 0; i < samples; ++i)
        m_values[i].setX(i-1);
    m_values.removeFirst();
    m_values.append(QPoint(samples,value));
    m_series->upperSeries()->replace(m_values);
    m_chart->update();
}

void ChartObject::setChartAccentColor(const QColor &color)
{
    QColor darkerColor(color.darker(135));
    m_series->setBorderColor(darkerColor);
    QLinearGradient g(QPointF(0,0), QPointF(0,1));
    g.setColorAt(0.0, color);
    g.setColorAt(1.0, color.lighter(120));
    g.setCoordinateMode(QGradient::ObjectBoundingMode);
    m_series->setBrush(g);
}

void ChartObject::pushMemory(qulonglong total, qulonglong free, qulonglong avail, qulonglong buffers, qulonglong cache)
{
    Q_UNUSED(avail);
    // this comes to us in KiB
    float mem = (float)(total-free-buffers-cache);
    mem = mem/1024/1024;
    float buffers2 = (float)buffers/1024/1024;
    float cache2 = (float)cache/1024/1024;
    for(int i = 0; i < samples; ++i)
    {
        m_values[i].setX(i-1);
        m_values2[i].setX(i-1);
    }
    m_values.removeFirst();
    m_values2.removeFirst();
    m_values.append(QPointF(samples,mem));
    m_values2.append(QPointF(samples,mem+buffers2+cache2));

    m_series->upperSeries()->replace(m_values);
    m_series2->lowerSeries()->replace(m_values);
    m_series2->upperSeries()->replace(m_values2);

    m_chart->update();
}

void ChartObject::pushDiskStat(qulonglong read, qulonglong write)
{
    // axis m_y is read m_y2 is write
    // this comes to us in bytes
    for(int i = 0; i < samples; ++i)
    {
        m_values[i].setX(i-1);
        m_values2[i].setX(i-1);
    }
    m_values.removeFirst();
    m_values2.removeFirst();
    m_values.append(QPointF(samples,read));
    m_values2.append(QPointF(samples,write));
    m_series->upperSeries()->replace(m_values);
    m_series2->upperSeries()->replace(m_values2);
    updateDiskCeiling();
    m_chart->update();
}

void ChartObject::updateDiskCeiling()
{
    // working in KiB
    uint min_disk_ceiling = 50;
    uint top_write = 0;
    uint top_read = 0;

    for(int i = 0; i < samples; ++i)
    {
        if(m_values[i].y() > top_read)
            top_read = m_values[i].y();
        if(m_values2[i].y() > top_write)
            top_write = m_values2[i].y();
    }

    uint top_max = qMax(top_write, top_read);

    // give a 10% buffer on the top of the max space
    uint top_ten = top_max * 10 / 100;
    uint val = qMax(top_max+top_ten, min_disk_ceiling);

    m_y->setRange(0,val);
    m_y->setMax(val);
    m_y2->setRange(0,val);
    m_y2->setMax(val);
    m_y->applyNiceNumbers();
    m_y2->applyNiceNumbers();

}

void ChartObject::setIOColors()
{
    QColor write(PORTAL_ORANGE);
    QColor read(PORTAL_BLUE);

    QColor darkerColor(read.darker(135));
    m_series->setBorderColor(darkerColor);
    QLinearGradient g(QPointF(0,0), QPointF(0,1));
    g.setColorAt(0.0, read);
    g.setColorAt(1.0, read.lighter(120));
    g.setCoordinateMode(QGradient::ObjectBoundingMode);
    m_series->setBrush(g);
    m_series->setOpacity(0.7);

    QColor darkerColorW(write.darker(135));
    m_series2->setBorderColor(darkerColorW);
    QLinearGradient g2(QPointF(0,0), QPointF(0,1));
    g2.setColorAt(0.0, write);
    g2.setColorAt(1.0, write.lighter(120));
    g2.setCoordinateMode(QGradient::ObjectBoundingMode);
    m_series2->setBrush(g2);
    m_series2->setOpacity(0.7);
}

void ChartObject::setupRAMColors()
{
    QColor bc(BUFFERCACHE);
    QColor darkerColorW(bc.darker(135));
    m_series2->setBorderColor(darkerColorW);
    QLinearGradient g2(QPointF(0,0), QPointF(0,1));
    g2.setColorAt(0.0, bc);
    g2.setColorAt(1.0, bc.lighter(110));
    g2.setCoordinateMode(QGradient::ObjectBoundingMode);
    m_series2->setBrush(g2);
}
