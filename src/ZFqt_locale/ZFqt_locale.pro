TEMPLATE	=	app
QT			+=	core gui widgets sql xml
INCLUDEPATH	+=	$$PWD/../../include
LIBS		+=	-L$$PWD/../../lib -lZFqt
DESTDIR		=	$$PWD/../../bin
CONFIG		+=	c++17
QMAKE_LFLAGS_WINDOWS	=	/SUBSYSTEM:WINDOWS
DEFINES		+=	QT_DEPRECATED_WARNINGS

TARGET		=	ZFqt_locale
RC_ICONS	=	res/icons/ZFqt_locale.ico

HEADERS +=  \
	db_app.h \
	main_window.h

SOURCES += \
	main.cpp \
	db_app.cpp \
	main_window.cpp \

RESOURCES += ZFqt_locale.qrc
