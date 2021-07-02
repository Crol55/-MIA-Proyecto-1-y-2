TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.cpp

HEADERS += \
    interprete.h \
    analizador_lexico.h \
    mkdisk.h \
    estructuras.h \
    rmdisk.h \
    fdisk.h \
    mount.h \
    lista_enlazada.h \
    rep.h \
    exec.h \
    unmount.h \
    raid1.h
