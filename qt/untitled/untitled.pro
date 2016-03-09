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
    ../../common/fs.c \
    ../../common/lab2.c

OTHER_FILES += \
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
    ../../../EXT2_source/include/include/linux/ext2_fs.h \
    ../../include/lab2.h

