@echo off

:: generate project files for Visual Studio
echo generate project files for Visual Studio
qmake -tp vc -recursive ZFqt.pro
