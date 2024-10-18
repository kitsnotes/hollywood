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
eglfs.depends = libhwudev libhwlogind libinput
