#ifndef HWMESSAGEDIALOGHELPER_H
#define HWMESSAGEDIALOGHELPER_H

#include <qpa/qplatformdialoghelper.h>
#include <QObject>
#include <QSharedPointer>
#include <hollywood/messagebox.h>

class HWMessageDialogHelper : public QPlatformMessageDialogHelper
{
public:
    HWMessageDialogHelper();
    ~HWMessageDialogHelper() override;

    void exec() override;
    bool show(Qt::WindowFlags windowFlags, Qt::WindowModality windowModality, QWindow *parent) override;
    void hide() override;
private:
    void provision(QMessageBox::Icon icon, const QString &title, const QString &text,
                   QMessageBox::StandardButtons buttons = QMessageBox::NoButton);

private slots:
    void hwDialogButtonClicked(QAbstractButton *btn);
private:
    HWMessageBox* m_dlg = nullptr;
};

#endif // HWMESSAGEDIALOGHELPER_H
