#ifndef COMMONFUNCTIONS_H
#define COMMONFUNCTIONS_H

#include <QObject>

class LSCommonFunctions : public QObject
{
    Q_OBJECT
public:
    static LSCommonFunctions* instance();
    bool executeDesktopOverDBus(const QString &desktop);
    bool openFileOverDBusWithDefault(const QString &file);
    void showGetInfoDialog(const QUrl &target, QWidget *parent = nullptr);

};

#endif // COMMONFUNCTIONS_H
