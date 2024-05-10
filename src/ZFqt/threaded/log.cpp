
#include "ZFqt/threaded/log.h"
#include "ZFqt/common.h"
#include <QDir>
#include <QDateTime>
#include <QTextEdit>

namespace ZFqt
{
	static const char*	s_pstrLogLevel_Error	=	"ERROR";
	static const char*	s_pstrLogLevel_Warn		=	"WARN";
	static const char*	s_pstrLogLevel_Info		=	"INFO";
	static const char*	s_pstrLogLevel_Debug	=	"DEBUG";
	static const char*	s_pstrLogLevel_Trace	=	"TRACE";
	static const char*	s_pstrLogLevel_None		=	"NONE";
};

	// private functions
ZFqt::ET_LogLevel	ZFqt::GetLogLevel(const char* pstrLogLevel)
{
	if (::strcasecmp(pstrLogLevel, ZFqt::s_pstrLogLevel_Error) == 0)
	{
		return ZFqt::E_LogLevel_Error;
	}
	else if (::strcasecmp(pstrLogLevel, ZFqt::s_pstrLogLevel_Warn) == 0)
	{
		return ZFqt::E_LogLevel_Warn;
	}
	else if (::strcasecmp(pstrLogLevel, ZFqt::s_pstrLogLevel_Info) == 0)
	{
		return ZFqt::E_LogLevel_Info;
	}
	else if (::strcasecmp(pstrLogLevel, ZFqt::s_pstrLogLevel_Debug) == 0)
	{
		return ZFqt::E_LogLevel_Debug;
	}
	else if (::strcasecmp(pstrLogLevel, ZFqt::s_pstrLogLevel_Trace) == 0)
	{
		return ZFqt::E_LogLevel_Trace;
	}
	else
	{
		return ZFqt::E_LogLevel_None;
	}
}

const char* ZFqt::GetLogLevelString(ZFqt::ET_LogLevel eLogLevel)
{
	switch (eLogLevel)
	{
	case ZFqt::E_LogLevel_Error:
	{
		return ZFqt::s_pstrLogLevel_Error;
		break;
	}
	case ZFqt::E_LogLevel_Warn:
	{
		return ZFqt::s_pstrLogLevel_Warn;
		break;
	}
	case ZFqt::E_LogLevel_Info:
	{
		return ZFqt::s_pstrLogLevel_Info;
		break;
	}
	case ZFqt::E_LogLevel_Debug:
	{
		return ZFqt::s_pstrLogLevel_Debug;
		break;
	}
	case ZFqt::E_LogLevel_Trace:
	{
		return ZFqt::s_pstrLogLevel_Trace;
		break;
	}
	case ZFqt::E_LogLevel_None:
	default:
	{
		return ZFqt::s_pstrLogLevel_None;
		break;
	}
	}
}

ZFqt::LogMgr*	ZFqt::LogMgr::m_spInstance	=	NULL;

ZFqt::LogMgr::LogMgr()
	:m_eLogLevelMax(ZFqt::E_LogLevel_None)
{
	this->m_pThread	=	new QThread;
	this->m_pThread->setObjectName("ZFqt_LogMgr");

	connect(this->m_pThread, SIGNAL(started()),
		this, SLOT(slot_OnThreadStarted()));

	QMutexLocker	lck(&this->m_mutexThread);

	this->moveToThread(this->m_pThread);
	this->m_pThread->start();

	this->m_condThread.wait(&this->m_mutexThread);

	connect(this, SIGNAL(signal_DoInit(void*, void*, void*, void*, void*, QString, QString, QString, bool)),
		this, SLOT(slot_DoInit(void*, void*, void*, void*, void*, QString, QString, QString, bool)));
	connect(this, SIGNAL(signal_DoFini(void*, void*, void*, void*)),
		this, SLOT(slot_DoFini(void*, void*, void*, void*)));
	connect(this, SIGNAL(signal_DoSetModuleLogFile(void*, void*, void*, void*, QString, QString, QString, bool)),
		this, SLOT(slot_DoSetModuleLogFile(void*, void*, void*, void*, QString, QString, QString, bool)));

	connect(this, SIGNAL(signal_WriteLog(QString, ZFqt_SPLogItem)),
		this, SLOT(slot_WriteLog(QString, ZFqt_SPLogItem)));
}

