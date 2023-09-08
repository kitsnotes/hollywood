#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H

#include "libcommdlg_global.h"
#include <QDialog>
#include <QMessageBox>

class MessageBoxPrivate;
class LIBCOMMDLG_EXPORT HWMessageBox : public QDialog
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText)
    Q_PROPERTY(QMessageBox::Icon icon READ icon WRITE setIcon)
    Q_PROPERTY(QPixmap iconPixmap READ iconPixmap WRITE setIconPixmap)
    Q_PROPERTY(Qt::TextFormat textFormat READ textFormat WRITE setTextFormat)
    Q_PROPERTY(QMessageBox::StandardButtons standardButtons READ standardButtons WRITE setStandardButtons)
    Q_PROPERTY(QString detailedText READ detailedText WRITE setDetailedText)
    Q_PROPERTY(QString informativeText READ informativeText WRITE setInformativeText)
    Q_PROPERTY(Qt::TextInteractionFlags textInteractionFlags READ textInteractionFlags
               WRITE setTextInteractionFlags)
public:
    HWMessageBox(QMessageBox::Icon icon, const QString &title, const QString &text,
                QMessageBox::StandardButtons buttons = QMessageBox::NoButton, QWidget *parent = nullptr,
                Qt::WindowFlags flags = Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    ~HWMessageBox();

    void addButton(QAbstractButton *button, QMessageBox::ButtonRole role);
    QPushButton *addButton(const QString &text, QMessageBox::ButtonRole role);
    QPushButton *addButton(QMessageBox::StandardButton button);
    void removeButton(QAbstractButton *button);

    using QDialog::open;
    void open(QObject *receiver, const char *member);

    QList<QAbstractButton *> buttons() const;
    QMessageBox::ButtonRole buttonRole(QAbstractButton *button) const;

    void setStandardButtons(QMessageBox::StandardButtons buttons);
    QMessageBox::StandardButtons standardButtons() const;
    QMessageBox::StandardButton standardButton(QAbstractButton *button) const;
    QAbstractButton *button(QMessageBox::StandardButton which) const;

    QPushButton *defaultButton() const;
    void setDefaultButton(QPushButton *button);
    void setDefaultButton(QMessageBox::StandardButton button);

    QAbstractButton *escapeButton() const;
    void setEscapeButton(QAbstractButton *button);
    void setEscapeButton(QMessageBox::StandardButton button);

    QAbstractButton *clickedButton() const;

    QString text() const;
    void setText(const QString &text);

    QString detailedText() const;
    void setDetailedText(const QString &text);

    QString informativeText() const;
    void setInformativeText(const QString &text);

    QMessageBox::Icon icon() const;
    void setIcon(QMessageBox::Icon);

    QPixmap iconPixmap() const;
    void setIconPixmap(const QPixmap &pixmap);

    Qt::TextFormat textFormat() const;
    void setTextFormat(Qt::TextFormat format);

    void setTextInteractionFlags(Qt::TextInteractionFlags flags);
    Qt::TextInteractionFlags textInteractionFlags() const;

    void setCheckBox(QCheckBox *cb);
    QCheckBox* checkBox() const;
public slots:
    int exec() override;
signals:
    void buttonClicked(QAbstractButton *button);
protected:
    void showEvent(QShowEvent *event);
private slots:
    friend class MessageBoxPrivate;
    void boxButtonClicked(QAbstractButton *button);
    void helpButtonClicked();
    void showDetailedText();
private:
    MessageBoxPrivate *q;
};
#endif // MESSAGEBOX_H
