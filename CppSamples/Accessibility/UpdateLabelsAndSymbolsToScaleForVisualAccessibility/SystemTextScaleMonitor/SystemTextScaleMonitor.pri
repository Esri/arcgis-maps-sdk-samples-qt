HEADERS += $$PWD/SystemTextScaleMonitor.h
SOURCES += $$PWD/SystemTextScaleMonitor.cpp

win32 {
    SOURCES += $$PWD/SystemTextScaleMonitor_windows.cpp
    LIBS += -ladvapi32
}

ios {
    SOURCES += $$PWD/SystemTextScaleMonitor_ios.mm
    LIBS += -framework UIKit -framework Foundation
}

unix:!macx:!android:!ios {
    SOURCES += $$PWD/SystemTextScaleMonitor_linux.cpp
}

android {
    SOURCES += $$PWD/SystemTextScaleMonitor_android.cpp
    textScaleJavaSource = $$PWD/android/src/com/esri/samples/accessibility/SystemTextScaleMonitor.java
    textScaleJavaDirectory = $$OUT_PWD/android-build/src/com/esri/samples/accessibility
    SampleTextScaleJava.target = $$textScaleJavaDirectory/SystemTextScaleMonitor.java
    SampleTextScaleJava.depends = $$textScaleJavaSource
    SampleTextScaleJava.commands = \
        $$sprintf($$QMAKE_MKDIR_CMD, $$shell_quote($$shell_path($$textScaleJavaDirectory))) $$escape_expand(\n\t) \
        $$QMAKE_COPY $$shell_quote($$shell_path($$textScaleJavaSource)) $$shell_quote($$shell_path($$textScaleJavaDirectory))
    QMAKE_EXTRA_TARGETS += SampleTextScaleJava
    ALL_DEPS += $$SampleTextScaleJava.target
    target.depends += SampleTextScaleJava
    OTHER_FILES += $$textScaleJavaSource
}
