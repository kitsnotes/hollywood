#pragma once

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
