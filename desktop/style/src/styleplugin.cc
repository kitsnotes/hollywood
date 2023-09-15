// Based on PhantomStyle
// License: LGPL 2.1

#include "styleplugin.h"
#include "style.h"

ArionStylePlugin::ArionStylePlugin(QObject *parent) : QStylePlugin() {}
ArionStylePlugin::~ArionStylePlugin() {}
QStyle* ArionStylePlugin::create(const QString& key) {
    if(key.toLower() == "hollywood")
        return new HollywoodStyle();

    return nullptr;
}