ZFqt::LogMgr::~LogMgr()
{
	this->Fini();

	this->m_pThread->quit();
	this->m_pThread->wait();
	this->m_pThread->deleteLater();
}

int32_t	ZFqt::LogMgr::Init(const QString& qstrLogDirParam, const QString& qstrLogFile, const QString& qstrLogLevelMax, bool bTruncate)
{
	// close log module if it already opened
	if (this->m_eLogLevelMax != ZFqt::E_LogLevel_None)
	{
		this->Log(NULL, ZFqt_Log_Header_Info, ZFqt::E_LogLevel_Info,
			"[open log module] log module already opened, close it first ...\n");

		this->Fini();
	}

	// check parameter
	if (qstrLogFile.isEmpty())
	{
		this->Log(NULL, ZFqt_Log_Header_Info, ZFqt::E_LogLevel_Warn,
			"[open log module] failed: Errno[%d], ErrMsg[\"%s\"] !!!\n",
			ZFqt::E_Errno_ERR_INVALID_PARAMS, "log file is EMPTY");

		return ZFqt::E_Errno_ERR_INVALID_PARAMS;
	}

	this->Log(NULL, ZFqt_Log_Header_Info, ZFqt::E_LogLevel_Info,
		"[open log module] begin ...\n");

	QString	qstrLogDir(qstrLogDirParam);
	if (qstrLogDir.isEmpty() || (qstrLogDir == "."))
	{
		qstrLogDir	=	QDir::toNativeSeparators(QDir::currentPath());
	}
	else if (qstrLogDir == "..")
	{
		qstrLogDir	=	QDir::toNativeSeparators(QDir::currentPath());
		qsizetype	lenLogDir = qstrLogDir.length();
		qstrLogDir.resize(lenLogDir - QDir::current().dirName().length() - 1);
	}

	int32_t	nErrno	=	ZFqt::E_Errno_ERR_GENERAL;
	QString	qstrErrMsg;

	QWaitCondition	cond;
	QMutex			mutex;
	QMutexLocker	lck(&mutex);

	emit this->signal_DoInit(
		&cond, &mutex,
		&nErrno, &qstrErrMsg,
		&this->m_eLogLevelMax,
		qstrLogDir, qstrLogFile, qstrLogLevelMax, bTruncate);

	cond.wait(&mutex);

	if (nErrno == ZFqt::E_Errno_SUCCESS)
	{
		this->Log(NULL, ZFqt_Log_Header_Info, ZFqt::E_LogLevel_Info,
			"[open log module] successfully: MainLogFile[%s%c%s] ...\n",
			qstrLogDir.toStdString().c_str(), QDir::separator().toLatin1(), qstrLogFile.toStdString().c_str());
	}
	else
	{
		this->Log(NULL, ZFqt_Log_Header_Info, ZFqt::E_LogLevel_Warn,
			"[open log module] failed: Errno[%d], ErrMsg[\"%s\"] !!! [no log file output]\n",
			nErrno, qstrErrMsg.toStdString().c_str());
	}

	return nErrno;
}

int32_t	ZFqt::LogMgr::Fini()
{
	this->Log(NULL, ZFqt_Log_Header_Info, ZFqt::E_LogLevel_Info,
		"[close log module] begin ...\n");

	int32_t	nErrno	=	ZFqt::E_Errno_ERR_GENERAL;
	QString	qstrErrMsg;

	QWaitCondition	cond;
	QMutex			mutex;
	QMutexLocker	lck(&mutex);

	emit this->signal_DoFini(
		&cond, &mutex,
		&nErrno, &qstrErrMsg);

	cond.wait(&mutex);

	if (nErrno == ZFqt::E_Errno_SUCCESS)
	{
		this->Log(NULL, ZFqt_Log_Header_Info, ZFqt::E_LogLevel_Info,
			"[close log module] successfully !!! [no log file output]\n");
	}
	else
	{
		this->Log(NULL, ZFqt_Log_Header_Info, ZFqt::E_LogLevel_Warn,
			"[close log module] failed: Errno[%d], ErrMsg[\"%s\"] !!!\n",
			nErrno, qstrErrMsg.toStdString().c_str());
	}

	return nErrno;
}

