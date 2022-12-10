#ifndef GETINFODIALOG_H
#define GETINFODIALOG_H

#include <hollywood/preferencedialog.h>
#include "libshell_int.h"

class LSGetInfoDialogPrivate;
class LIBSHELL_EXPORT LSGetInfoDialog : public HWPreferenceDialog
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
