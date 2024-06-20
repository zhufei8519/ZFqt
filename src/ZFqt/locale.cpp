
#include "ZFqt/locale.h"
#include "ZFqt/app.h"
#include "ZFqt/threaded/log.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

#include <QDomDocument>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

#include <QMenu>
#include <QAction>
#include <QToolButton>
#include <QDockWidget>
#include <QTreeWidget>
#include <QTabWidget>

ZFqt::Locale*	ZFqt::Locale::m_spInstance	=	NULL;

ZFqt::Locale::Locale()
	:ZFqt::DBAppBase()
	, m_qstrCurLocale("en_US")
{
}

ZFqt::Locale::~Locale()
{
	this->Close();
}

int32_t	ZFqt::Locale::Open(const QString& qstrDBHome, const QString& qstrDBName, const QString& qstrPassword, const QString& qstrDataSourceDir)
{
	// close first if app database already opened
	if (this->IsOpen())
	{
		this->Close();
	}

	ZFqt::LogMgr::Instance()->Log(NULL, ZFqt_Log_Header_Info, ZFqt::E_LogLevel_Debug,
		"ZFqt::Locale::Open(\"%s\") begin ...\n",
		qstrDBName.toStdString().c_str());

	int32_t	nErrno	=	ZFqt::E_Errno_ERR_GENERAL;

	for (int32_t iOnce = 0; iOnce < 1; ++iOnce)
	{
		nErrno	=	ZFqt::DBAppBase::Open(qstrDBHome, qstrDBName, qstrPassword);
		if (nErrno != ZFqt::E_Errno_SUCCESS)
		{
			break;
		}

		// open supported locales' table, create if not exists
		nErrno	=	this->OpenTableSupportedLocales();
		if (nErrno != ZFqt::E_Errno_SUCCESS)
		{
			break;
		}

		// get configured CUR_LOCALE, initialize if not exists
		this->m_qstrCurLocale	=	this->GetCfgItem("CUR_LOCALE", "");
		if (this->m_qstrCurLocale.isEmpty())
		{
			this->m_qstrCurLocale	=	ZFqt::Locale::GetSystemLocale();

			ZFqt::LogMgr::Instance()->Log(NULL, ZFqt_Log_Header_Info, ZFqt::E_LogLevel_Debug,
				"ZFqt::Locale::Open(\"%s\") : NONE CUR_LOCALE configured, use system locale[\"%s\"] ...\n",
				this->m_qstrDBName.toStdString().c_str(),
				this->m_qstrCurLocale.toStdString().c_str());
		}
		if (!this->IsLocaleInitialized(this->m_qstrCurLocale))
		{
			this->m_qstrCurLocale	=	this->ImportLocale(ZFqt::App::Instance()->GetAppHome() + QDir::separator() + qstrDataSourceDir + QDir::separator() + "locale" + QDir::separator() + "locale." + this->m_qstrCurLocale + ".xml");
			if (this->m_qstrCurLocale.isEmpty())
			{
				this->m_qstrCurLocale	=	"en_US";
			}
		}

		ZFqt::LogMgr::Instance()->Log(NULL, ZFqt_Log_Header_Info, ZFqt::E_LogLevel_Debug,
			"ZFqt::Locale::Open(\"%s\") successfully with locale[\"%s\"] ...\n",
			this->m_qstrDBName.toStdString().c_str(),
			this->m_qstrCurLocale.toStdString().c_str());

		nErrno	=	ZFqt::E_Errno_SUCCESS;
	}
	if (nErrno != ZFqt::E_Errno_SUCCESS)
	{
		this->Close();
	}

	return nErrno;
}

int32_t	ZFqt::Locale::Close()
{
	if (this->IsOpen())
	{
		QString	qstrDBName	=	this->m_qstrDBName;

		ZFqt::LogMgr::Instance()->Log(NULL, ZFqt_Log_Header_Info, ZFqt::E_LogLevel_Debug,
			"ZFqt::Locale::Close(\"%s\") begin ...\n",
			qstrDBName.toStdString().c_str());

		ZFqt::DBAppBase::Close();

		ZFqt::LogMgr::Instance()->Log(NULL, ZFqt_Log_Header_Info, ZFqt::E_LogLevel_Debug,
			"ZFqt::Locale::Close(\"%s\") successfully !!!\n",
			qstrDBName.toStdString().c_str());
	}

	this->m_qstrCurLocale =	"en_US";

	return ZFqt::E_Errno_SUCCESS;
}

