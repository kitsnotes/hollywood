// Hollywood Shell Library
// (C) 2024 Originull Software
// SPDX-License-Identifier: LGPL-2.1

#include "progresswidget.h"
#include "fileoperation.h"

LSOpProgressWidget::LSOpProgressWidget(FileOperation *op, QWidget *parent)
    : QWidget(parent)
    , m_op(op)
    , vs_icon_top(new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding))
    , m_icon(new QLabel(this))
    , vs_icon_btm(new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding))
    , m_task(new QLabel(this))
    , m_progress(new QProgressBar(this))
    , m_cancelbtn(new QToolButton(this))
    , m_details(new QLabel(this))
    , m_error(new QLabel(this))
    , m_errorButtons(new QDialogButtonBox(this))
{
    setMinimumHeight(75);
    setMaximumHeight(75);
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

    m_progress->setValue(0);
    m_progress->setTextVisible(false);
    m_progress->setMaximumHeight(15);
    m_cancelbtn->setMaximumHeight(32);
    m_cancelbtn->setMaximumWidth(32);
    m_cancelbtn->setAutoRaise(true);
    m_cancelbtn->setIcon(QIcon::fromTheme("process-stop"));
    m_cancelbtn->setToolTip(tr("Cancel this operation"));

    vl_main->addWidget(m_task);
    vl_main->addWidget(m_progress);
    vl_main->addWidget(m_details);

    auto font = m_details->font();
    font.setPointSize(font.pointSize()-1);
    m_details->setFont(font);

    m_details->setText(tr("Starting..."));

    main_layout->addLayout(vl_icon);
    main_layout->addLayout(vl_main);
    main_layout->addWidget(m_cancelbtn);

    auto errorLayout = new QVBoxLayout;
    errorLayout->addWidget(m_error, 1);
    errorLayout->addWidget(m_errorButtons);

    m_error->setText(tr("Could not copy file."));
    m_errorButtons->setStandardButtons(QDialogButtonBox::Ok);
    m_error->setVisible(false);
    m_errorButtons->setVisible(false);
    m_errorButtons->setEnabled(false);

    main_layout->addLayout(errorLayout);
    connect(op, &FileOperation::dataTransferProgress,
        this, &LSOpProgressWidget::progressChanged);
    connect(op, &FileOperation::started,
            this, &LSOpProgressWidget::started);
    connect(op, &FileOperation::error,
                this, &LSOpProgressWidget::error);
    connect(op, &FileOperation::stateChanged,
                this, &LSOpProgressWidget::stateChanged);
}

void LSOpProgressWidget::setOperationTitle(const QString &title)
{
    m_task->setText(title);
}

void LSOpProgressWidget::setIcon(const QIcon &icon)
{
    m_icon->setPixmap(icon.pixmap(32,32));
}

void LSOpProgressWidget::error(int id, FileOperation::Error error, bool stopped)
{

}

void LSOpProgressWidget::stateChanged(FileOperation::State state)
{

}

void LSOpProgressWidget::started(int id)
{

}

void LSOpProgressWidget::finished(int id, bool error)
{

}

void LSOpProgressWidget::progressChanged(uint id, qint64 progress)
{
    Q_UNUSED(id)
    m_progress->setValue(progress);
}

void LSOpProgressWidget::statusChanged(const QString &status)
{
    m_details->setText(status);
}
