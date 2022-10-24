#ifndef VIEWOPTIONSDIALOG_H
#define VIEWOPTIONSDIALOG_H

#include "libshell_int.h"

class IconViewOptions;
class LIBSHELL_EXPORT ViewOptionsDialog : public QDialog
{
public:
    ViewOptionsDialog(QWidget *parent = nullptr);
    void setCurrentView(ArionShell::ViewMode view);
    void attachIconView(QListView *view);
protected:
    void closeEvent(QCloseEvent *event);
public slots:
    void show();
private:
    void refreshViewOptions();
private:
    ArionShell::ViewMode m_viewMode;
    IconViewOptions *m_icons;
};

#endif // VIEWOPTIONSDIALOG_H