bool	ZFqt::Locale::IsLocaleInitialized(const QString& qstrLocaleName)
{
	// check parameters
	if (qstrLocaleName.isEmpty())
	{
		ZFqt::LogMgr::Instance()->Log(NULL, ZFqt_Log_Header_Info, ZFqt::E_LogLevel_Debug,
			"ZFqt::Locale::IsLocaleInitialized(\"%s\") failed: locale name is EMPTY !!!\n",
			qstrLocaleName.toStdString().c_str());

		return false;
	}

	// check locale is supported
	std::map< QString, QString >	mapSupportedLocales;
	this->GetSupportedLocales(mapSupportedLocales);
	if (mapSupportedLocales.find(qstrLocaleName) == mapSupportedLocales.end())
	{
		ZFqt::LogMgr::Instance()->Log(NULL, ZFqt_Log_Header_Info, ZFqt::E_LogLevel_Debug,
			"ZFqt::Locale::IsLocaleInitialized(\"%s\") : locale NOT supported !!!\n",
			qstrLocaleName.toStdString().c_str());

		return false;
	}

	// check locale table exists or not
	QString	qstrLocaleTableName	=	QString("tbl_") + qstrLocaleName;
	if (!this->IsTableExists(qstrLocaleTableName))
	{
		ZFqt::LogMgr::Instance()->Log(NULL, ZFqt_Log_Header_Info, ZFqt::E_LogLevel_Debug,
			"ZFqt::Locale::IsLocaleInitialized(\"%s\") : locale table[\"%s\"] NOT EXIST !!!\n",
			qstrLocaleName.toStdString().c_str(),
			qstrLocaleTableName.toStdString().c_str());

		return false;
	}

	return true;
}

QString	ZFqt::Locale::GetLocaleString(const QString& qstr_en_US)
{
	QString	qstrLocale(qstr_en_US);

	if (this->m_qstrCurLocale != "en_US")
	{
		if (this->IsOpen())
		{
			for (int32_t iOnce = 0; iOnce < 1; ++iOnce)
			{
				QString		qstrStmt =
					QString("SELECT value FROM tbl_") + this->m_qstrCurLocale + " WHERE name = '" + qstr_en_US + "'";
				QSqlQuery	query(QSqlDatabase::database(this->m_qstrDBName));
				if (!query.exec(qstrStmt))
				{
					ZFqt::LogMgr::Instance()->Log(NULL, ZFqt_Log_Header_Info, ZFqt::E_LogLevel_Warn,
						"ZFqt::Locale::GetLocaleString(\"%s\") : failed to execute SQL[\"%s\"], ERROR message[\"%s\"], return default value[\"%s\"] directly !!!\n",
						qstr_en_US.toStdString().c_str(),
						qstrStmt.toStdString().c_str(),
						query.lastError().text().toStdString().c_str(),
						qstr_en_US.toStdString().c_str());

					break;
				}
				if (!query.next())
				{
					ZFqt::LogMgr::Instance()->Log(NULL, ZFqt_Log_Header_Info, ZFqt::E_LogLevel_Warn,
						"ZFqt::Locale::GetLocaleString(\"%s\") : NONE locale string queried, set and return default value[\"%s\"] !!!\n",
						qstr_en_US.toStdString().c_str(),
						qstr_en_US.toStdString().c_str());

					QString	qstrStmtSet =
						QString("INSERT INTO tbl_") + this->m_qstrCurLocale + "(name, value) VALUES('" + qstr_en_US + "', '" + qstr_en_US + "')";
					QSqlQuery	query(QSqlDatabase::database(this->m_qstrDBName));
					query.exec(qstrStmtSet);

					break;
				}

				qstrLocale =	query.value(0).toString();
			}
		}
	}

	ZFqt::LogMgr::Instance()->Log(NULL, ZFqt_Log_Header_Info, ZFqt::E_LogLevel_Trace,
		"ZFqt::Locale::GetLocaleString(\"%s\") successfully with value[\"%s\"].\n",
		qstr_en_US.toStdString().c_str(), qstrLocale.toStdString().c_str());

	return qstrLocale;
}

