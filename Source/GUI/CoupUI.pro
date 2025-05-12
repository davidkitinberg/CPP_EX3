QT += widgets

TEMPLATE = app
TARGET = CoupUI

SOURCES += CoupUI.cpp \
           TargetDialog.cpp \
           ../main.cpp

HEADERS += CoupUI.hpp \
           TargetDialog.hpp

FORMS += CoupUI.ui
FORMS += GameSetupDialog.ui

INCLUDEPATH += ../
