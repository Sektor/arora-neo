TEMPLATE = app
TARGET = arora

CONFIG += qtopia
QT += webkit network

DEFINES += QT_NO_CAST_FROM_ASCII

#-----------------------------------------------

INCLUDEPATH += utils

RESOURCES += data/data.qrc \
    htmls/htmls.qrc

#-----------------------------------------------
SOURCES += main.cpp
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
