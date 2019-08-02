TEMPLATE = app
CONFIG -= console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        chip_8.c \
        control.c \
        graphic.c \
        main.c

LIBS += -L$$PWD/../../SDL2/lib/x64/ -lSDL2main \
        -L$$PWD/../../SDL2/lib/x64/ -lSDL2 \
        -L$$PWD/../../SDL2/lib/x64/ -lSDL2test

INCLUDEPATH += $$PWD/../../SDL2/include
DEPENDPATH += $$PWD/../../SDL2/include

HEADERS += \
    chip_8.h \
    control.h \
    graphic.h


