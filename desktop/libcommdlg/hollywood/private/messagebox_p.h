#ifndef MESSAGEBOX_P_H
#define MESSAGEBOX_P_H

#include <QString>
#include <QIcon>
#include <QObject>
#include <QMessageBox>
#include <QAnimationGroup>
#include <QPropertyAnimation>
#include <QSoundEffect>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QVBoxLayout>

class HWMessageBox;
class MessageBoxPrivate
{
private:
    void generateStandardIcon();
    void playBell();
    void animateOpenDetails();
    void animateCloseDetails();
private:
    friend class HWMessageBox;
    MessageBoxPrivate(HWMessageBox *parent);
    HWMessageBox *d;

    QVBoxLayout *verticalLayout;
    QLabel *m_icon;
    QLabel *m_text;
    QLabel *m_informative_text;
    QPushButton *m_help;
    QPushButton *m_expand;
    QDialogButtonBox *buttonBox;
    QTextBrowser *m_description;

    QMessageBox::Icon stdIcon;
    QPixmap icon;
    QSoundEffect *m_bell = nullptr;

    QCheckBox *m_cb = nullptr;

    Qt::TextFormat m_format;

    QPushButton *m_default = nullptr;
    QAbstractButton *m_escape = nullptr;
    QAbstractButton *m_clicked = nullptr;
};

#endif // MESSAGEBOX_P_H
