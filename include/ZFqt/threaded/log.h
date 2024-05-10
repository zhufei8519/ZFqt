
#ifndef ZFqt_LOG_H
#define ZFqt_LOG_H

#include "ZFqt/types.h"
#include <map>
#include <QFile>
class QTextEdit;

#define ZFqt_Log_Header_Info	__FILE__, __LINE__

namespace ZFqt
{
	enum ET_LogLevel
	{
		E_LogLevel_Error = 10,
		E_LogLevel_Warn = 20,
		E_LogLevel_Info = 30,
		E_LogLevel_Debug = 40,
		E_LogLevel_Trace = 50,
		E_LogLevel_None = 99,
	};

	struct TLogItem
	{
		ET_LogLevel	eLogLevel;
		char*		pcLog;
		int32_t		lenLog;

		TLogItem()
			:eLogLevel(ZFqt::E_LogLevel_None), pcLog(NULL), lenLog(0)
		{}
		TLogItem(ZFqt::ET_LogLevel eLogLevelParam, char* pcLogParam, int32_t lenLogParam)
			:eLogLevel(eLogLevelParam), pcLog(pcLogParam), lenLog(lenLogParam)
		{}
		~TLogItem()
		{
			eLogLevel	=	ZFqt::E_LogLevel_None;
			if (pcLog != NULL) { delete[] pcLog; pcLog = NULL; }
			lenLog = 0;
		}
	};

	// private functions
	ZFqt_Export ET_LogLevel	GetLogLevel(const char* pstrLogLevel);
	ZFqt_Export const char*	GetLogLevelString(ZFqt::ET_LogLevel eLogLevel);
};
typedef std::shared_ptr< ZFqt::TLogItem >	ZFqt_SPLogItem;

namespace ZFqt
{
	class ZFqt_Export LogMgr : public QObject
	{
		Q_OBJECT

	public:
		LogMgr();
		~LogMgr();

		int32_t	Init(const QString& qstrLogDir, const QString& qstrLogFile, const QString& qstrLogLevelMax, bool bTruncate = false);
		int32_t	Fini();
		void	SetLogLevelMax(const QString& qstrLogLevelMax);

		int32_t	SetModuleLogFile(const QString& qstrModule, const QString& qstrModuleLogDir, const QString& qstrModuleLogFile, bool bTruncate = false);

		int32_t	Log(const char* pstrModule, const char* pstrSourceFile, int32_t nLineNo,
			ET_LogLevel eLogLevel,
			const char* pcFormat, ...);

		static void	LogToTextCtrl(QTextEdit* pTextEdit, ZFqt_SPLogItem spLogItem);

	signals:
		// async signal for log event
		void	signal_DoInit(
			void*, void*,	// QWaitCondition, QMutex
			void*, void*,	// Errno, ErrMsg
			void*,			// LogLevelMax
			QString, QString, QString, bool);
		void	signal_DoFini(
			void*, void*,	// QWaitCondition, QMutex
			void*, void*);	// Errno, ErrMsg
		void	signal_DoSetModuleLogFile(
			void*, void*,	// QWaitCondition, QMutex
			void*, void*,	// Errno, ErrMsg
			QString, QString, QString, bool);

        void	signal_WriteLog(QString, ZFqt_SPLogItem);

	private slots:
		// switch thread to handle log event
		void	slot_DoInit(
			void*, void*,	// QWaitCondition, QMutex
			void*, void*,	// Errno, ErrMsg
			void*,			// LogLevelMax
			QString, QString, QString, bool);
		void	slot_DoFini(
			void*, void*,	// QWaitCondition, QMutex
			void*, void*);	// Errno, ErrMsg
		void	slot_DoSetModuleLogFile(
			void*, void*,	// QWaitCondition, QMutex
			void*, void*,	// Errno, ErrMsg
			QString, QString, QString, bool);

        void	slot_WriteLog(QString, ZFqt_SPLogItem);

		// async event handler
		void	slot_OnThreadStarted();

	private:
		QThread*		m_pThread;
		QWaitCondition	m_condThread;
		QMutex			m_mutexThread;

		ET_LogLevel	m_eLogLevelMax;

		QFile		m_mainFileLog;
		std::map< QString, QFile* >	m_mapModuleLogFiles;

	public:
		static LogMgr*	Instance();
		static void		Destroy();

	private:
		static LogMgr*	m_spInstance;
	};
};

inline
ZFqt::LogMgr*	ZFqt::LogMgr::Instance()
{
    if (ZFqt::LogMgr::m_spInstance == NULL)
	{
		ZFqt::LogMgr::m_spInstance	=	new ZFqt::LogMgr;
	}

    return ZFqt::LogMgr::m_spInstance;
}

inline
void	ZFqt::LogMgr::Destroy()
{
    if (ZFqt::LogMgr::m_spInstance != NULL)
	{
        delete ZFqt::LogMgr::m_spInstance;
		ZFqt::LogMgr::m_spInstance	=	NULL;
	}
}

#endif //end ZFqt_LOG_H

