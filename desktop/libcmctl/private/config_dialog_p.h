#ifndef CONFIG_DIALOG_P_H
#define CONFIG_DIALOG_P_H

#include <QObject>

#include "ui_peditor.h"

namespace HWCM
{
    class ConnmanPrivate;
    class ConfigDialog;
    class Service;
    class ConfigDialogPrivate : public QObject
    {
        Q_OBJECT
    public:
        explicit ConfigDialogPrivate(Service *service, ConfigDialog *parent);
        ~ConfigDialogPrivate() = default;
        void readConfig();
    private slots:
        void saveChanges();
        void ip4ConfigSelectionChanged();
        void ip6ConfigSelectionChanged();
        void proxyConfigSelectionChanged();

    private:
        ConfigDialog *m_q = nullptr;
        Service *m_service = nullptr;
        Ui::Peditor ui;
    };
}

#endif // CONFIG_DIALOG_P_H
