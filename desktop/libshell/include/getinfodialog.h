#ifndef GETINFODIALOG_H
#define GETINFODIALOG_H

#include <hollywood/preferencedialog.h>

class LSGetInfoDialogPrivate;
class LSGetInfoDialog : public HWPreferenceDialog
{
    Q_OBJECT
public:
    LSGetInfoDialog(const QUrl &file, QWidget *parent = nullptr);
    void reloadInfo();
public slots:
    void open();
private:
    LSGetInfoDialogPrivate *p;
};

#endif // GETINFODIALOG_H
