#include "displaysapplet.h"
#include "screen.h"
#include "screenmanager.h"
#include "smanwidget.h"

#include <QtWidgets/QButtonGroup>

static QStringList g_resolutions = QStringList()
        << "640x480"
        << "800x600"
        << "1024x768"
        << "1280x720"
        << "1280x1024"
        << "1600x1200"
        << "1920x1080"
        << "2560x1440"
        << "3840x2160";

DisplaysApplet::DisplaysApplet(QObject *parent)
    : QObject(parent)
    , SettingsAppletInterface()
{
}

bool DisplaysApplet::init()
{
    setupWidget();
    return true;
}

QString DisplaysApplet::id() const
{
    return QLatin1String("org.originull.displays");
}

QString DisplaysApplet::name() const
{
    return tr("Displays");
}

QString DisplaysApplet::description() const
{
    return tr("Configure and re-arrange system displays and color management.");
}

QIcon DisplaysApplet::icon() const
{
    return QIcon::fromTheme("preferences-desktop-display");
}

QWidget *DisplaysApplet::applet() const
{
    // return a const version of our m_host applet
    if(!m_host)
        return nullptr;

    return const_cast<QWidget*>(m_host);
}

SettingsAppletInterface::Category DisplaysApplet::category() const
{
    return System;
}

QStringList DisplaysApplet::searchTokens() const
{
    return QStringList();
}

void DisplaysApplet::setupWidget()
{
    m_host = new QWidget(0);
    if (m_host->objectName().isEmpty())
        m_host->setObjectName(QString::fromUtf8("DisplayAppletHost"));
    m_host->resize(625, 439);
    m_layout = new QVBoxLayout(m_host);

    m_manager = new ScreenManager(this);
    m_sman = new ScreenManagerWidget(m_manager, m_host);

    auto hl_opt = new QHBoxLayout();
    m_mode_off = new QRadioButton(m_host);
    bg_mode = new QButtonGroup(m_host);
    m_mode_extend = new QRadioButton(m_host);
    m_mode_primary = new QRadioButton(m_host);
    m_mode_mirror = new QRadioButton(m_host);
    m_mirrorselect = new QComboBox(m_host);

    m_mode_off->setText(tr("Disabled"));
    m_mode_primary->setText(tr("Primary"));
    m_mode_extend->setText(tr("Extend Desktop"));
    m_mode_mirror->setText(tr("Mirror Display:"));
    bg_mode->addButton(m_mode_off);
    bg_mode->addButton(m_mode_primary);
    bg_mode->addButton(m_mode_extend);
    bg_mode->addButton(m_mode_mirror);
    hl_opt->addWidget(m_mode_off);
    hl_opt->addWidget(m_mode_primary);
    hl_opt->addWidget(m_mode_extend);
    hl_opt->addWidget(m_mode_mirror);
    hl_opt->addWidget(m_mirrorselect);
    hl_opt->setStretch(4, 1);

    auto line = new QFrame(m_host);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    auto hl_resolution = new QHBoxLayout();

    l_orientation = new QLabel(m_host);
    l_orientation->setText(tr("Rotation:"));
    m_orientation = new QComboBox(m_host);
    l_res = new QLabel(m_host);
    l_res->setText(tr("Resolution:"));
    m_resolution = new QComboBox(m_host);
    hl_resolution->addWidget(l_res);
    hl_resolution->addWidget(m_resolution);
    hl_resolution->setStretch(1, 1);
    m_resolution->addItems(g_resolutions);

    hl_resolution->addWidget(l_orientation);
    hl_resolution->addWidget(m_orientation);
    hl_resolution->setStretch(3, 1);
    m_orientation->addItems(QStringList() << "Standard" << "90*" << "180*" << "270*");
    m_orientation->setDisabled(true);

    auto hl_position = new QHBoxLayout();
    l_position = new QLabel(m_host);
    l_position_2 = new QLabel(m_host);
    m_position_buddy = new QComboBox(m_host);
    m_position_select = new QComboBox(m_host);
    l_position->setText(tr("Position this monitor"));
    l_position_2->setText(tr("of monitor:"));
    hl_position->addWidget(l_position);
    hl_position->addWidget(m_position_select);
    hl_position->addWidget(l_position_2);
    hl_position->addWidget(m_position_buddy);

    m_position_select->addItems(QStringList() << "above" << "to the left" << "to the right" << "below");
    hl_position->setStretch(1, 1);
    hl_position->setStretch(3, 1);

    m_layout->addWidget(m_sman);
    m_layout->setStretch(1, 1);
    m_layout->addLayout(hl_opt);
    m_layout->addWidget(line);
    m_layout->addLayout(hl_resolution);
    m_layout->addLayout(hl_position);

    disableAll();
    connect(m_sman, &ScreenManagerWidget::selectedScreenChanged, this, &DisplaysApplet::selectedScreenChanged);

    initialScreenProvision();
}

void DisplaysApplet::disableAll()
{
    m_mode_off->setChecked(false);
    m_mode_primary->setChecked(false);
    m_mode_extend->setChecked(false);
    m_mode_mirror->setChecked(false);

    m_mode_off->setDisabled(true);
    m_mode_primary->setDisabled(true);
    m_mode_extend->setDisabled(true);
    m_mode_mirror->setDisabled(true);

    m_mirrorselect->setDisabled(true);
    m_resolution->setDisabled(true);
    m_orientation->setDisabled(true);
    m_position_select->setDisabled(true);
    m_position_buddy->setDisabled(true);
}

void DisplaysApplet::initialScreenProvision()
{
    auto ps = qApp->primaryScreen();
    Screen *last = m_manager->createFirstScreen(ps);

    for(QScreen *s : qApp->screens())
    {
        if(s != ps)
        {
            // if we don't have a saved buddy put it to the end of the last screen
            last = m_manager->createExtensionScreen(s, last, Qt::RightEdge, 0);
        }
    }
}

void DisplaysApplet::selectedScreenChanged()
{
    disableAll();
    if(!m_sman->selectedScreen())
        return;

    auto screen = m_sman->selectedScreen();
    if(screen->mode() != Screen::Primary)
    {
        m_position_select->setEnabled(true);
        m_position_select->setEnabled(true);
        switch(m_manager->oppositeEdge(screen->buddyAnchor()))
        {
        case Qt::TopEdge:
            m_position_select->setCurrentIndex(0);
            break;
        case Qt::LeftEdge:
            m_position_select->setCurrentIndex(1);
            break;
        case Qt::RightEdge:
            m_position_select->setCurrentIndex(2);
            break;
        case Qt::BottomEdge:
            m_position_select->setCurrentIndex(3);
            break;
        }
    }
    switch(screen->mode())
    {
    case Screen::Primary:
        m_mode_primary->setChecked(true);
        break;
    case Screen::Extension:
        m_mode_extend->setChecked(true);
        break;
    case Screen::Mirror:
        m_mode_mirror->setChecked(true);
    }

    //m_mode_off->setDisabled(false);
    m_mode_primary->setDisabled(false);
    m_mode_extend->setDisabled(false);
    m_mode_mirror->setDisabled(false);
}

