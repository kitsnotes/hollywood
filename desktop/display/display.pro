TEMPLATE = subdirs
SUBDIRS = \
    driver \
    compositor \
    proxy
    compositor.depends = driver

