include(../include/global.pri)

TARGET = startext
TEMPLATE = app
QT       += core gui widgets printsupport
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

isEmpty(PREFIX): PREFIX=/usr

DEFINES += BUILD_HOLLYWOOD
INCLUDEPATH += ../libcommdlg
INCLUDEPATH += ../include

DISTFILES += \
    html-symbols.db

INCLUDEPATH += "./include"

SOURCES += \
    src/editorwidget.cc \
    src/editorwindow.cc \
    src/document.cc \
    src/tabwidget.cc \
    src/application.cc \
    src/colordialog.cc \
    src/preferencesdialog.cc \
    src/features/cpp/csyntaxhighlighter.cc \
    src/features/html/htmlsyntaxhighlighter.cc \
    src/finddialog.cc \
    src/spellcheckdialog.cc \
    src/spellcheck.cc \
    src/features/cpp/cfeatures.cc \
    src/features/html/htmlfeatures.cc \
    src/features/plain/plainfeatures.cc \
    src/features/textblockdata.cc

HEADERS += \
    include/editor.h \
    include/editorwidget.h \
    include/editorwindow.h \
    include/document.h \
    include/tabwidget.h \
    include/application.h \
    include/colordialog.h \
    include/preferencesdialog.h \
    include/finddialog.h \
    include/rcdefs.h \
    include/spellcheckdialog.h \
    include/spellcheck.h \
    include/features/abstractfeatures.h \
    include/features/cpp/csyntaxhighlighter.h \
    include/features/html/htmlsyntaxhighlighter.h \
    include/features/html/htmlfeatures.h \
    include/features/cpp/cfeatures.h \
    include/features/plain/plainfeatures.h \
    include/features/textblockdata.h

RESOURCES += \
    resource/basic.qrc


macx {
    DESTDIR = ../build/macos/
    LIBS += -L$${BUILD_DIRECTORY} -lhunspell
    ENG_US_DICT.files = dictionaries/US-English/index.aff \
                        dictionaries/US-English/index.dic

    ENG_US_DICT.path = "Contents/Resources/Dictionaries/US English"
    QMAKE_BUNDLE_DATA += ENG_US_DICT
    CONFIG += thinkdifferent
    QMAKE_INFO_PLIST = resource/mac/Info.plist
    ICON = resource/mac/ArionEditor.icns
    RESOURCES +=     resource/icons8-office.qrc
}

win32 {
    QT += winextras
    greaterThan(5, QT_MAJOR_VERSION) {
        greaterThan(2, QT_MINOR_VERSION) {
            DEFINES += WINDOWS_USE_JUMPLIST
        }
    }
    #DEFINES += NATIVE_WINDOWS_MENU
    LIBS += -lgdi32 -luser32 -lversion -lcomdlg32 -ldwmapi
    LIBS += $${BUILD_DIRECTORY}/hunspell.lib
    RC_FILE = resource/windows/editor.rc
    msvc:CONFIG -= embed_manifest_exe
    RESOURCES += resource/icons8-office.qrc
    DESTDIR = ../build/windows/
    eng_us_dict.path = $${BUILD_DIRECTORY}/Spelling/US-English
    eng_us_dict.files = dictionaries/US-English/index.aff \
                     dictionaries/US-English/index.dic

    eng_ca_dict.path = $${BUILD_DIRECTORY}/Spelling/Canadian-English
    eng_ca_dict.files = dictionaries/Canadian-English/index.aff \
                     dictionaries/Canadian-English/index.dic


    editor_files.path = $${BUILD_DIRECTORY}
    editor_files.files += resource/windows/installer.wxs
    INSTALLS += editor_files eng_us_dict
}

unix {
    LIBS += -lhunspell-1.7
    versionAtLeast(QT_VERSION, 6.0.0) {
        LIBS += -L../output -lcommdlg-$${HOLLYWOOD_APIVERSION}
    } else {
        LIBS += -L../output -lcommdlg5-$${HOLLYWOOD_APIVERSION}
    }
    RESOURCES += resource/default-icons.qrc

    QMAKE_SUBSTITUTES += org.originull.startext.desktop.in
    desktop.path = $$PREFIX/share/applications
    desktop.files = org.originull.startext.desktop
    target.path = $$PREFIX/bin

    INSTALLS += target desktop
}
DISTFILES += \
    org.originull.startext.desktop.in
