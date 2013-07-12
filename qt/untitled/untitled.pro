TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.c \
    ../../kernel.c \
    ../../common/tui.cpp \
    ../../common/string_ext.c \
    ../../common/shutdown.c \
    ../../common/screen.c \
    ../../common/scanf.c \
    ../../common/printf.c \
    ../../common/paging.c \
    ../../common/ordered_map.c \
    ../../common/malloc.c \
    ../../common/keyboard.c \
    ../../common/idt.c \
    ../../common/heap.c \
    ../../common/gdt.c \
    ../../common/assert.c \
    ../../common/acpi.c \
    ../../common/.tui.c \
    ../../common/timer.c \
    ../../common/fs.c

OTHER_FILES += \
    android/READ-THIS-BEFORE-MANUALLY-ADDING-FILES-TO-PACKAGE.txt \
    android/AndroidManifest.xml \
    android/version.xml \
    android/java.pro \
    android/src/org/kde/necessitas/ministro/IMinistro.aidl \
    android/src/org/kde/necessitas/ministro/IMinistroCallback.aidl \
    android/src/org/qtproject/qt5/android/bindings/QtActivity.java \
    android/src/org/qtproject/qt5/android/bindings/QtApplication.java \
    android/res/values-et/strings.xml \
    android/res/values-zh-rTW/strings.xml \
    android/res/values-ms/strings.xml \
    android/res/values-el/strings.xml \
    android/res/values-it/strings.xml \
    android/res/values-fa/strings.xml \
    android/res/values-ru/strings.xml \
    android/res/values/strings.xml \
    android/res/values/libs.xml \
    android/res/values-nl/strings.xml \
    android/res/values-zh-rCN/strings.xml \
    android/res/values-de/strings.xml \
    android/res/values-ro/strings.xml \
    android/res/values-es/strings.xml \
    android/res/values-pt-rBR/strings.xml \
    android/res/values-id/strings.xml \
    android/res/values-nb/strings.xml \
    android/res/layout/splash.xml \
    android/res/values-pl/strings.xml \
    android/res/values-rs/strings.xml \
    android/res/values-fr/strings.xml \
    android/res/values-ja/strings.xml \
    ../../boot.s \
    ../../common/string.s \
    ../../common/misc.s \
    ../../common/idt_s.s \
    ../../common/gdt_s.s \
    ../../makefile

INCLUDEPATH -= /usr/include
INCLUDEPATH += ../../include

HEADERS += \
    ../../include/types.h \
    ../../include/tui.hpp \
    ../../include/string.h \
    ../../include/stdlib.h \
    ../../include/stdarg.h \
    ../../include/shutdown.h \
    ../../include/screen.h \
    ../../include/scanf.h \
    ../../include/reboot.h \
    ../../include/printf.h \
    ../../include/pci.h \
    ../../include/paging.h \
    ../../include/ordered_array.h \
    ../../include/multiboot.h \
    ../../include/keyboard.h \
    ../../include/io.h \
    ../../include/idt.h \
    ../../include/heap.h \
    ../../include/gdt.h \
    ../../include/errno.h \
    ../../include/assert.h \
    ../../include/asm.h \
    ../../include/acpi.h \
    ../../include/timer.h \
    ../../include/fs.h \
    ../../../EXT2_source/include/include/linux/ext2_fs.h

