
#ifndef ZFqt_APP_H
#define ZFqt_APP_H

#include "ZFqt/types.h"
#include <map>

namespace ZFqt
{
	class ZFqt_Export App
	{
	public:
		App();
		~App();

		void	SetAppName(const QString& qstrAppName)					{ this->m_qstrAppName = qstrAppName; }
		void	SetAppVersion(const QString& qstrAppVersion)			{ this->m_qstrAppVersion = qstrAppVersion; }
		void	SetAppDisplayName(const QString& qstrAppDisplayName)	{ this->m_qstrAppDisplayName = qstrAppDisplayName; }
		void	SetVendor(const QString& qstrVendor)					{ this->m_qstrVendor = qstrVendor; }
		void	SetVendorDomain(const QString& qstrVendorDomain)		{ this->m_qstrVendorDomain = qstrVendorDomain; }
		void	SetVendorEmail(const QString& qstrVendorEmail)			{ this->m_qstrVendorEmail = qstrVendorEmail; }
		void	SetCustomSetting(const QString& qstrSettingName, const QString& qstrSettingValue);
		void	SetAppGroupName(const QString& qstrAppGroupName)		{ this->m_qstrAppGroupName = qstrAppGroupName; }

		const QString&	GetAppGroupName() const		{ return this->m_qstrAppGroupName; }
		const QString&	GetAppName() const			{ return this->m_qstrAppName; }
		const QString&	GetAppDisplayName() const	{ return this->m_qstrAppDisplayName; }
		const QString&	GetAppVersion() const		{ return this->m_qstrAppVersion; }
		const QString&	GetVendor() const			{ return this->m_qstrVendor; }
		const QString&	GetVendorDomain() const		{ return this->m_qstrVendorDomain; }
		const QString&	GetVendorEmail() const		{ return this->m_qstrVendorEmail; }
		QString			GetCustomSetting(const QString& qstrSettingName, const QString& qstrDftSettingValue);

		QString	GetAppPath() const;
		QString	GetAppHome() const;
		QString	GetAppGroupDataHome() const;
		QString	GetAppDataHome() const;

	private:
		QString	m_qstrAppGroupName;
		QString	m_qstrAppName;
		QString	m_qstrAppDisplayName;
		QString	m_qstrAppVersion;

		QString	m_qstrVendor;
		QString	m_qstrVendorDomain;
		QString	m_qstrVendorEmail;

		std::map< QString, QString >	m_mapSettings;

	public:
		static App*	Instance();
		static void	Destroy();

	private:
		static App*	m_spInstance;
	};
};

inline
ZFqt::App* ZFqt::App::Instance()
{
	if (ZFqt::App::m_spInstance == NULL)
	{
		ZFqt::App::m_spInstance	=	new ZFqt::App();
	}

	return ZFqt::App::m_spInstance;
}

inline
void	ZFqt::App::Destroy()
{
	if (ZFqt::App::m_spInstance != NULL)
	{
		delete ZFqt::App::m_spInstance;
		ZFqt::App::m_spInstance	=	NULL;
	}
}

#endif //end ZFqt_APP_H

