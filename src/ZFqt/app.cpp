
#include "ZFqt/app.h"
#include <QApplication>
#include <QDir>

ZFqt::App*	ZFqt::App::m_spInstance	=	NULL;

ZFqt::App::App()
{
}

ZFqt::App::~App()
{
}

void	ZFqt::App::SetCustomSetting(const QString& qstrSettingName, const QString& qstrSettingValue)
{
	if (!qstrSettingName.isEmpty())
	{
		this->m_mapSettings[qstrSettingName]	=	qstrSettingValue;
	}
}

QString	ZFqt::App::GetCustomSetting(const QString& qstrSettingName, const QString& qstrDftSettingValue)
{
	if (qstrSettingName.isEmpty())
	{
		return "";
	}

	std::map< QString, QString >::iterator	iterMapSettings	=
		this->m_mapSettings.find(qstrSettingName);
	if (iterMapSettings != this->m_mapSettings.end())
	{
		return iterMapSettings->second;
	}
	else
	{
		this->m_mapSettings[qstrSettingName]	=	qstrDftSettingValue;

		return qstrDftSettingValue;
	}
}

QString	ZFqt::App::GetAppPath() const
{
	return QApplication::applicationFilePath();
}

QString	ZFqt::App::GetAppHome() const
{
	QString	qstrBinDir	=	QApplication::applicationDirPath();
	QDir	dirBinDir(qstrBinDir);

	qstrBinDir.resize(qstrBinDir.length() - dirBinDir.dirName().length() - 1);

	return qstrBinDir;
}

QString	ZFqt::App::GetAppDataHome() const
{
	QString	qstrAppName(this->m_qstrAppName);
	if (qstrAppName.isEmpty())
	{
		qstrAppName	=	"ZFqt";
	}

	return QDir::toNativeSeparators(QDir::homePath() + QDir::separator() + "." + qstrAppName);
}

