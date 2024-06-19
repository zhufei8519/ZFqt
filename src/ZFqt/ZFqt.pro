TEMPLATE	=	lib
DEFINES		+=	ZFqt_BUILD_DLL
QT			+=	core gui widgets sql xml
INCLUDEPATH	+=	$$PWD/../../include
DESTDIR		=	$$PWD/../../lib
win32: DLLDESTDIR	=	$$PWD/../../bin
#win32 {
#	CONFIG(debug, debug|release):{
#		DESTDIR	=	$$PWD/../../lib/debug
#	}
#	else:CONFIG(release, debug|release):{
#		DESTDIR	=	$$PWD/../../lib/release
#	}
#}

HEADERS += \
	$$PWD/../../include/ZFqt/errno.h \
	$$PWD/../../include/ZFqt/consts.h \
	$$PWD/../../include/ZFqt/export.h \
	$$PWD/../../include/ZFqt/types.h \
	$$PWD/../../include/ZFqt/common.h \
	$$PWD/../../include/ZFqt/app.h \
	$$PWD/../../include/ZFqt/db_app_base.h \
	$$PWD/../../include/ZFqt/locale.h \
	$$PWD/../../include/ZFqt/threaded/log.h \
	$$PWD/../../include/ZFqt/dlgs.h

SOURCES += \
	$$PWD/common.cpp \
	$$PWD/app.cpp \
	$$PWD/db_app_base.cpp \
	$$PWD/locale.cpp \
	$$PWD/threaded/log.cpp \
	$$PWD/dlgs.cpp

