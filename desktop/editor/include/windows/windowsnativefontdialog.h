#ifndef WINDOWSNATIVEFONTDIALOG_H
#define WINDOWSNATIVEFONTDIALOG_H

#include "luvedit.h"

#include <QFontDialog>

class Win32FontDialog : public QObject
{
    Q_OBJECT
public:
    explicit Win32FontDialog(QWidget *parent = nullptr);
    ~Win32FontDialog();
    //void setCurrentFont(QFont &font);
    //QFont currentFont();
    //void setOption(FontDialogOption option, bool on = true);
    //void setOption(FontDialogOption options);
    //FontDialogOptions options();
signals:
    void accepted();
    void rejected();
public slots:
    int exec();
private:
    CHOOSEFONT *m_cf;
    LOGFONT *m_lf;
};

#endif // WINDOWSNATIVEFONTDIALOG_H
