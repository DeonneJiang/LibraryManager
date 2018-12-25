TEMPLATE = app
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

SOURCES += main.cpp \
    Core/bookman.cpp \
    Core/borrowingrecordman.cpp \
    Core/common.cpp \
    Core/readerman.cpp \
    Core/timemac.cpp \
    GUI/booklistwindow.cpp \
    GUI/homewindow.cpp \
    GUI/loginwindow.cpp \
    GUI/mainwindow.cpp \
    Core/reader.cpp \
    GUI/bookswindow.cpp \
    GUI/recordswindow.cpp \
    GUI/readerswindow.cpp

HEADERS += \
    Core/bookman.h \
    Core/borrowingrecordman.h \
    Core/common.h \
    Core/readerman.h \
    Core/timemac.h \
    GUI/booklistwindow.h \
    GUI/homewindow.h \
    GUI/loginwindow.h \
    GUI/mainwindow.h \
    Core/reader.h \
    GUI/bookswindow.h \
    GUI/recordswindow.h \
    GUI/readerswindow.h

FORMS += \
    GUI/booklistwindow.ui \
    GUI/homewindow.ui \
    GUI/loginwindow.ui \
    GUI/mainwindow.ui \
    GUI/bookswindow.ui \
    GUI/recordswindow.ui \
    GUI/readerswindow.ui

RESOURCES += \
    GUI/resource.qrc