void	ZFqt::LogMgr::SetLogLevelMax(const QString& qstrLogLevelMax)
{
	this->m_eLogLevelMax	=	ZFqt::GetLogLevel(qstrLogLevelMax.toStdString().c_str());

	this->Log(NULL, ZFqt_Log_Header_Info, ZFqt::E_LogLevel_Info,
		"[set log level] successfully: log level now is [\"%s\"] ...\n",
		qstrLogLevelMax.toStdString().c_str());
}

int32_t	ZFqt::LogMgr::SetModuleLogFile(const QString& qstrModule, const QString& qstrModuleLogDirParam, const QString& qstrModuleLogFile, bool bTruncate)
{
	// check parameters
	if (qstrModule.isEmpty() || (qstrModule == "main"))
	{
		this->Log(NULL, ZFqt_Log_Header_Info, ZFqt::E_LogLevel_Warn,
			"[set module log file] failed: Errno[%d], ErrMsg[\"%s\"], Module[%s], ModuleLogFile[%s%c%s] !!!\n",
			ZFqt::E_Errno_ERR_INVALID_PARAMS, "module name cannot be empty or \"main\"",
			qstrModule.toStdString().c_str(), qstrModuleLogDirParam.toStdString().c_str(), QDir::separator().toLatin1(), qstrModuleLogFile.toStdString().c_str());

		return ZFqt::E_Errno_ERR_INVALID_PARAMS;
	}
	if (qstrModuleLogFile.isEmpty())
	{
		this->Log(NULL, ZFqt_Log_Header_Info, ZFqt::E_LogLevel_Warn,
			"[set module log file] failed: Errno[%d], ErrMsg[\"%s\"], Module[%s], ModuleLogFile[%s%c%s] !!!\n",
			ZFqt::E_Errno_ERR_INVALID_PARAMS, "module log file cannot be EMPTY",
			qstrModule.toStdString().c_str(), qstrModuleLogDirParam.toStdString().c_str(), QDir::separator().toLatin1(), qstrModuleLogFile.toStdString().c_str());

		return ZFqt::E_Errno_ERR_INVALID_PARAMS;
	}

	this->Log(NULL, ZFqt_Log_Header_Info, ZFqt::E_LogLevel_Info,
		"[set module log file] begin: module[%s] ...\n",
		qstrModule.toStdString().c_str());

	QString	qstrModuleLogDir(qstrModuleLogDirParam);
	if (qstrModuleLogDir.isEmpty() || (qstrModuleLogDir == "."))
	{
		qstrModuleLogDir	=	QDir::toNativeSeparators(QDir::currentPath());
	}
	else if (qstrModuleLogDir == "..")
	{
		qstrModuleLogDir	=	QDir::toNativeSeparators(QDir::currentPath());
		qsizetype	lenModuleLogDir = qstrModuleLogDir.length();
		qstrModuleLogDir.resize(lenModuleLogDir - QDir::current().dirName().length() - 1);
	}

	int32_t	nErrno	=	ZFqt::E_Errno_ERR_GENERAL;
	QString	qstrErrMsg;

	QWaitCondition	cond;
	QMutex			mutex;
	QMutexLocker	lck(&mutex);

	emit this->signal_DoSetModuleLogFile(
		&cond, &mutex,
		&nErrno, &qstrErrMsg,
		qstrModule, qstrModuleLogDir, qstrModuleLogFile, bTruncate);

	cond.wait(&mutex);

	if (nErrno == ZFqt::E_Errno_SUCCESS)
	{
		this->Log(NULL, ZFqt_Log_Header_Info, ZFqt::E_LogLevel_Info,
			"[set module log file] successfully: Module[%s], ModuleLogFile[%s%c%s] ...\n",
			qstrModule.toStdString().c_str(),
			qstrModuleLogDir.toStdString().c_str(), QDir::separator().toLatin1(), qstrModuleLogFile.toStdString().c_str());
	}
	else
	{
		this->Log(NULL, ZFqt_Log_Header_Info, ZFqt::E_LogLevel_Warn,
			"[set module log file] failed: Errno[%d], ErrMsg[\"%s\"], Module[%s], ModuleLogFile[%s%c%s] !!!\n",
			nErrno, qstrErrMsg.toStdString().c_str(),
			qstrModule.toStdString().c_str(), qstrModuleLogDir.toStdString().c_str(), QDir::separator().toLatin1(), qstrModuleLogFile.toStdString().c_str());
	}

	return nErrno;
}

