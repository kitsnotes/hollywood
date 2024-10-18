TEMPLATE = subdirs

SUBDIRS += libhwlogind \
           libhwudev \
           eglfsxkb \
           libinput \
           platformheaders \
           platformsupport/edid \
           platformsupport/kmsconvenience \
           plugins/platforms/eglfs

libinput.depends = libhwudev
plugins/platforms/eglfs.depends = libhwudev libhwlogind libinput
