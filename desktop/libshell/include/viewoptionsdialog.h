#ifndef VIEWOPTIONSDIALOG_H
#define VIEWOPTIONSDIALOG_H

#include "libshell_int.h"

class LSIconViewOptions;
class LSViewOptionsDialogPrivate;
class LIBSHELL_EXPORT LSViewOptionsDialog : public QDialog
{
public:
    LSViewOptionsDialog(QWidget *parent = nullptr);
    void setCurrentView(HWShell::ViewMode view);
    void attachIconView(QListView *view);
protected:
    void closeEvent(QCloseEvent *event);
public slots:
    void show();
private:
    void refreshViewOptions();
private:
    LSViewOptionsDialogPrivate *p;
};

#endif // VIEWOPTIONSDIALOG_H
