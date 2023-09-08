#ifndef PREFERENCEDIALOG_H
#define PREFERENCEDIALOG_H

#include "libcommdlg_global.h"

#include <QDialog>

class HWPreferenceDialogPrivate;
class LIBCOMMDLG_EXPORT HWPreferenceDialog : public QDialog
{
    Q_OBJECT
    Q_PROPERTY(bool animated READ animated WRITE setAnimated NOTIFY animatedChanged)
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
public:
    HWPreferenceDialog(QWidget *parent = nullptr);
    ~HWPreferenceDialog();
    void addPage(QWidget *page, const QIcon &icon, const QString &title);
    bool animated();
    int currentIndex();
public slots:
    void setAnimated(bool animated);
    void setCurrentIndex(int index);
    void show();
signals:
    void animatedChanged();
    void currentIndexChanged(int index);
private slots:
    void tabActionTriggered();
private:
    HWPreferenceDialogPrivate *p;
};

#endif // PREFERENCEDIALOG_H
