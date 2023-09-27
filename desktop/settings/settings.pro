TEMPLATE = subdirs

greaterThan(QT_MAJOR_VERSION, 5) {
SUBDIRS = \
    applet-apk \
    applet-audio \
    applet-datetime \
    applet-defaults \
    applet-efiboot \
    applet-general \
    applet-displays \
    applet-network \
    applet-keyboard \
    applet-mouse \
    applet-wallpaper \
    applet-region \
    applet-stage \
    applet-energy \
    applet-users \
    applet-thunderbolt \
    applet-bluetooth \
    host
}
