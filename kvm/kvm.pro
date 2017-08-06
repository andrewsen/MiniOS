CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
TEMPLATE = lib
TARGET = uvm

QMAKE_CXXFLAGS += -std=c++11 -fPIC -Wall -pedantic -Wall -Wextra -Wcast-align \
    -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self \
    -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept \
    -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion \
    -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default -Wundef -Wunused
#-Wall -Wextra -pedantic -Wcast-align \
#    -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 \
#    -Winit-self -Wlogical-op -Wmissing-include-dirs -Wnoexcept \
#    -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-promo \
#    -Wstrict-null-sentinel -Wstrict-overflow=5 -Wundef -Wno-unused \
#    -Wno-variadic-macros -Wno-parentheses -fdiagnostics-show-option \
#    -Wconversion -Wdouble-promotion -Wmissing-braces \
#    -Wswitch-default -Wuninitialized \
#    -Wfloat-equal -Wcast-align

SOURCES += \
    runtime.cpp \
    module.cpp \
    log.cpp \
    internals.cpp \
    memorymanager.cpp \
    jit.cpp \
    jit_runtime_api.cpp

INCLUDEPATH += ../include
INCLUDEPATH += ../include/c++

HEADERS += \
    runtime.h \
    module.h \
    log.h \
    memorymanager.h \
    jit.h \
    common.h \
    defines.h \
    opcodes.h

DISTFILES += \
    jit_helpers.asm

LIBS += \
    $${PWD}/jit_helpers.o \
    -ldl