int32_t	ZFqt::LogMgr::Log(const char* pstrModule, const char* pstrSourceFile, int32_t nLineNo,
	ZFqt::ET_LogLevel eLogLevel,
	const char* pcFormat, ...
)
{
	// check log level
	if (eLogLevel > this->m_eLogLevelMax)
	{ // no need log on this level
		return ZFqt::E_Errno_SUCCESS;
	}

	if (ZFqt::IsZSTR(pstrModule))
	{
		pstrModule	=	"main";
	}

	int32_t	nErrno	=	ZFqt::E_Errno_ERR_GENERAL;

	char*	pcLog	=	NULL;

	for (int32_t iOnce = 0; iOnce < 1; ++iOnce)
	{
		std::string	strNow	=	QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz").toStdString();

		int32_t	lenHeaderInfo	=	::snprintf(NULL, 0,
			"[%s] %s [%s] %s:%d ",
			pstrModule,
			strNow.c_str(),
			ZFqt::GetLogLevelString(eLogLevel),
			pstrSourceFile, nLineNo);

		int32_t	lenBodyInfo	=	0;
		va_list	args;
		va_start(args, pcFormat);
		lenBodyInfo =	::vsnprintf(NULL, 0, pcFormat, args);
		va_end(args);

		pcLog	=	new char[lenHeaderInfo + lenBodyInfo + 1];
		if (pcLog == NULL)
		{
			fprintf(stderr, "[write log] failed: no enough memory !!!\n");

			nErrno	=	ZFqt::E_Errno_ERR_NO_MORE_MEM;
			break;
		}

		::snprintf(pcLog, lenHeaderInfo,
			"[%s] %s [%s] %s:%d ",
			pstrModule,
			strNow.c_str(),
			ZFqt::GetLogLevelString(eLogLevel),
			pstrSourceFile, nLineNo);

		va_start(args, pcFormat);
		lenBodyInfo =	::vsnprintf(pcLog + lenHeaderInfo, lenBodyInfo + 1, pcFormat, args);
		va_end(args);

        ZFqt_SPLogItem	spLogItem(new ZFqt::TLogItem(eLogLevel, pcLog, lenHeaderInfo + lenBodyInfo));
		emit this->signal_WriteLog(pstrModule, spLogItem);

		nErrno	=	ZFqt::E_Errno_SUCCESS;
	}
	if (nErrno != ZFqt::E_Errno_SUCCESS)
	{
		if (pcLog != NULL)
		{
			delete [] pcLog;
			pcLog	=	NULL;
		}
	}

	return nErrno;
}

void	ZFqt::LogMgr::LogToTextCtrl(QTextEdit* pTextEdit, ZFqt_SPLogItem spLogItem)
{
	if (pTextEdit != nullptr)
	{
		QColor	fcOld = pTextEdit->textColor();

		switch (spLogItem->eLogLevel)
		{
		case ZFqt::E_LogLevel_Error:
		{
			pTextEdit->setTextColor(Qt::red);
			pTextEdit->insertPlainText(spLogItem->pcLog);
			break;
		}
		case ZFqt::E_LogLevel_Warn:
		{
			pTextEdit->setTextColor(Qt::darkMagenta);
			pTextEdit->insertPlainText(spLogItem->pcLog);
			break;
		}
		case ZFqt::E_LogLevel_Info:
		{
			pTextEdit->setTextColor(Qt::darkGreen);
			pTextEdit->insertPlainText(spLogItem->pcLog);
			break;
		}
		case ZFqt::E_LogLevel_Debug:
		{
			pTextEdit->setTextColor(Qt::darkYellow);
			pTextEdit->insertPlainText(spLogItem->pcLog);
			break;
		}
		case ZFqt::E_LogLevel_Trace:
		{
			pTextEdit->setTextColor(Qt::darkYellow);
			pTextEdit->insertPlainText(spLogItem->pcLog);
			break;
		}
		default:
		{
			//do-nothing
			break;
		}
		}

		pTextEdit->setTextColor(fcOld);
		pTextEdit->moveCursor(QTextCursor::End);
	}
}

		// switch thread to handle log event
