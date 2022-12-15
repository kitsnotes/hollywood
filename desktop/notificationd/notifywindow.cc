#include "notifywindow.h"
#include "notification.h"
#include <QPainterPath>

NotificationWindow::NotificationWindow(Notification *parent)
    : QWidget(0)
    , m_parent(parent)
    , m_layout(new QHBoxLayout(this))
    , m_icon(new QLabel(this))
    , m_title(new QLabel(this))
    , m_details(new QLabel(this))
    , m_close(new QToolButton(this))
{
    setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
    setMinimumWidth(100);
    setMaximumWidth(550);
    m_layout->setSpacing(0);
    m_layout->setContentsMargins(2,8,0,2);
    QFont font = m_title->font();
    font.setBold(true);
    font.setPointSize(font.pointSize()+1);
    m_title->setFont(font);

    m_close->setIcon(QIcon::fromTheme("window-close"));

    m_icon->setAlignment(Qt::AlignTop|Qt::AlignHCenter);
    m_title->setAlignment(Qt::AlignTop|Qt::AlignLeft);
    m_details->setAlignment(Qt::AlignTop|Qt::AlignLeft);
    m_details->setMargin(15);
    m_details->setOpenExternalLinks(true);
    auto iconLayout = new QVBoxLayout;
    iconLayout->addWidget(m_icon);
    iconLayout->addSpacerItem(new QSpacerItem(1,1,QSizePolicy::MinimumExpanding,QSizePolicy::Preferred));

    auto textlayout = new QVBoxLayout;
    textlayout->addWidget(m_title);
    textlayout->addWidget(m_details);
    textlayout->addSpacerItem(new QSpacerItem(1,1,QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding));

    auto actionLayout = new QVBoxLayout;
    actionLayout->addWidget(m_close);
    actionLayout->addSpacerItem(new QSpacerItem(1,1,QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding));

    m_layout->addLayout(iconLayout);
    m_layout->addLayout(textlayout);
    m_layout->addLayout(actionLayout);

    m_title->setText(m_parent->m_summary);
    m_details->setText(m_parent->m_body);
    if(!m_parent->m_app_icon.isEmpty())
    {
        QIcon ico = QIcon::fromTheme(m_parent->m_app_icon);
        if(!ico.isNull())
        {
            m_icon->setPixmap(ico.pixmap(64,64));
        }
        else
        {
            // see if we can do anything with app_id
        }
    }
    m_details->setTextFormat(Qt::RichText);

}

NotificationWindow::~NotificationWindow()
{
}

