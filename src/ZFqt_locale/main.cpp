
#include <QApplication>
#include <QDir>
#include "main_window.h"

int main(int argc, char* argv[])
{
	QDir::setCurrent(QCoreApplication::applicationDirPath());

	QCoreApplication::addLibraryPath(".");
	qRegisterMetaType< ZFqt_SPLogItem >("ZFqt_SPLogItem");

	QApplication app(argc, argv);

	ZFqt::App::Instance()->SetAppGroupName("ZFqt");
	ZFqt::App::Instance()->SetAppName("ZFqt_locale");
	ZFqt::App::Instance()->SetAppVersion("1.0");
	ZFqt::App::Instance()->SetAppDisplayName("ZFqt locale");

	ZFqt::App::Instance()->SetVendor("Zhu Fei");
	ZFqt::App::Instance()->SetVendorDomain("https:://github.com/zhufei8519/ZFqt.git");
	ZFqt::App::Instance()->SetVendorEmail("zhufei8519@outlook.com");

	MainWindow	mainWindow;
	mainWindow.show();

	return app.exec();
}

