
#ifndef ZFqt_LOCALE_H
#define ZFqt_LOCALE_H

#include "ZFqt/db_app_base.h"
class QLabel;
class QAction;

namespace ZFqt
{
	class ZFqt_Export Locale: public DBAppBase
	{
	public:
		Locale();
		virtual ~Locale();

		virtual int32_t	Open(const QString& qstrDBHome, const QString& qstrDBName, const QString& qstrPassword);
		virtual int32_t	Close();

		QString	GetLocaleString(const QString& qstr_en_US);

		void	GetSupportedLocales(std::map< QString, QString >& mapSupportedLocales);
		QString	GetCurLocale();
		void	SetCurLocale(const QString& qstrCurLocale);

		void	GenerateLocaleTemplate(const QString& qstrFilePathLocale);
		QString	ImportLocale(const QString& qstrFilePathLocale);

		static	QString	GetSystemLocale();

	protected:
		int32_t	OpenTableSupportedLocales();
		int32_t	OpenTableLocale(const QString& qstrLocale, bool bTruncate = false);

		int32_t	PersistenceLocale(const QString& qstrLocaleName, const QString& qstrLocaleValue,
			const std::map< QString, QString >& mapLocaleStrings);

	protected:
		QString	m_qstrCurLocale;

	public:
		static Locale*	Instance();
		static void		Destroy();

	private:
		static Locale*	m_spInstance;
	};
};

#define	ZFqt_T(x)	ZFqt::Locale::Instance()->GetLocaleString(x)

inline
ZFqt::Locale*	ZFqt::Locale::Instance()
{
	if (ZFqt::Locale::m_spInstance == NULL)
	{
		ZFqt::Locale::m_spInstance	=	new ZFqt::Locale();
	}

	return ZFqt::Locale::m_spInstance;
}

inline
void	ZFqt::Locale::Destroy()
{
	if (ZFqt::Locale::m_spInstance != NULL)
	{
		delete ZFqt::Locale::m_spInstance;
		ZFqt::Locale::m_spInstance	=	NULL;
	}
}

#endif //end ZFqt_LOCALE_H

