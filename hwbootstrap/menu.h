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
private:
    void setupUi();
private:
    QPixmap *m_lightBg;
    QPixmap *m_darkBg;

    QVBoxLayout *vl_main;
    QHBoxLayout *hl_branding;
    QLabel *logo_icon;
    QLabel *hollywood_label;
    QLabel *hw_train;
    QLabel *lbl_copyright;
    QVBoxLayout *fl_actions;

    QCommandLinkButton *m_install_sxs;
    QCommandLinkButton *m_replace;
    QCommandLinkButton *m_vm;
};
#endif // ABOUTWINDOW_H