void	ZFqt::Locale::GetSupportedLocales(std::map< QString, QString >& mapSupportedLocales)
{
	mapSupportedLocales.clear();
	mapSupportedLocales.insert(std::make_pair("en_US", "English"));

	QString		qstrStmt	=
		"SELECT name, value FROM tbl_locale ORDER BY name";
	QSqlQuery	query(QSqlDatabase::database(this->m_qstrDBName));
	if (query.exec(qstrStmt))
	{
		while (query.next())
		{
			mapSupportedLocales.insert(std::make_pair(query.value(0).toString(), query.value(1).toString()));
		}
	}
}

QString	ZFqt::Locale::GetLocaleValue(const QString& qstrLocaleName)
{
	QString	qstrLocaleValue;

	QString		qstrStmt =
		QString("SELECT value FROM tbl_locale WHERE name='") + qstrLocaleName + "'";
	QSqlQuery	query(QSqlDatabase::database(this->m_qstrDBName));
	if (query.exec(qstrStmt))
	{
		if (query.next())
		{
			qstrLocaleValue	=	query.value(0).toString();
		}
	}

	return qstrLocaleValue;
}

QString	ZFqt::Locale::GetCurLocale()
{
	return this->m_qstrCurLocale;
}

bool	ZFqt::Locale::SetCurLocale(const QString& qstrCurLocale)
{
	this->m_qstrCurLocale	=	qstrCurLocale;

	this->SetCfgItem("CUR_LOCALE", this->m_qstrCurLocale);
	this->OpenTableLocale(this->m_qstrCurLocale);

	this->OnLoacleChanged();

	return true;
}

