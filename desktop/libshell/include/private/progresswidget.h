#ifndef LSOPPROGRESSWIDGET_H
#define LSOPPROGRESSWIDGET_H

#include <QWidget>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <QtWidgets/QDialogButtonBox>

#include "fileoperation.h"

class FileOperation;
class LSOpProgressWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LSOpProgressWidget(FileOperation *op, QWidget *parent = nullptr);
    void setOperationTitle(const QString &title);
    void setIcon(const QIcon &icon);
public slots:
    void error(int id, FileOperation::Error error, bool stopped);
    void stateChanged(FileOperation::State state);
    void started(int id);
    void finished(int id, bool error);
    void progressChanged(uint id, qint64 progress);
    void statusChanged(const QString &status);
signals:
    void cancelRequested();
private:
    void presentError();
    void presentProgress();
private:
    FileOperation *m_op = nullptr;
    QSpacerItem *vs_icon_top = nullptr;
    QLabel *m_icon = nullptr;
    QSpacerItem *vs_icon_btm = nullptr;
    QLabel *m_task = nullptr;
    QProgressBar *m_progress = nullptr;
    QToolButton *m_cancelbtn = nullptr;
    QLabel *m_details = nullptr;

    QLabel *m_error = nullptr;
    QDialogButtonBox *m_errorButtons = nullptr;
};

#endif // LSOPPROGRESSWIDGET_H
