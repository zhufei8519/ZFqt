
#include "db_app.h"
#include "ZFqt/app.h"
#include "ZFqt/threaded/log.h"

DBApp* DBApp::m_spInstance = NULL;

DBApp::DBApp()
	:DBAppBase()
{
}

DBApp::~DBApp()
{
	this->Close();
}

int32_t	DBApp::Open(const QString& qstrDBName, const QString& qstrPassword)
{
	int32_t	nErrno = ZFqt::E_Errno_ERR_GENERAL;

	ZFqt::LogMgr::Instance()->Log(NULL, ZFqt_Log_Header_Info, ZFqt::E_LogLevel_Info,
		"DBApp::Open(\"%s\") begin ...\n",
		qstrDBName.toStdString().c_str());

	for (int32_t iOnce = 0; iOnce < 1; ++iOnce)
	{
		nErrno = DBAppBase::Open(ZFqt::App::Instance()->GetAppDataHome(), qstrDBName, qstrPassword);
		if (nErrno != ZFqt::E_Errno_SUCCESS)
		{
			break;
		}

		nErrno = ZFqt::E_Errno_SUCCESS;
	}
	if (nErrno == ZFqt::E_Errno_SUCCESS)
	{
		ZFqt::LogMgr::Instance()->Log(NULL, ZFqt_Log_Header_Info, ZFqt::E_LogLevel_Info,
			"DBApp::Open(\"%s\") successfully .\n",
			qstrDBName.toStdString().c_str());
	}
	else
	{
		ZFqt::LogMgr::Instance()->Log(NULL, ZFqt_Log_Header_Info, ZFqt::E_LogLevel_Warn,
			"DBApp::Open(\"%s\") failed !!!\n",
			qstrDBName.toStdString().c_str());

		this->Close();
	}

	return nErrno;
}

int32_t	DBApp::Close()
{
	int32_t	nErrno	=	ZFqt::E_Errno_ERR_GENERAL;

	if (this->IsOpen())
	{
		QString	qstrDBName	=	this->m_qstrDBName;

		ZFqt::LogMgr::Instance()->Log(NULL, ZFqt_Log_Header_Info, ZFqt::E_LogLevel_Info,
			"DBApp::Close(\"%s\") begin ...\n",
			qstrDBName.toStdString().c_str());

		for (int32_t iOnce = 0; iOnce < 1; ++iOnce)
		{
			nErrno = DBAppBase::Close();
			if (nErrno != ZFqt::E_Errno_SUCCESS)
			{
				break;
			}

			nErrno = ZFqt::E_Errno_SUCCESS;
		}
		if (nErrno == ZFqt::E_Errno_SUCCESS)
		{
			ZFqt::LogMgr::Instance()->Log(NULL, ZFqt_Log_Header_Info, ZFqt::E_LogLevel_Info,
				"DBApp::Close(\"%s\") successfully !!!\n",
				qstrDBName.toStdString().c_str());
		}
		else
		{
			ZFqt::LogMgr::Instance()->Log(NULL, ZFqt_Log_Header_Info, ZFqt::E_LogLevel_Warn,
				"DBApp::Close(\"%s\") failed !!!\n",
				qstrDBName.toStdString().c_str());

			this->Close();
		}
	}

	return nErrno;
}

