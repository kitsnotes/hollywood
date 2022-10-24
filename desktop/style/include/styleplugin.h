#ifndef PHANTOMSTYLEPLUGIN_H
#define PHANTOMSTYLEPLUGIN_H
#include <QtWidgets/qstyleplugin.h>

class ArionStylePlugin : public QStylePlugin {
  Q_OBJECT
  Q_PLUGIN_METADATA(IID QStyleFactoryInterface_iid FILE
                    "styleplugin.json")
public:
  ArionStylePlugin(QObject *parent = nullptr);
  ~ArionStylePlugin();
  QStyle* create(const QString& key) override;
};
#endif
