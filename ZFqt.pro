TEMPLATE = subdirs

# project names
SUBDIRS += \
	ZFqt \
	ZFqt_locale

# where to find the projects - give the folders
ZFqt.subdir			=	src/ZFqt
ZFqt_locale.subdir	=	src/ZFqt_locale

# projects' dependencies
ZFqt_locale.depends	=	ZFqt
