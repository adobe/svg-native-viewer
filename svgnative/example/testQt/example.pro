QT       += core gui widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

DEFINES += BUILDING_DLL SVG_IMPORT

SVGNATIVE = $$PWD/../..

INCLUDEPATH += \
  $$SVGNATIVE/include \

HEADERS += \
  mainwindow.h \

SOURCES += \
  TestQt.cpp \
  mainwindow.cpp \

LIBS += -L./debug -lQSVGNative

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
