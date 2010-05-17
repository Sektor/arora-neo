TEMPLATE = app
TARGET = arora

CONFIG += qtopia
QT += webkit network

#DEFINES += QT_NO_CAST_FROM_ASCII
DEFINES += QT_NO_UITOOLS
DEFINES += GITVERSION=0
DEFINES += GITCHANGENUMBER=0
DEFINES += QT_VER_DEFINE=263424
#QT_VERSION

#-----------------------------------------------

pkg [
    name=arora
    desc="Cross Platform WebKit Browser"
    license=GPL
    version=0.4
    maintainer="Anton Olkhovik <ant007h@gmail.com>"
]

desktop [
    hint=desktop
    files=arora.desktop
    path=/apps/Applications
]

pics [
    hint=pics
    files=data/arora.svg
    path=/pics/arora
]

#-----------------------------------------------

INCLUDEPATH += utils \
    $$path(.,project)/src/utils/

RESOURCES += \
    data/data.qrc \
    htmls/htmls.qrc

#-----------------------------------------------
SOURCES += main.cpp
#-----------------------------------------------

FORMS += \
    aboutdialog.ui \
    addbookmarkdialog.ui \
    bookmarks.ui \
    cookies.ui \
    cookiesexceptions.ui \
    downloaditem.ui \
    downloads.ui \
    history.ui \
    passworddialog.ui \
    proxy.ui \
    searchbanner.ui \
    settings.ui

HEADERS += \
    aboutdialog.h \
    bookmarks.h \
    browserapplication.h \
    browsermainwindow.h \
    clearprivatedata.h \
    cookiejar.h \
    downloadmanager.h \
    edittableview.h \
    edittreeview.h \
    history.h \
    locationbar.h \
    locationbar_p.h \
    languagemanager.h \
    modelmenu.h \
    networkaccessmanager.h \
    plaintexteditsearch.h \
    searchbar.h \
    searchlineedit.h \
    settings.h \
    sourcehighlighter.h \
    sourceviewer.h \
    tabbar.h \
    tabwidget.h \
    toolbarsearch.h \
    webactionmapper.h \
    webview.h \
    webviewsearch.h \
    xbel.h

SOURCES += \
    aboutdialog.cpp \
    bookmarks.cpp \
    browserapplication.cpp \
    browsermainwindow.cpp \
    clearprivatedata.cpp \
    cookiejar.cpp \
    downloadmanager.cpp \
    edittableview.cpp \
    edittreeview.cpp \
    history.cpp \
    locationbar.cpp \
    languagemanager.cpp \
    modelmenu.cpp \
    networkaccessmanager.cpp \
    plaintexteditsearch.cpp \
    searchbar.cpp \
    searchlineedit.cpp \
    settings.cpp \
    sourcehighlighter.cpp \
    sourceviewer.cpp \
    tabbar.cpp \
    tabwidget.cpp \
    toolbarsearch.cpp \
    webactionmapper.cpp \
    webview.cpp \
    webviewsearch.cpp \
    xbel.cpp

#-----------------------------------------------

HEADERS += \
    utils/autosaver.h \
    utils/lineedit.h \
    utils/lineedit_p.h \
    utils/proxystyle.h \
    utils/singleapplication.h \
    utils/squeezelabel.h

SOURCES += \
    utils/autosaver.cpp \
    utils/lineedit.cpp \
    utils/singleapplication.cpp \
    utils/squeezelabel.cpp

#-----------------------------------------------

HEADERS += \
    utils/rotate.h

SOURCES += \
    utils/rotate.cpp

#-----------------------------------------------
