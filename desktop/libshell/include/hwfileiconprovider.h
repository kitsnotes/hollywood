#ifndef HWFILEICONPROVIDER_H
#define HWFILEICONPROVIDER_H

#include <QFileIconProvider>
#include <QObject>

class HWFileIconProvider : public QFileIconProvider
{
public:
    HWFileIconProvider();
    ~HWFileIconProvider() = default;
    QIcon icon(QFileIconProvider::IconType type) const override;
    QIcon icon(const QFileInfo &info) const override;
};

#endif // HWFILEICONPROVIDER_H
