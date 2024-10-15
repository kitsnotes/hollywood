#ifndef ABOUTWINDOW_H
#define ABOUTWINDOW_H

#include <QDialog>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <QtWidgets/QCommandLinkButton>

class MenuDialog : public QDialog
{
    Q_OBJECT

public:
    MenuDialog(QWidget *parent = nullptr);
    ~MenuDialog() = default;
protected:
    void paintEvent(QPaintEvent *event);
    void closeEvent(QCloseEvent *event);
private:
    void setupUi();
    void checkActions();
private:
    friend class HWUtilities;
    QPixmap *m_lightBg;
    QPixmap *m_darkBg;

    QVBoxLayout *vl_main;
    QHBoxLayout *hl_branding;
    QLabel *logo_icon;
    QLabel *hollywood_label;
    QLabel *hw_train;
    QVBoxLayout *fl_actions;

    QCommandLinkButton *m_rescue;
    QCommandLinkButton *m_browser;
    QCommandLinkButton *m_reinstall;
    QCommandLinkButton *m_terminull;
};
#endif // ABOUTWINDOW_H
