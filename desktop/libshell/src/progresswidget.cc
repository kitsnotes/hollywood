#include "progresswidget.h"

LSOpProgressWidget::LSOpProgressWidget(QWidget *parent)
    : QWidget(parent),
      vs_icon_top(new QSpacerItem(20, 2, QSizePolicy::Minimum, QSizePolicy::Expanding)),
      m_icon(new QLabel(this)),
      vs_icon_btm(new QSpacerItem(20, 2, QSizePolicy::Minimum, QSizePolicy::Expanding)),
      m_task(new QLabel(this)),
      m_progress(new QProgressBar(this)),
      m_cancelbtn(new QToolButton(this)),
      m_details(new QLabel(this))
{
    m_task->setObjectName(QString::fromUtf8("TaskLabel"));
    m_progress->setObjectName(QString::fromUtf8("ProgressBar"));
    m_cancelbtn->setObjectName(QString::fromUtf8("CancelButton"));
    m_details->setObjectName(QString::fromUtf8("DetailsLabel"));

    auto main_layout = new QHBoxLayout(this);
    auto vl_main =  new QVBoxLayout();
    auto vl_icon = new QVBoxLayout();

    vl_icon->addItem(vs_icon_top);
    vl_icon->addWidget(m_icon);
    vl_icon->addItem(vs_icon_btm);

    m_progress->setValue(24);
    m_progress->setTextVisible(false);
    m_cancelbtn->setAutoRaise(true);

    auto progress_layout = new QHBoxLayout();
    progress_layout->addWidget(m_progress);
    progress_layout->addWidget(m_cancelbtn);

    vl_main->addWidget(m_task);
    vl_main->addLayout(progress_layout);
    vl_main->addWidget(m_details);

    main_layout->addLayout(vl_icon);
    main_layout->addLayout(vl_main);
}
