#-------------------------------------------------
#
# Project created by QtCreator 2018-04-11T12:57:49
#
#-------------------------------------------------

QT += core gui multimedia svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Mu
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000 # disables all the APIs deprecated before Qt 6.0.0

windows{
    RC_ICONS = windows/Mu.ico
    QMAKE_CFLAGS += -openmp
    QMAKE_CXXFLAGS += -openmp
    DEFINES += "_Pragma=__pragma"
    DEFINES += EMU_MULTITHREADED
}

macx{
    QMAKE_CFLAGS += -std=c89 -D__STDBOOL_H -Dinline= -Dbool=char -Dtrue=1 -Dfalse=0 # tests C89 mode
    ICON = macos/Mu.icns
    QMAKE_INFO_PLIST = macos/Info.plist
    DEFINES += EMU_MULTITHREADED
}

linux-g++{
    QMAKE_CFLAGS += -fopenmp
    QMAKE_CXXFLAGS += -fopenmp
    QMAKE_LFLAGS += -fopenmp
    DEFINES += EMU_MULTITHREADED
}

android{
    QMAKE_CFLAGS += -fopenmp
    QMAKE_CXXFLAGS += -fopenmp
    QMAKE_LFLAGS += -fopenmp
    DEFINES += EMU_MULTITHREADED
}

ios{
    # QMAKE_INFO_PLIST = ios/Info.plist
    DEFINES += EMU_MULTITHREADED
}


CONFIG(debug, debug|release){
    # debug build, be accurate, fail hard, and add logging
    DEFINES += EMU_DEBUG EMU_CUSTOM_DEBUG_LOG_HANDLER EMU_SANDBOX
    DEFINES += EMU_SANDBOX_OPCODE_LEVEL_DEBUG # for breakpoints
    DEFINES += EMU_SANDBOX_LOG_APIS # for printing sysTrap* calls, EMU_SANDBOX_OPCODE_LEVEL_DEBUG must be on too
    macx|linux-g++{
        # also check for any buffer overflows and memory leaks
        # -fsanitize=undefined,leak
        QMAKE_CFLAGS += -fstack-protector-strong -fsanitize=address -Werror=array-bounds
        QMAKE_CXXFLAGS += -fstack-protector-strong -fsanitize=address -Werror=array-bounds
        QMAKE_LFLAGS += -fsanitize=address
    }
}else{
    # release build, go fast
    DEFINES += EMU_NO_SAFETY
}

CONFIG += c++11

INCLUDEPATH += $$PWD/qt-common/include

SOURCES += \
    debugviewer.cpp \
    emuwrapper.cpp \
    main.cpp \
    mainwindow.cpp \
    statemanager.cpp \
    touchscreen.cpp \
    ../../src/audio/blip_buf.c \
    ../../src/debug/sandbox.c \
    ../../src/ads7846.c \
    ../../src/emulator.c \
    ../../src/flx68000.c \
    ../../src/hardwareRegisters.c \
    ../../src/memoryAccess.c \
    ../../src/pdiUsbD12.c \
    ../../src/sdCard.c \
    ../../src/sed1376.c \
    ../../src/silkscreen.c \
    ../../src/m68k/m68kcpu.c \
    ../../src/m68k/m68kdasm.c \
    ../../src/m68k/m68kopac.c \
    ../../src/m68k/m68kopdm.c \
    ../../src/m68k/m68kopnz.c \
    ../../src/m68k/m68kops.c \
    ../../src/armv5/CPU.c \
    ../../src/armv5/icache.c \
    ../../src/armv5.c \
    ../../src/expansionHardware.c \
    settingsmanager.cpp

HEADERS += \
    debugviewer.h \
    emuwrapper.h \
    mainwindow.h \
    statemanager.h \
    touchscreen.h \
    ../../src/audio/blip_buf.h \
    ../../src/debug/sandbox.h \
    ../../src/debug/sandboxTrapNumToName.c.h \
    ../../src/debug/trapNames.h \
    ../../src/m68k/m68k.h \
    ../../src/m68k/m68kconf.h \
    ../../src/m68k/m68kcpu.h \
    ../../src/m68k/m68kexternal.h \
    ../../src/m68k/m68kops.h \
    ../../src/specs/dragonballVzRegisterSpec.h \
    ../../src/ads7846.h \
    ../../src/emulator.h \
    ../../src/flx68000.h \
    ../../src/hardwareRegisters.h \
    ../../src/hardwareRegistersAccessors.c.h \
    ../../src/hardwareRegistersTiming.c.h \
    ../../src/memoryAccess.h \
    ../../src/pdiUsbD12.h \
    ../../src/portability.h \
    ../../src/sdCard.h \
    ../../src/sed1376.h \
    ../../src/sed1376Accessors.c.h \
    ../../src/silkscreen.h \
    ../../src/specs/sed1376RegisterSpec.h \
    ../../src/specs/pdiUsbD12CommandSpec.h \
    ../../src/specs/emuFeatureRegisterSpec.h \
    ../../src/specs/sdCardCommandSpec.h \
    ../../src/armv5/CPU.h \
    ../../src/armv5/math64.h \
    ../../src/armv5/types.h \
    ../../src/armv5/icache.h \
    ../../src/armv5.h \
    ../../src/expansionHardware.h \
    ../../src/sdCardAccessors.c.h \
    ../../src/sdCardCrcTables.c.h \
    settingsmanager.h

FORMS += \
    mainwindow.ui \
    debugviewer.ui \
    statemanager.ui \
    settingsmanager.ui

CONFIG += mobility
MOBILITY = 

DISTFILES += \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat \
    android/res/drawable-hdpi/icon.png \
    android/res/drawable-ldpi/icon.png \
    android/res/drawable-mdpi/icon.png \
    images/addressBook.svg \
    images/calendar.svg \
    images/center.svg \
    images/debugger.svg \
    images/down.svg \
    images/install.svg \
    images/left.svg \
    images/notes.svg \
    images/pause.svg \
    images/play.svg \
    images/power.svg \
    images/right.svg \
    images/screenshot.svg \
    images/settingsManager.svg \
    images/stateManager.svg \
    images/stop.svg \
    images/todo.svg \
    images/up.svg

RESOURCES += \
    mainwindow.qrc

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
