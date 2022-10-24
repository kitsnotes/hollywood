#ifndef WIFI_DIALOG_P_H
#define WIFI_DIALOG_P_H

#include "ui_wifi_dialog.h"

namespace HWCM
{
    class ConnmanPrivate;
    class Service;
    class WifiDialog : public QDialog
    {
        Q_OBJECT
    public:
        explicit WifiDialog(QWidget *parent);
        ~WifiDialog() = default;
        int showWithMap(QMap<QString,QString> input_map);
        void createDict(QMap<QString,QVariant>& r_map);
    private slots:
        void showPassphrase(bool);
        void useWPSPushButton(bool);
    private:
        void hideElements();
    private:
        Ui::WifiDialog ui;
        bool m_radius = false;
        bool m_wispr = false;
        bool m_hidden = false;
    };
}

#endif // WIFI_DIALOG_P_H
