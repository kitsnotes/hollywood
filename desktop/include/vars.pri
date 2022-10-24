## According to https://semver.org/, main page at the top, choosing
## the x.y.z in a semver version you must increment the:
##
##     o. MAJOR version when you make incompatible API changes,
##     o. MINOR version when you add functionality in a backwards-compatible manner, and
##     o. PATCH version when you make backwards-compatible bug fixes.

## According to https://autotools.io/libtool/version.html, section 4.1
## Setting the proper Shared Object Version we need to :
##
##     o. Increase the current value whenever an interface has been
##        added, removed or changed.
##     o. Always increase the revision value.
##     o. Increase the age value only if the changes made to the ABI
##        are backward compatible.
##
## For simplicity I am for now going to use the three numbers 3, 0 and 1
## for libtool's current, revision and age (this will end up as .so.2.1.0).
##
## The point of current, revision and age are that they form your ABI
## version (the so called VERSION). The point of major, minor (and patch)
## of semver is that it forms your API version (the so called APIVERSION).

HOLLYWOOD_MAJOR_VERSION =   1
HOLLYWOOD_MINOR_VERSION =   0
HOLLYWOOD_PATCH_VERSION =   0

HOLLYWOOD_CURRENT_VERSION = 1
HOLLYWOOD_REVISION_VERSION = 0
HOLLYWOOD_AGE_VERSION = 0

HOLLYWOOD_SOVERSION = $$system("echo $(($$HOLLYWOOD_CURRENT_VERSION - $$HOLLYWOOD_AGE_VERSION))")
HOLLYWOOD_VERSION = $${HOLLYWOOD_SOVERSION}"."$${HOLLYWOOD_AGE_VERSION}"."$${HOLLYWOOD_REVISION_VERSION}
HOLLYWOOD_APIVERSION = $${HOLLYWOOD_MAJOR_VERSION}"."$${HOLLYWOOD_MINOR_VERSION}

DISTFILES += \
    $$PWD/global-applet.pri