void	ZFqt::LogMgr::slot_DoInit(
	void* pvCond, void* pvMutex,		// QWaitCondition, QMutex
	void* pvnErrno, void* pvqstrErrMsg,	// Errno, ErrMsg
	void* pveLogLevelMax,				// LogLevelMax
	QString qstrLogDir, QString qstrLogFile, QString qstrLogLevelMax, bool bTruncate)
{
	QWaitCondition*	pCond	=	(QWaitCondition*)pvCond;
	QMutex*			pMutex	=	(QMutex*)pvMutex;

	int32_t*	pnErrno		=	(int32_t*)pvnErrno;
	QString*	pqstrErrMsg	=	(QString*)pvqstrErrMsg;

	ZFqt::ET_LogLevel*	peLogLevelMax	=	(ZFqt::ET_LogLevel*)pveLogLevelMax;

	QMutexLocker	lck(pMutex);

	for (int32_t iOnce = 0; iOnce < 1; ++iOnce)
	{
		ZFqt::ET_LogLevel	eLogLevelMax = ZFqt::GetLogLevel(qstrLogLevelMax.toStdString().c_str());
		if (eLogLevelMax == ZFqt::E_LogLevel_None)
		{ //no need to open log module
			*pnErrno		=	ZFqt::E_Errno_SUCCESS;
			*pqstrErrMsg	=	"log level is [NONE] or unkown";

			break;
		}

		QDir	qdir(qstrLogDir);
		if (!qdir.exists())
		{
			qdir.mkpath(qstrLogDir);
		}

		QString	qstrLogFilePath	=	qstrLogDir + QDir::separator() + qstrLogFile;
		this->m_mainFileLog.setFileName(qstrLogFilePath);
		//if (!this->m_fileLog.open(!bTruncate ? QIODevice::Append : QIODevice::Truncate))
		if (!this->m_mainFileLog.open(QIODevice::Append))
		{
			*pnErrno		=	ZFqt::E_Errno_ERR_NO_MORE_RES;
			*pqstrErrMsg	=	"failed to open main log file";

			break;
		}
		//failed to use QIODevice::Truncate mode for log file, USE replaced solution
		if (bTruncate)
		{
			this->m_mainFileLog.resize(0);
		}

		*peLogLevelMax	=	eLogLevelMax;

		*pnErrno		=	ZFqt::E_Errno_SUCCESS;
		*pqstrErrMsg	=	"successfully";
	}
	if (*pnErrno != ZFqt::E_Errno_SUCCESS)
	{
		this->Fini();
	}

	pCond->wakeOne();
}

void	ZFqt::LogMgr::slot_DoFini(
	void* pvCond, void* pvMutex,		// QWaitCondition, QMutex
	void* pvnErrno, void* pvqstrErrMsg)	// Errno, ErrMsg
{
	QWaitCondition*	pCond	=	(QWaitCondition*)pvCond;
	QMutex*			pMutex	=	(QMutex*)pvMutex;

	int32_t*	pnErrno		=	(int32_t*)pvnErrno;
	QString*	pqstrErrMsg	=	(QString*)pvqstrErrMsg;

	QMutexLocker	lck(pMutex);

	// close main log file
	if (this->m_mainFileLog.isOpen())
	{
		this->m_mainFileLog.close();
	}

	// close module log files
	std::map< QString, QFile* >::iterator	iterMapModuleLogFiles;
	for (iterMapModuleLogFiles = this->m_mapModuleLogFiles.begin(); iterMapModuleLogFiles != this->m_mapModuleLogFiles.end(); ++iterMapModuleLogFiles)
	{
		if (iterMapModuleLogFiles->second != NULL)
		{
			iterMapModuleLogFiles->second->close();

			delete iterMapModuleLogFiles->second;
			iterMapModuleLogFiles->second	=	NULL;
		}
	}
	this->m_mapModuleLogFiles.clear();

	*pnErrno		=	ZFqt::E_Errno_SUCCESS;
	*pqstrErrMsg	=	"successfully";

	pCond->wakeOne();
}

