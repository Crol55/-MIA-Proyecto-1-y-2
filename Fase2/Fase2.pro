TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++0x -pthread -lcurl
LIBS += -pthread -lcurl

SOURCES += \
        main.cpp

HEADERS += \
    analizador_lexico.h \
    estructuras.h \
    exec.h \
    fdisk.h \
    interprete.h \
    lista_enlazada.h \
    mkdisk.h \
    mount.h \
    raid1.h \
    rep.h \
    rmdisk.h \
    unmount.h \
    manejo_particiones/mkfs.h \
    manejo_particiones/getterysetter_globales.h \
    manejo_particiones/op_inodos.h \
    manejo_particiones/op_bloques.h \
    manejo_particiones/admin_usuario.h \
    manejo_particiones/mkfile.h \
    manejo_particiones/mkdir.h \
    manejo_particiones/rep_.h \
    manejo_particiones/journaling.h \
    manejo_particiones/loss.h \
    manejo_particiones/recovery.h \
    manejo_particiones/cat.h \
    syncronice.h
