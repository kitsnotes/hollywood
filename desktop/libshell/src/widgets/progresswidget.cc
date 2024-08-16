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
    m_progress->setMaximum(0);
    m_progress->setMinimum(0);
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

    presentProgress();
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
    qDebug() << "error" << id << error << stopped;
    if(stopped)
    {
        auto action = QString("copy");
        auto src_file = m_op->sourceFilePath(id).toLocalFile();
        auto dst_file = m_op->destinationFilePath(id).toLocalFile();
        QString errorString = QString("error");
        switch(error)
        {
        case FileOperation::SourceNotExists:
            errorString = tr("Could not %1 %2: the source file does not exist.").arg(action).arg(src_file);
            break;
        case FileOperation::DestinationExists:
            errorString = tr("The file %1 already exists in the target directory. What would you like to do?")
                              .arg(src_file);
            break;
        case FileOperation::SourceDirectoryOmitted:
            break;
        case FileOperation::SourceFileOmitted:
            break;
        case FileOperation::PathToDestinationNotExists:
            errorString = tr("Could not %1 %2: the destination path %1 does not exist.")
                              .arg(action).arg(src_file).arg(dst_file);
            break;
        case FileOperation::CannotCreateDestinationDirectory:
            errorString = tr("Could not %1 %2: the destination directory %1 could not be created.")
                              .arg(action).arg(src_file).arg(dst_file);
            break;
        case FileOperation::CannotOpenSourceFile:
            errorString = tr("Could not %1 %2: the source file can not be opened.")
                              .arg(action).arg(src_file);
            break;
        case FileOperation::CannotOpenDestinationFile:
        case FileOperation::CannotRemoveDestinationFile:
        case FileOperation::CannotCreateSymLink:
        case FileOperation::CannotReadSourceFile:
        case FileOperation::CannotWriteDestinationFile:
        case FileOperation::CannotRemoveSource:
        case FileOperation::Canceled:
        case FileOperation::NoError:
        default:
            errorString = tr("No error!");
        }

        m_error->setText(errorString);
        presentError();
    }
}

void LSOpProgressWidget::stateChanged(FileOperation::State state)
{
    qDebug() << "state changed" << state;
}

void LSOpProgressWidget::started(int id)
{
    qDebug() << "started" << id;
}

void LSOpProgressWidget::finished(int id, bool error)
{
    qDebug() << "finished" << id;

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

void LSOpProgressWidget::presentError()
{
    m_progress->setVisible(false);
    m_task->setVisible(false);
    m_details->setVisible(false);
    m_cancelbtn->setVisible(false);

    m_error->setVisible(true);
    m_errorButtons->setVisible(true);
}

void LSOpProgressWidget::presentProgress()
{
    m_progress->setVisible(true);
    m_task->setVisible(true);
    m_details->setVisible(true);
    m_cancelbtn->setVisible(true);

    m_error->setVisible(false);
    m_errorButtons->setVisible(false);
}