void	ZFqt::LogMgr::slot_DoSetModuleLogFile(
	void* pvCond, void* pvMutex,		// QWaitCondition, QMutex
	void* pvnErrno, void* pvqstrErrMsg,	// Errno, ErrMsg
	QString qstrModule, QString qstrModuleLogDir, QString qstrModuleLogFile, bool bTruncate)
{
	QWaitCondition*	pCond	=	(QWaitCondition*)pvCond;
	QMutex*			pMutex	=	(QMutex*)pvMutex;

	int32_t*	pnErrno		=	(int32_t*)pvnErrno;
	QString*	pqstrErrMsg	=	(QString*)pvqstrErrMsg;

	QMutexLocker	lck(pMutex);

	// check and close module log file if it already opened
	std::map< QString, QFile* >::iterator	iterMapModuleLogFiles =
		this->m_mapModuleLogFiles.find(qstrModule);
	if (iterMapModuleLogFiles != this->m_mapModuleLogFiles.end())
	{
		iterMapModuleLogFiles->second->close();

		delete iterMapModuleLogFiles->second;
		iterMapModuleLogFiles->second	=	NULL;

		this->m_mapModuleLogFiles.erase(iterMapModuleLogFiles);
	}

	QFile*	pfileModuleLog	=	NULL;
	for (int32_t iOnce = 0; iOnce < 1; ++iOnce)
	{
		pfileModuleLog	=	new QFile;
		if (pfileModuleLog == NULL)
		{
			*pnErrno		=	ZFqt::E_Errno_ERR_NO_MORE_RES;
			*pqstrErrMsg	=	"failed to new module log file object";

			break;
		}

		if (!qstrModuleLogDir.isEmpty() && !qstrModuleLogFile.isEmpty())
		{
			QDir	qdir(qstrModuleLogDir);
			if (!qdir.exists())
			{
				qdir.mkpath(qstrModuleLogDir);
			}

			QString	qstrLogFilePath = qstrModuleLogDir + QDir::separator() + qstrModuleLogFile;
			pfileModuleLog->setFileName(qstrLogFilePath);
			//if (!pfileModuleLog->open(!bTruncate ? QIODevice::Append : QIODevice::Truncate))
			if (!pfileModuleLog->open(QIODevice::Append))
			{
				*pnErrno		=	ZFqt::E_Errno_ERR_NO_MORE_RES;
				*pqstrErrMsg	=	"failed to open module log file";

				break;
			}
			//failed to use QIODevice::Truncate mode for log file, USE replaced solution
			if (bTruncate)
			{
				pfileModuleLog->resize(0);
			}
		}

		this->m_mapModuleLogFiles.insert(std::make_pair(qstrModule, pfileModuleLog));
		pfileModuleLog	=	NULL;

		*pnErrno		=	ZFqt::E_Errno_SUCCESS;
		*pqstrErrMsg	=	"successfully";

	}
	if (*pnErrno != ZFqt::E_Errno_SUCCESS)
	{
		if (pfileModuleLog != NULL)
		{
			delete pfileModuleLog;
			pfileModuleLog	=	NULL;
		}
	}

	pCond->wakeOne();
}

void	ZFqt::LogMgr::slot_WriteLog(QString qstrModule, ZFqt_SPLogItem spLogItem)
{
	QFile*	pfileLog	=	&this->m_mainFileLog;

	if (qstrModule != "main")
	{
		std::map< QString, QFile* >::iterator	iterMapModuleLogFiles =
			this->m_mapModuleLogFiles.find(qstrModule);
		if (iterMapModuleLogFiles != this->m_mapModuleLogFiles.end())
		{
			pfileLog	=	iterMapModuleLogFiles->second;
		}
	}

	if (pfileLog->isOpen())
	{
		pfileLog->write(spLogItem->pcLog, spLogItem->lenLog);
		pfileLog->flush();
	}
}

// async event handler
void	ZFqt::LogMgr::slot_OnThreadStarted()
{
	QMutexLocker	lck(&this->m_mutexThread);

	this->m_condThread.wakeOne();
}

