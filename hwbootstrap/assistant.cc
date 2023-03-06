#include "assistant.h"
#include "bootstrap.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QSpacerItem>
#include <QSvgRenderer>
#include <QPainter>

void ThemedPage::loadWatermark(std::string page)
{
    QPalette p = bsApp->palette();
    auto tc = p.color(QPalette::Active, QPalette::WindowText);
    auto bc = p.color(QPalette::Active, QPalette::Window);

    QSvgRenderer renderer(this);
    renderer.load(QString(":/BGLight"));

    renderer.setAspectRatioMode(Qt::KeepAspectRatio);

    QSvgRenderer logo(QString(":/Logo"), this);
    logo.setAspectRatioMode(Qt::KeepAspectRatio);

    QPixmap pm(700,500);
    pm.fill(bc);
    QPainter painter(&pm);
    painter.setOpacity(0.8);
    painter.setBrush(QBrush(bc));
    renderer.render(&painter, pm.rect());
    painter.setBrush(QBrush(tc));
    int startx = 25;
    int starty = 35;
    int img = painter.fontMetrics().height();

    int lw = 120;
    int lh = 60;
    QRect lr(startx,starty,lw,lh);
    logo.render(&painter, lr);

    starty += lh + 10;

    QList<QPair<QString,QString>> pages;
    pages << QPair<QString,QString>("disk", tr("Setup Disk"));
    pages << QPair<QString,QString>("settings", tr("Copy Settings"));
    pages << QPair<QString,QString>("passwd", tr("Set Password"));
    pages << QPair<QString,QString>("review", tr("Review"));

    QIcon completed(":/Success");
    QIcon current(":/Current");
    QStringList levels;
    for(auto &i : pages)
        levels.append(i.second);

    QString currentLevel;
    for(auto &i : pages)
    {
        if(i.first == QString::fromStdString(page))
            currentLevel = i.second;
    }

    painter.setBrush(QBrush(tc));
    painter.setBackground(QBrush(tc));
    int levelInt = levels.indexOf(currentLevel);
    for(int i = 0; i < levels.count(); ++i)
    {
        if(levelInt == i)
        {
            painter.drawPixmap(QPoint(startx,starty),
                               current.pixmap(QSize(img,img)));
            QFont font = painter.font();
            font.setBold(true);
            painter.setFont(font);
        }
        else
        {
            if(i < levelInt)
            {
                painter.drawPixmap(QPoint(startx,starty),
                                   completed.pixmap(QSize(img,img)));
            }
            QFont font = painter.font();
            font.setBold(false);
            painter.setFont(font);
        }

        painter.setPen(tc);
        painter.drawText(QPoint(startx+img+5,starty+15), levels[i]);
        starty += painter.fontMetrics().height() + 5;
    }

    setPixmap(QWizard::BackgroundPixmap, pm);
}
PartitionPage::PartitionPage(QWidget *parent)
    : ThemedPage(parent)
    , m_splitter(new QSplitter(this))
{
    setTitle(tr("Disk Partitioning"));
    loadWatermark("disk");
    auto layout = new QVBoxLayout;
    m_splitter->setOrientation(Qt::Vertical);

    auto desc = new QLabel(this);
    desc->setWordWrap(true);
    desc->setText(tr("Each operating system requires it's own unique disk partition. Drag the divider below to set the size of each disk partition."));

    layout->addWidget(desc);
    layout->addWidget(m_splitter);


    layout->addSpacerItem(new QSpacerItem(1,1,QSizePolicy::Minimum,QSizePolicy::Expanding));
    setLayout(layout);
}

SettingsPage::SettingsPage(QWidget *parent)
    : ThemedPage(parent)
    , m_keyboard(new QCheckBox(this))
    , m_mouse(new QCheckBox(this))
    , m_display(new QCheckBox(this))
    , m_icc(new QCheckBox(this))
    , m_appearance(new QCheckBox(this))
    , m_wallpaper(new QCheckBox(this))
    , m_regional(new QCheckBox(this))
{
    setTitle(tr("Migrate Settings"));
    loadWatermark("settings");
    m_keyboard->setText(tr("Keyboard Settings"));
    m_mouse->setText(tr("Mouse && Keyboard Settings"));
    m_display->setText(tr("Display Positioning"));
    m_icc->setText(tr("Display Color Profiles"));
    m_appearance->setText(tr("Appearance Settings"));
    m_wallpaper->setText(tr("Desktop Wallpaper"));
    m_regional->setText(tr("Regional Settings"));

    m_keyboard->setChecked(true);
    m_mouse->setChecked(true);
    m_display->setChecked(true);
    m_icc->setChecked(true);
    m_appearance->setChecked(true);
    m_wallpaper->setChecked(true);
    m_regional->setChecked(true);

    auto layout = new QVBoxLayout;

    auto desc = new QLabel(this);
    desc->setWordWrap(true);
    desc->setText(tr("Hollywood will migrate all checked personal settings. Uncheck any settings you do not wish to transfer below."));
    layout->addWidget(desc);
    layout->addWidget(m_appearance);
    layout->addWidget(m_wallpaper);
    layout->addWidget(m_regional);
    layout->addWidget(m_keyboard);
    layout->addWidget(m_mouse);
    layout->addWidget(m_display);
    layout->addWidget(m_icc);

    layout->addSpacerItem(new QSpacerItem(1,1,QSizePolicy::Minimum,QSizePolicy::Expanding));
    setLayout(layout);
}

ReviewPage::ReviewPage(QWidget *parent)
    : ThemedPage(parent)
    , m_review(new QTextBrowser(this))
{
    setTitle(tr("Review Installation Operations"));
    loadWatermark("review");
    auto layout = new QVBoxLayout;

    auto desc = new QLabel(this);
    desc->setWordWrap(true);
    desc->setText(tr("The following changes will be made after you click Done below. Your system should remain on AC mains power until the installation process is complete."));
    layout->addWidget(desc);
    layout->addWidget(m_review);
    setLayout(layout);
}


SetupAssistant::SetupAssistant(QWidget *parent)
    : QWizard(parent)
    , m_disk(new PartitionPage)
    , m_settings(new SettingsPage)
    , m_review(new ReviewPage)
{
    setWindowTitle(tr("Hollywood Setup Assistant"));
    addPage(m_disk);
    addPage(m_settings);
    addPage(m_review);
    setWizardStyle(MacStyle);
    setMaximumWidth(700);
    setMinimumWidth(700);
    setMaximumHeight(480);
    setMinimumHeight(480);
}

