#-------------------------------------------------
#
# Project created by QtCreator 2017-05-05T18:42:20
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = im2model_gui
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES +=\
        configwin.cpp \
    im2model_gui.cpp \
    treeitem.cpp \
    treemodel.cpp \
    cv_image_delegate.cpp \
    gui_tdmap_cell.cpp \
    gui_tdmap_table.cpp \
    td_map.cpp \
    image_crystal.cpp \
    celslc_prm.cpp \
    msa_prm.cpp \
    wavimg_prm.cpp \
    simgrid_steplength.cpp \
    settings.cpp \
    treeitem_file_delegate.cpp \
    qt_file_push_button.cpp \
    cv_tdmap_cell_image_frame.cpp \
    cv_tdmap_cell_image_frame_delegate.cpp


HEADERS  += configwin.h \
    treeitem.h \
    treemodel.h \
    cv_image_widget.h \
    cv_image_frame.h \
    cv_image_delegate.h \
    gui_tdmap_cell.h \
    gui_tdmap_table.h \
    td_map.hpp \
    image_crystal.hpp \
    celslc_prm.hpp \
    msa_prm.hpp \
    wavimg_prm.hpp \
    simgrid_steplength.hpp \
    global_def.hpp \
    settings.h \
    treeitem_file_delegate.hpp \
    qt_file_push_button.h \
    cv_tdmap_cell_image_frame.h \
    cv_tdmap_cell_image_frame_delegate.h \
    cv_image_cell_widget.h

RESOURCES     = ../im2model.qrc

FORMS    += configwin.ui \
    settings.ui

INCLUDEPATH += /usr/local/Cellar/boost/1.64.0_1/
