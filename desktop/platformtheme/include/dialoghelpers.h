#ifndef HWMESSAGEDIALOGHELPER_H
#define HWMESSAGEDIALOGHELPER_H

#include <qpa/qplatformdialoghelper.h>
#include <QObject>
#include <QSharedPointer>
#include <messagebox.h>

class HWMessageDialogHelper : public QPlatformMessageDialogHelper
{
public:
    HWMessageDialogHelper();
    ~HWMessageDialogHelper() override;

    void exec() override;
    bool show(Qt::WindowFlags windowFlags, Qt::WindowModality windowModality, QWindow *parent) override;
    void hide() override;

private slots:
    void hwDialogButtonClicked(QAbstractButton *btn);
private:
    HWMessageBox* m_dlg;
};

#endif // HWMESSAGEDIALOGHELPER_H
