QT += dbus \
    svg \
    multimedia \
    widgets

CONFIG += c++17 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        dbusconnector.cpp \
        dbuswatcher.cpp \
        main.cpp \
        mainapplication.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    dbusconnector.h \
    dbuswatcher.h \
    mainapplication.h

DISTFILES += \
    "audio/battery 10%.aac" \
    "audio/battery 15%.aac" \
    "audio/battery 20%.aac" \
    "audio/battery 25%.aac" \
    "audio/battery 30%.aac" \
    "audio/battery 35%.aac" \
    "audio/battery 40%.aac" \
    "audio/battery 45%.aac" \
    "audio/battery 5%.aac" \
    "audio/battery 50%.aac" \
    "audio/battery 55%.aac" \
    "audio/battery 60%.aac" \
    "audio/battery 65%.aac" \
    "audio/battery 70%.aac" \
    "audio/battery 75%.aac" \
    "audio/battery 80%.aac" \
    "audio/battery 85%.aac" \
    "audio/battery 90%.aac" \
    "audio/battery 95%.aac" \
    "audio/battery fully charged.aac" \
    "audio/battery level critical.aac" \
    audio/charging.aac \
    "audio/device not found.aac" \
    images/battery-000.svg \
    images/battery-010.svg \
    images/battery-020-charging.svg \
    images/battery-020.svg \
    images/battery-030.svg \
    images/battery-040-charging.svg \
    images/battery-040.svg \
    images/battery-050.svg \
    images/battery-060-charging.svg \
    images/battery-060.svg \
    images/battery-070.svg \
    images/battery-080-charging.svg \
    images/battery-080.svg \
    images/battery-090.svg \
    images/battery-100-charging.svg \
    images/battery-100.svg \
    images/emblem-question.svg \
    images/icon-headset-glow-norgb.png \
    images/icon-headset-glow-offline.png \
    images/icon-headset-glow-rgb.png

resources.files = $${DISTFILES}
resources.prefix = /
RESOURCES += resources