bool	ZFqt::Locale::GenerateLocaleTemplate(const QString& qstrFilePathLocale)
{
	// check parameters
	if (qstrFilePathLocale.isEmpty())
	{
		ZFqt::LogMgr::Instance()->Log(NULL, ZFqt_Log_Header_Info, ZFqt::E_LogLevel_Error,
			"ZFqt::Locale::GenerateLocaleTemplate(\"%s\") failed: qstrFilePathLocale is EMPTY !!!\n",
			qstrFilePathLocale.toStdString().c_str());

		return false;
	}

	ZFqt::LogMgr::Instance()->Log(NULL, ZFqt_Log_Header_Info, ZFqt::E_LogLevel_Debug,
		"ZFqt::Locale::GenerateLocaleTemplate(\"%s\") begin ...\n",
		qstrFilePathLocale.toStdString().c_str());

	QString	qstrLocaleName(this->m_qstrCurLocale);
	QString	qstrLocaleValue	=	this->GetLocaleValue(qstrLocaleName);
	if (qstrLocaleValue.isEmpty())
	{
		qstrLocaleName	=	"en_US";
		qstrLocaleValue	=	"English";
	}

	for (int32_t iOnce = 0; iOnce < 1; ++iOnce)
	{
		QFile	xmlFile(qstrFilePathLocale);
		if (!xmlFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
		{
			ZFqt::LogMgr::Instance()->Log(NULL, ZFqt_Log_Header_Info, ZFqt::E_LogLevel_Error,
				"ZFqt::Locale::GenerateLocaleTemplate(\"%s\") failed: open file to write failed !!!\n",
				qstrFilePathLocale.toStdString().c_str());

			break;
		}

		QXmlStreamWriter	xmlWriter(&xmlFile);
		xmlWriter.setAutoFormatting(true);
		//xmlWriter.setAutoFormattingIndent(4);
		xmlWriter.writeStartDocument("1.0", true);

			xmlWriter.writeStartElement("Locale");
			xmlWriter.writeAttribute("name", qstrLocaleName);
			xmlWriter.writeAttribute("value", qstrLocaleValue);

			QString	qstrStmt =
				QString("SELECT name, value FROM tbl_") + this->m_qstrCurLocale + " ORDER BY name ASC";
			QSqlQuery	query(QSqlDatabase::database(this->m_qstrDBName));
			if (!query.exec(qstrStmt))
			{
				ZFqt::LogMgr::Instance()->Log(NULL, ZFqt_Log_Header_Info, ZFqt::E_LogLevel_Error,
					"ZFqt::Locale::GenerateLocaleTemplate(\"%s\") failed: failed to execute SQL[\"%s\"], ERROR message[\"%s\"] !!!\n",
					qstrFilePathLocale.toStdString().c_str(),
					qstrStmt.toStdString().c_str(),
					query.lastError().text().toStdString().c_str());

				break;
			}
			while (query.next())
			{
				xmlWriter.writeStartElement("Item");
				xmlWriter.writeAttribute("en_US", query.value(0).toString());
				xmlWriter.writeAttribute("locale", query.value(1).toString());
				xmlWriter.writeEndElement();
			}

			xmlWriter.writeEndElement();

		xmlWriter.writeEndDocument();

		xmlFile.close();

		ZFqt::LogMgr::Instance()->Log(NULL, ZFqt_Log_Header_Info, ZFqt::E_LogLevel_Debug,
			"ZFqt::Locale::GenerateLocaleTemplate(\"%s\") finished successfully !!!\n",
			qstrFilePathLocale.toStdString().c_str());
	}

	return true;
}

QString	ZFqt::Locale::ImportLocale(const QString& qstrFilePathLocale)
{
	// check parameters
	if (qstrFilePathLocale.isEmpty())
	{
		ZFqt::LogMgr::Instance()->Log(NULL, ZFqt_Log_Header_Info, ZFqt::E_LogLevel_Error,
			"ZFqt::Locale::ImportLocale(\"%s\") failed: qstrFilePathLocale is EMPTY !!!\n",
			qstrFilePathLocale.toStdString().c_str());

		return "";
	}

	ZFqt::LogMgr::Instance()->Log(NULL, ZFqt_Log_Header_Info, ZFqt::E_LogLevel_Debug,
		"ZFqt::Locale::ImportLocale(\"%s\") begin ...\n",
		qstrFilePathLocale.toStdString().c_str());

	QString	qstrLocaleName;
	QString	qstrLocaleValue;
	std::map< QString, QString >	mapLocaleStrings;

	for (int32_t iOnce = 0; iOnce < 1; ++iOnce)
	{
		QFile	xmlFile(qstrFilePathLocale);
		if (!xmlFile.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			ZFqt::LogMgr::Instance()->Log(NULL, ZFqt_Log_Header_Info, ZFqt::E_LogLevel_Error,
				"ZFqt::Locale::ImportLocale(\"%s\") failed: open file to read failed !!!\n",
				qstrFilePathLocale.toStdString().c_str());

			break;
		}

		QXmlStreamReader	xmlReader(&xmlFile);
		while (!xmlReader.atEnd() && !xmlReader.hasError())
		{
			QXmlStreamReader::TokenType	tokenType	=	xmlReader.readNext();
			if (tokenType == QXmlStreamReader::StartElement)
			{
				QString					qstrElementName	=	xmlReader.name().toString();
				QXmlStreamAttributes	attributes		=	xmlReader.attributes();
				if (qstrElementName == "Locale")
				{
					qstrLocaleName	=	attributes.value("name").toString();
					qstrLocaleValue	=	attributes.value("value").toString();

					ZFqt::LogMgr::Instance()->Log(NULL, ZFqt_Log_Header_Info, ZFqt::E_LogLevel_Debug,
						"rootNode[\"Locale\"] read, LocaleName[\"%s\"], LocaleValue[\"%s\"] ...\n",
						qstrLocaleName.toStdString().c_str(), qstrLocaleValue.toStdString().c_str());
				}
				else if (qstrElementName == "Item")
				{
					QString	qstr_en_US	=	attributes.value("en_US").toString();
					QString	qstr_locale	=	attributes.value("locale").toString();
					mapLocaleStrings[qstr_en_US]	=	qstr_locale;

					ZFqt::LogMgr::Instance()->Log(NULL, ZFqt_Log_Header_Info, ZFqt::E_LogLevel_Debug,
						"itemNode[\"Item\"] read, English[\"%s\"], Locale[\"%s\"] ...\n",
						qstr_en_US.toStdString().c_str(), qstr_locale.toStdString().c_str());
				}
			}
			else if (tokenType == QXmlStreamReader::StartDocument)
			{
				ZFqt::LogMgr::Instance()->Log(NULL, ZFqt_Log_Header_Info, ZFqt::E_LogLevel_Debug,
					"ZFqt::Locale::ImportLocale(\"%s\") begin read locale file ...\n",
					qstrFilePathLocale.toStdString().c_str());
			}
			else if (tokenType == QXmlStreamReader::EndDocument)
			{
				ZFqt::LogMgr::Instance()->Log(NULL, ZFqt_Log_Header_Info, ZFqt::E_LogLevel_Debug,
					"ZFqt::Locale::ImportLocale(\"%s\") finished read locale file !!!\n",
					qstrFilePathLocale.toStdString().c_str());
			}
		}
		if (xmlReader.hasError())
		{
			ZFqt::LogMgr::Instance()->Log(NULL, ZFqt_Log_Header_Info, ZFqt::E_LogLevel_Error,
				"ZFqt::Locale::ImportLocale(\"%s\") failed: XML file is corrupted !!!\n",
				qstrFilePathLocale.toStdString().c_str());

			qstrLocaleName.clear();
			qstrLocaleValue.clear();
			mapLocaleStrings.clear();

			break;
		}

		if (this->PersistenceLocale(qstrLocaleName, qstrLocaleValue, mapLocaleStrings) != ZFqt::E_Errno_SUCCESS)
		{
			ZFqt::LogMgr::Instance()->Log(NULL, ZFqt_Log_Header_Info, ZFqt::E_LogLevel_Error,
				"ZFqt::Locale::ImportLocale(\"%s\") failed: failed on persistence !!!\n",
				qstrFilePathLocale.toStdString().c_str());

			qstrLocaleName.clear();
			qstrLocaleValue.clear();
			mapLocaleStrings.clear();

			break;
		}

		ZFqt::LogMgr::Instance()->Log(NULL, ZFqt_Log_Header_Info, ZFqt::E_LogLevel_Debug,
			"ZFqt::Locale::ImportLocale(\"%s\") finished successfully !!!\n",
			qstrFilePathLocale.toStdString().c_str());
		xmlFile.close();
	}

	return qstrLocaleName;
}

QString	ZFqt::Locale::GetSystemLocale()
{
	QLocale	localeSys;
	QString	qstrLangSys;

	switch (localeSys.language())
	{
	case QLocale::Chinese:
		qstrLangSys	=	"zh_CN";
		break;
	default:
		qstrLangSys	=	"en_US";
		break;
	}

	return qstrLangSys;
}

void	ZFqt::Locale::OnLoacleChanged()
{
	// Menu
	std::map< QMenu*, const char* >::iterator	iterMapLocales_Menu;
	for (iterMapLocales_Menu = this->m_mapLocales_Menu.begin(); iterMapLocales_Menu != this->m_mapLocales_Menu.end(); ++iterMapLocales_Menu)
	{
		iterMapLocales_Menu->first->setTitle(ZFqt_T(iterMapLocales_Menu->second));
	}

	// MenuItem
	std::map< QAction*, const char* >::iterator	iterMapLocales_MenuItem;
	for (iterMapLocales_MenuItem = this->m_mapLocales_MenuItem.begin(); iterMapLocales_MenuItem != this->m_mapLocales_MenuItem.end(); ++iterMapLocales_MenuItem)
	{
		iterMapLocales_MenuItem->first->setText(ZFqt_T(iterMapLocales_MenuItem->second));
	}

	// ToolButton
	std::map< QToolButton*, const char* >::iterator	iterMapLocales_ToolButton;
	for (iterMapLocales_ToolButton = this->m_mapLocales_ToolButton.begin(); iterMapLocales_ToolButton != this->m_mapLocales_ToolButton.end(); ++iterMapLocales_ToolButton)
	{
		iterMapLocales_ToolButton->first->setText(ZFqt_T(iterMapLocales_ToolButton->second));
	}

	// DockWidget
	std::map< QDockWidget*, const char* >::iterator	iterMapLocales_DockWidget;
	for (iterMapLocales_DockWidget = this->m_mapLocales_DockWidget.begin(); iterMapLocales_DockWidget != this->m_mapLocales_DockWidget.end(); ++iterMapLocales_DockWidget)
	{
		iterMapLocales_DockWidget->first->setWindowTitle(ZFqt_T(iterMapLocales_DockWidget->second));
	}

	// TreeWidgetItem
	std::map< QTreeWidgetItem*, const char* >::iterator	iterMapLocales_TreeWidgetItem;
	for (iterMapLocales_TreeWidgetItem = this->m_mapLocales_TreeWidgetItem.begin(); iterMapLocales_TreeWidgetItem != this->m_mapLocales_TreeWidgetItem.end(); ++iterMapLocales_TreeWidgetItem)
	{
		iterMapLocales_TreeWidgetItem->first->setText(0, ZFqt_T(iterMapLocales_TreeWidgetItem->second));
	}

	// TabPage
	std::multimap< QTabWidget*, ZFqt::TTabHeaderLocaleInfo >::iterator	iterMapLocales_TabPage;
	for (iterMapLocales_TabPage = this->m_multimapLocales_TabPage.begin(); iterMapLocales_TabPage != this->m_multimapLocales_TabPage.end(); ++iterMapLocales_TabPage)
	{
		iterMapLocales_TabPage->first->setTabText(iterMapLocales_TabPage->second.nIndex, ZFqt_T(iterMapLocales_TabPage->second.pstrLabel_en_US));
	}
}

void	ZFqt::Locale::OnMenuCreated(QMenu* pMenu, const char* pstrLabel_en_US)
{
	this->m_mapLocales_Menu.insert(std::make_pair(pMenu, pstrLabel_en_US));
}

void	ZFqt::Locale::OnMenuItemCreated(QAction* pMenuItem, const char* pstrLabel_en_US)
{
	this->m_mapLocales_MenuItem.insert(std::make_pair(pMenuItem, pstrLabel_en_US));
}

void	ZFqt::Locale::OnToolButtonCreated(QToolButton* pToolButton, const char* pstrLabel_en_US)
{
	this->m_mapLocales_ToolButton.insert(std::make_pair(pToolButton, pstrLabel_en_US));
}

void	ZFqt::Locale::OnDockWidgetCreated(QDockWidget* pDockWidget, const char* pstrLabel_en_US)
{
	this->m_mapLocales_DockWidget.insert(std::make_pair(pDockWidget, pstrLabel_en_US));
}

void	ZFqt::Locale::OnTreeWidgetItemCreated(QTreeWidgetItem* pTreeWidgetItem, const char* pstrLabel_en_US)
{
	this->m_mapLocales_TreeWidgetItem.insert(std::make_pair(pTreeWidgetItem, pstrLabel_en_US));
}

void	ZFqt::Locale::OnTabPageCreated(QTabWidget* pTabWidget, int nIndex, const char* pstrLabel_en_US)
{
	ZFqt::TTabHeaderLocaleInfo	tabHeaderLocalInfo(pTabWidget, nIndex, pstrLabel_en_US);
	this->m_multimapLocales_TabPage.insert(std::make_pair(pTabWidget, tabHeaderLocalInfo));
}

void	ZFqt::Locale::OnMenuUpdated(QMenu* pMenu, const char* pstrLabel_en_US)
{
	std::map< QMenu*, const char* >::iterator	iterMapLocales_Menu	=
		this->m_mapLocales_Menu.find(pMenu);
	if (iterMapLocales_Menu != this->m_mapLocales_Menu.end())
	{
		iterMapLocales_Menu->second	=	pstrLabel_en_US;
	}
	else
	{
		this->m_mapLocales_Menu.insert(std::make_pair(pMenu, pstrLabel_en_US));
	}
}

void	ZFqt::Locale::OnMenuItemUpdated(QAction* pMenuItem, const char* pstrLabel_en_US)
{
	std::map< QAction*, const char* >::iterator	iterMapLocales_MenuItem =
		this->m_mapLocales_MenuItem.find(pMenuItem);
	if (iterMapLocales_MenuItem != this->m_mapLocales_MenuItem.end())
	{
		iterMapLocales_MenuItem->second	=	pstrLabel_en_US;
	}
	else
	{
		this->m_mapLocales_MenuItem.insert(std::make_pair(pMenuItem, pstrLabel_en_US));
	}
}

void	ZFqt::Locale::OnToolButtonUpdated(QToolButton* pToolButton, const char* pstrLabel_en_US)
{
	std::map< QToolButton*, const char* >::iterator	iterMapLocales_ToolButton =
		this->m_mapLocales_ToolButton.find(pToolButton);
	if (iterMapLocales_ToolButton != this->m_mapLocales_ToolButton.end())
	{
		iterMapLocales_ToolButton->second	=	pstrLabel_en_US;
	}
	else
	{
		this->m_mapLocales_ToolButton.insert(std::make_pair(pToolButton, pstrLabel_en_US));
	}
}

void	ZFqt::Locale::OnDockWidgetUpdated(QDockWidget* pDockWidget, const char* pstrLabel_en_US)
{
	std::map< QDockWidget*, const char* >::iterator	iterMapLocales_DockWidget =
		this->m_mapLocales_DockWidget.find(pDockWidget);
	if (iterMapLocales_DockWidget != this->m_mapLocales_DockWidget.end())
	{
		iterMapLocales_DockWidget->second	=	pstrLabel_en_US;
	}
	else
	{
		this->m_mapLocales_DockWidget.insert(std::make_pair(pDockWidget, pstrLabel_en_US));
	}
}

void	ZFqt::Locale::OnTreeWidgetItemUpdated(QTreeWidgetItem* pTreeWidgetItem, const char* pstrLabel_en_US)
{
	std::map< QTreeWidgetItem*, const char* >::iterator	iterMapLocales_TreeWidgetItem	=
		this->m_mapLocales_TreeWidgetItem.find(pTreeWidgetItem);
	if (iterMapLocales_TreeWidgetItem != this->m_mapLocales_TreeWidgetItem.end())
	{
		iterMapLocales_TreeWidgetItem->second = pstrLabel_en_US;
	}
	else
	{
		this->m_mapLocales_TreeWidgetItem.insert(std::make_pair(pTreeWidgetItem, pstrLabel_en_US));
	}
}

void	ZFqt::Locale::OnTabPageUpdated(QTabWidget* pTabWidget, int nIndex, const char* pstrLabel_en_US)
{
	std::multimap< QTabWidget*, ZFqt::TTabHeaderLocaleInfo >::iterator	iterMultimapLocales_TabPage	=
		this->m_multimapLocales_TabPage.find(pTabWidget);
	for (; iterMultimapLocales_TabPage != this->m_multimapLocales_TabPage.end(); ++iterMultimapLocales_TabPage)
	{
		if (iterMultimapLocales_TabPage->second.nIndex == nIndex)
		{
			break;
		}
		else
		{
			iterMultimapLocales_TabPage	=	this->m_multimapLocales_TabPage.end();
			break;
		}
	}

	if (iterMultimapLocales_TabPage != this->m_multimapLocales_TabPage.end())
	{
		iterMultimapLocales_TabPage->second.pstrLabel_en_US	=	pstrLabel_en_US;
	}
	else
	{
		ZFqt::TTabHeaderLocaleInfo	tabHeaderLocalInfo(pTabWidget, nIndex, pstrLabel_en_US);
		this->m_multimapLocales_TabPage.insert(std::make_pair(pTabWidget, tabHeaderLocalInfo));
	}
}

int32_t	ZFqt::Locale::OpenTableSupportedLocales()
{
	QString	qstrTableName	=	QString("tbl_locale");
	if (this->IsTableExists(qstrTableName))
	{
		return ZFqt::E_Errno_SUCCESS;
	}

	ZFqt::LogMgr::Instance()->Log(NULL, ZFqt_Log_Header_Info, ZFqt::E_LogLevel_Debug,
		"ZFqt::Locale::OpenTableSupportedLocales(\"%s\") begin ...\n",
		qstrTableName.toStdString().c_str());

	QString		qstrStmt =
		QString("CREATE TABLE ") + qstrTableName + "(name VARCHAR(255) PRIMARY KEY, value VARCHAR(255))";
	QSqlQuery	query(QSqlDatabase::database(this->m_qstrDBName));
	if (!query.exec(qstrStmt))
	{
		ZFqt::LogMgr::Instance()->Log(NULL, ZFqt_Log_Header_Info, ZFqt::E_LogLevel_Error,
			"ZFqt::Locale::OpenTableSupportedLocales(\"%s\") failed: SQL script[\"%s\"], ERROR message[\"%s\"] !!!\n",
			qstrTableName.toStdString().c_str(),
			qstrStmt.toStdString().c_str(),
			query.lastError().text().toStdString().c_str());

		return ZFqt::E_Errno_ERR_GENERAL;
	}

	ZFqt::LogMgr::Instance()->Log(NULL, ZFqt_Log_Header_Info, ZFqt::E_LogLevel_Debug,
		"ZFqt::Locale::OpenTableSupportedLocales(\"%s\") successfully .\n",
		qstrTableName.toStdString().c_str());

	return ZFqt::E_Errno_SUCCESS;
}

int32_t	ZFqt::Locale::OpenTableLocale(const QString& qstrLocale, bool bTruncate)
{
	QString	qstrLocaleTableName	=	QString("tbl_") + qstrLocale;
	if (this->IsTableExists(qstrLocaleTableName))
	{
		if (!bTruncate)
		{
			return ZFqt::E_Errno_SUCCESS;
		}

		QString		qstrStmt =
			QString("DROP TABLE ") + qstrLocaleTableName;
		QSqlQuery	query(QSqlDatabase::database(this->m_qstrDBName));
		if (!query.exec(qstrStmt))
		{
			ZFqt::LogMgr::Instance()->Log(NULL, ZFqt_Log_Header_Info, ZFqt::E_LogLevel_Error,
				"ZFqt::Locale::OpenTableLocale(\"%s\") failed: failed on DROP TABLE, SQL script[\"%s\"], ERROR message[\"%s\"] !!!\n",
				qstrLocaleTableName.toStdString().c_str(),
				qstrStmt.toStdString().c_str(),
				query.lastError().text().toStdString().c_str());

			return ZFqt::E_Errno_ERR_GENERAL;
		}
	}

	ZFqt::LogMgr::Instance()->Log(NULL, ZFqt_Log_Header_Info, ZFqt::E_LogLevel_Debug,
		"ZFqt::Locale::OpenTableLocale(\"%s\") begin ...\n",
		qstrLocaleTableName.toStdString().c_str());

	QString		qstrStmt	=
		QString("CREATE TABLE ") + qstrLocaleTableName + "(name VARCHAR(255) PRIMARY KEY, value VARCHAR(255))";
	QSqlQuery	query(QSqlDatabase::database(this->m_qstrDBName));
	if (!query.exec(qstrStmt))
	{
		ZFqt::LogMgr::Instance()->Log(NULL, ZFqt_Log_Header_Info, ZFqt::E_LogLevel_Error,
			"ZFqt::Locale::OpenTableLocale(\"%s\") failed: SQL script[\"%s\"], ERROR message[\"%s\"] !!!\n",
			qstrLocaleTableName.toStdString().c_str(),
			qstrStmt.toStdString().c_str(),
			query.lastError().text().toStdString().c_str());

		return ZFqt::E_Errno_ERR_GENERAL;
	}

	ZFqt::LogMgr::Instance()->Log(NULL, ZFqt_Log_Header_Info, ZFqt::E_LogLevel_Debug,
		"ZFqt::Locale::OpenTableLocale(\"%s\") successfully .\n",
		qstrLocaleTableName.toStdString().c_str());

	return ZFqt::E_Errno_SUCCESS;
}

int32_t	ZFqt::Locale::PersistenceLocale(const QString& qstrLocaleName, const QString& qstrLocaleValue,
	const std::map< QString, QString >& mapLocaleStrings)
{
	int32_t	nErrno	=	ZFqt::E_Errno_ERR_GENERAL;

	for (int32_t iOnce = 0; iOnce < 1; ++iOnce)
	{
		nErrno	=	this->OpenTableLocale(qstrLocaleName, true);
		if (nErrno != ZFqt::E_Errno_SUCCESS)
		{
			break;
		}

		QSqlDatabase	dbLocale = QSqlDatabase::database(this->m_qstrDBName);

		// save locale strings to DB
		// {
			// begin transaction
			dbLocale.transaction();

			std::map< QString, QString >::const_iterator	iterMapLocaleStrings;
			for (iterMapLocaleStrings = mapLocaleStrings.begin(); iterMapLocaleStrings != mapLocaleStrings.end(); ++iterMapLocaleStrings)
			{
				QString	qstrStmt	=
					QString("INSERT INTO tbl_") + qstrLocaleName + "(name, value) VALUES('" + iterMapLocaleStrings->first + "', '" + iterMapLocaleStrings->second + "')";
				QSqlQuery	query(dbLocale);
				query.exec(qstrStmt);
			}

			// finish transaction
			dbLocale.commit();
		// }

		// update supported locale
		{
			QString	qstrStmt_set;

			QString	qstrStmt_get	=
				QString("SELECT value FROM tbl_locale WHERE name='") + qstrLocaleName + "'";
			QSqlQuery	query_get(dbLocale);
			query_get.exec(qstrStmt_get);
			if (query_get.next())
			{
				qstrStmt_set	=	QString("UPDATE tbl_locale SET value='") + qstrLocaleValue + "' WHERE name='" + qstrLocaleName + "'";
			}
			else
			{
				qstrStmt_set	=	QString("INSERT INTO tbl_locale(name, value) VALUES ('") + qstrLocaleName + "', '" + qstrLocaleValue + "')";
			}

			QSqlQuery	query_set(dbLocale);
			query_set.exec(qstrStmt_set);
		}

		nErrno	=	ZFqt::E_Errno_SUCCESS;
	}

	return nErrno;
}

