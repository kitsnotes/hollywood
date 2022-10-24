#ifndef CLIPBOARDWINDOW_H
#define CLIPBOARDWINDOW_H

#include "fileshell.h"

class FMApplication;
class ClipboardWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit ClipboardWindow(QWidget *parent = nullptr);
    ~ClipboardWindow();
private slots:
    void clipboardDataChanged();

private:
    QStatusBar *m_status;
};
#endif // CLIPBOARDWINDOW_H
