QT       += core gui widgets bluetooth
QT       += network

CONFIG += c++11

#DEFINES += QT_DEPRECATED_WARNINGS
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Calendrier.cpp \
    CommunicationGroom.cpp \
    Evenement.cpp \
    Occupant.cpp \
    main.cpp \
    IHMGroom.cpp

HEADERS += \
    Calendrier.h \
    CommunicationGroom.h \
    Evenement.h \
    IHMGroom.h \
    Occupant.h

FORMS += \
    IHMGroom.ui

CONFIG(release, debug|release):DEFINES+=QT_NO_DEBUG_OUTPUT

RESOURCES += \
    icones.qrc

COPIES += parametres
          parametres.files = parametres.ini
          parametres.path  = $$OUT_PWD/
          parametres.base  = $$PWD/
