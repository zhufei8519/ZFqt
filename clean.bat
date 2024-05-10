@echo off

:: clean project files for Visual Studio
echo clean project files for Visual Studio

SET FILE_ITEM=.qmake.stash
echo clean file [%FILE_ITEM%]
if exist %FILE_ITEM% (
	del %FILE_ITEM%
)
SET FILE_ITEM=ZFqt.sln
echo clean file [%FILE_ITEM%]
if exist %FILE_ITEM% (
	del %FILE_ITEM%
)
SET DIR_ITEM=.vs
echo clean directory [%DIR_ITEM%]
if exist %DIR_ITEM% (
	rmdir /S /Q %DIR_ITEM%
)
SET DIR_ITEM=bin
echo clean directory [%DIR_ITEM%]
if exist %DIR_ITEM% (
	rmdir /S /Q %DIR_ITEM%
)
SET DIR_ITEM=lib
echo clean directory [%DIR_ITEM%]
if exist %DIR_ITEM% (
	rmdir /S /Q %DIR_ITEM%
)

SET DIR_ITEM=src\ZFqt\debug
echo clean directory [%DIR_ITEM%]
if exist %DIR_ITEM% (
	rmdir /S /Q %DIR_ITEM%
)
SET DIR_ITEM=src\ZFqt\release
echo clean directory [%DIR_ITEM%]
if exist %DIR_ITEM% (
	rmdir /S /Q %DIR_ITEM%
)
SET FILE_ITEM=src\ZFqt\ZFqt.vcxproj
echo clean file [%FILE_ITEM%]
if exist %FILE_ITEM% (
	del %FILE_ITEM%
)
SET FILE_ITEM=src\ZFqt\ZFqt.vcxproj.filters
echo clean file [%FILE_ITEM%]
if exist %FILE_ITEM% (
	del %FILE_ITEM%
)
SET FILE_ITEM=src\ZFqt\ZFqt.vcxproj.user
echo clean file [%FILE_ITEM%]
if exist %FILE_ITEM% (
	del %FILE_ITEM%
)

SET DIR_ITEM=src\ZFqt_locale\debug
echo clean directory [%DIR_ITEM%]
if exist %DIR_ITEM% (
	rmdir /S /Q %DIR_ITEM%
)
SET DIR_ITEM=src\ZFqt_locale\release
echo clean directory [%DIR_ITEM%]
if exist %DIR_ITEM% (
	rmdir /S /Q %DIR_ITEM%
)
SET FILE_ITEM=src\ZFqt_locale\ZFqt_locale.vcxproj
echo clean file [%FILE_ITEM%]
if exist %FILE_ITEM% (
	del %FILE_ITEM%
)
SET FILE_ITEM=src\ZFqt_locale\ZFqt_locale.vcxproj.filters
echo clean file [%FILE_ITEM%]
if exist %FILE_ITEM% (
	del %FILE_ITEM%
)
SET FILE_ITEM=src\ZFqt_locale\ZFqt_locale.vcxproj.user
echo clean file [%FILE_ITEM%]
if exist %FILE_ITEM% (
	del %FILE_ITEM%
)
SET FILE_ITEM=src\ZFqt_locale\ZFqt_locale_resource.rc
echo clean file [%FILE_ITEM%]
if exist %FILE_ITEM% (
	del %FILE_ITEM%
)
