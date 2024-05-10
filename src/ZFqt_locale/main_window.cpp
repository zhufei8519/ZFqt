
#include "main_window.h"

#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QMessageBox>
#include <QToolButton>

	// NavMenu
#include <QDockWidget>

	// MainPanel
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QDateEdit>
#include <QLineEdit>
#include <QButtonGroup>
#include <QRadioButton>
#include <QPushButton>

	// Log
#include <QTextEdit>

#include <QDir>
#include <QFileDialog>
#include <QStandardPaths>

MainWindow::MainWindow(QWidget* parent)
	:QMainWindow(parent)
{
	this->CreateMainPanel();

	connect(this, SIGNAL(signal_OnCreateDialog()),
		this, SLOT(slot_OnCreateDialog()));

	connect(this, SIGNAL(signal_OnInitDialog()),
		this, SLOT(slot_OnInitDialog()));

	connect(ZFqt::LogMgr::Instance(), SIGNAL(signal_WriteLog(QString, ZFqt_SPLogItem)),
		this, SLOT(slot_WriteLog(QString, ZFqt_SPLogItem)));

	emit this->signal_OnCreateDialog();
}

MainWindow::~MainWindow()
{
}

void	MainWindow::slot_OnCreateDialog()
{
	ZFqt::LogMgr::Instance()->Init(
		ZFqt::App::Instance()->GetAppDataHome(),
		ZFqt::App::Instance()->GetAppName() + ".log",
#ifdef QT_NO_DEBUG
		"info"
#else //!QT_NO_DEBUG
		"trace"
#endif //end QT_NO_DEBUG + !QT_NO_DEBUG
	);

#ifndef QT_NO_DEBUG
	// Module log test
	ZFqt::LogMgr::Instance()->SetModuleLogFile("Module_test", ZFqt::App::Instance()->GetAppDataHome(), "Module_test.log");
	ZFqt::LogMgr::Instance()->Log("Module_test", ZFqt_Log_Header_Info, ZFqt::E_LogLevel_Error,
		"LogLevel [%s]\n",
		ZFqt::GetLogLevelString(ZFqt::E_LogLevel_Error));
	ZFqt::LogMgr::Instance()->Log("Module_test", ZFqt_Log_Header_Info, ZFqt::E_LogLevel_Warn,
		"LogLevel [%s]\n",
		ZFqt::GetLogLevelString(ZFqt::E_LogLevel_Warn));
	ZFqt::LogMgr::Instance()->Log("Module_test", ZFqt_Log_Header_Info, ZFqt::E_LogLevel_Info,
		"LogLevel [%s]\n",
		ZFqt::GetLogLevelString(ZFqt::E_LogLevel_Info));
	ZFqt::LogMgr::Instance()->Log("Module_test", ZFqt_Log_Header_Info, ZFqt::E_LogLevel_Debug,
		"LogLevel [%s]\n",
		ZFqt::GetLogLevelString(ZFqt::E_LogLevel_Debug));
	ZFqt::LogMgr::Instance()->Log("Module_test", ZFqt_Log_Header_Info, ZFqt::E_LogLevel_Trace,
		"LogLevel [%s]\n",
		ZFqt::GetLogLevelString(ZFqt::E_LogLevel_Trace));
#endif //end !QT_NO_DEBUG

	DBApp::Instance()->Open(ZFqt::App::Instance()->GetAppName(), ZFqt::App::Instance()->GetVendorEmail());
	ZFqt::Locale::Instance()->Open(ZFqt::App::Instance()->GetAppDataHome(), "locale", ZFqt::App::Instance()->GetVendorEmail());

	std::map< QString, QString >	mapSupportedLocales;
	std::map< QString, QString >::const_iterator	iterMapSupportedLocales;
	ZFqt::Locale::Instance()->GetSupportedLocales(mapSupportedLocales);
	ZFqt::LogMgr::Instance()->Log(NULL, ZFqt_Log_Header_Info, ZFqt::E_LogLevel_Debug,
		"Supported locales:\n");
	for (iterMapSupportedLocales = mapSupportedLocales.begin(); iterMapSupportedLocales != mapSupportedLocales.end(); ++iterMapSupportedLocales)
	{
		ZFqt::LogMgr::Instance()->Log(NULL, ZFqt_Log_Header_Info, ZFqt::E_LogLevel_Debug,
			"\t%s\t-\t%s\n",
			iterMapSupportedLocales->first.toStdString().c_str(), iterMapSupportedLocales->second.toStdString().c_str());
	}
	//ZFqt::Locale::Instance()->SetCurLocale("zh_CN");

	ZFqt::LogMgr::Instance()->Log(NULL, ZFqt_Log_Header_Info, ZFqt::E_LogLevel_Info,
		"Program[%s] started successfully .\n",
		ZFqt::App::Instance()->GetAppName().toStdString().c_str());

	emit this->signal_OnInitDialog();
}

void	MainWindow::slot_OnInitDialog()
{
	ZFqt::App::Instance()->SetAppDisplayName(ZFqt_T(ZFqt::App::Instance()->GetAppDisplayName()));
	this->setWindowTitle(ZFqt::App::Instance()->GetAppDisplayName());

	this->CreateMenuBar();
	this->CreateToolBar();
	//this->CreateMainPanel();
	this->m_pDockWidget_Log->setWindowTitle(ZFqt_T("Log Console"));
	this->CreateStatusBar();
	this->setMinimumSize(ZFqt::Const_MinWindowWidth, ZFqt::Const_MinWindowHeight);
	this->showMaximized();
}

void	MainWindow::slot_OnLogWindowClosed()
{
	this->OnToolBar_ShowOrHideLogWindow();
}

void	MainWindow::slot_WriteLog(QString /*qstrModule*/, ZFqt_SPLogItem spLogItem)
{
	ZFqt::LogMgr::Instance()->LogToTextCtrl(this->m_pTextEdit_Log, spLogItem);
}

void	MainWindow::closeEvent(QCloseEvent* /*pEvt*/)
{
	DBApp::Instance()->Close();
	ZFqt::Locale::Instance()->Close();

	ZFqt::LogMgr::Instance()->Fini();

	QWidget::close();
}

void	MainWindow::CreateMenuBar()
{
	const QIcon	iconFileExit = QIcon::fromTheme("file-exit", QIcon(":/res/images/exit.png"));

	//MenuBar
		//File
	QMenu*	pMenuFile	=	this->menuBar()->addMenu(ZFqt_T("&File"));
			//Exit
		this->m_pMenuItem_Exit = pMenuFile->addAction(iconFileExit, ZFqt_T("E&xit"), this, &MainWindow::OnMenuExit, QKeySequence(Qt::ALT + Qt::Key_F4));

		//Tools
	QMenu*	pMenuTools	=	this->menuBar()->addMenu(ZFqt_T("&Tools"));
			//GenerateLocaleTemplate
		pMenuTools->addAction(ZFqt_T("&Generate Locale Template"), this, &MainWindow::OnMenuGenerateLocaleTemplate);
			//ImportLocale
		pMenuTools->addAction(ZFqt_T("&Import Locale"), this, &MainWindow::OnMenuImportLocale);

		//Help
	QMenu*	pMenuHelp	=	this->menuBar()->addMenu(ZFqt_T("&Help"));
			//About
		pMenuHelp->addAction(ZFqt_T("&About"), this, &MainWindow::OnMenuAbout);
}

void	MainWindow::CreateToolBar()
{
	QToolBar*	pToolBar_LogInfo	=	this->addToolBar("Log");
		this->m_pToolButton_LogInfo = new QToolButton(this);
		this->m_pToolButton_LogInfo->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
		this->m_pToolButton_LogInfo->setText(ZFqt_T("hide log window"));
		this->m_pToolButton_LogInfo->setIcon(QIcon(":/res/images/log_off.png"));
		connect(this->m_pToolButton_LogInfo, &QToolButton::clicked, this, &MainWindow::OnToolBar_ShowOrHideLogWindow);
	pToolBar_LogInfo->addWidget(this->m_pToolButton_LogInfo);
}

void	MainWindow::CreateMainPanel()
{
	QWidget*	pCentralWidget = new QWidget();
	this->setCentralWidget(pCentralWidget);
	QVBoxLayout*	pVBoxLayout_Main = new QVBoxLayout();
	pCentralWidget->setLayout(pVBoxLayout_Main);

	this->m_pDockWidget_Log = new QDockWidget("Log Console", this);
	this->m_pDockWidget_Log->setAllowedAreas(Qt::BottomDockWidgetArea);
	this->m_pDockWidget_Log->setFeatures(QDockWidget::NoDockWidgetFeatures);
	this->m_pTextEdit_Log = new QTextEdit(this->m_pDockWidget_Log);
	this->m_pTextEdit_Log->setReadOnly(true);
	this->m_pTextEdit_Log->setFontPointSize(12);
	this->m_pDockWidget_Log->setWidget(this->m_pTextEdit_Log);
	this->addDockWidget(Qt::BottomDockWidgetArea, this->m_pDockWidget_Log);
}

void	MainWindow::CreateStatusBar()
{
	this->statusBar()->showMessage(ZFqt_T("Welcome to USE") + " " + ZFqt::App::Instance()->GetAppDisplayName());
}

	//MenuBar
		//File
			//Exit
void	MainWindow::OnMenuExit()
{
	QWidget::close();
}

		//Tools
			//GenerateLocaleTemplate
void	MainWindow::OnMenuGenerateLocaleTemplate()
{
	ZFqt::Locale::Instance()->GenerateLocaleTemplate(ZFqt::App::Instance()->GetAppDataHome() + QDir::separator() + "locale.en_US.xml");
}

			//ImportLocale
void	MainWindow::OnMenuImportLocale()
{
	QString	qstrLocale;

	QString	filePathLocale =
		QFileDialog::getOpenFileName(NULL, ZFqt_T("Import Locale"),
			ZFqt::App::Instance()->GetAppDataHome(),
			("XML files(*.xml)"));
	if (!filePathLocale.isEmpty())
	{
		qstrLocale	=	ZFqt::Locale::Instance()->ImportLocale(filePathLocale);
		if (!qstrLocale.isEmpty())
		{
			QMessageBox::information(NULL, ZFqt_T("Import Locale"), ZFqt_T("Import Locale") + ": " + ZFqt_T("successfully") + ", " + ZFqt_T("effected after restart") + ".");
		}
		else
		{
			QMessageBox::warning(NULL, ZFqt_T("Import Locale"), ZFqt_T("Import Locale") + ": " + ZFqt_T("failed") + "!!!");
		}
	}
}

		//Help
			//About
void	MainWindow::OnMenuAbout()
{
	std::string	strBuildDate(__DATE__);

	QMessageBox::information(NULL, QString("%1 %2 v%3").arg(ZFqt_T("About")).arg(ZFqt::App::Instance()->GetAppDisplayName()).arg(ZFqt::App::Instance()->GetAppVersion()),
		QString("%1\n"
			"%2 (C) %3~ [%4] <%5>\n"
			"%6").arg(ZFqt::App::Instance()->GetAppDisplayName())
			.arg(ZFqt_T("Copyright")).arg(strBuildDate.substr(7).c_str()).arg(ZFqt::App::Instance()->GetVendor()).arg(ZFqt::App::Instance()->GetVendorEmail())
			.arg(ZFqt_T("All rights reserved."))
		);
}

	//ToolBar
		//Log
			//ShowOrHideLogWindow
void	MainWindow::OnToolBar_ShowOrHideLogWindow()
{
	if (this->m_pDockWidget_Log->isVisible())
	{
		this->m_pDockWidget_Log->setVisible(false);

		this->m_pToolButton_LogInfo->setText(ZFqt_T("show log window"));
		this->m_pToolButton_LogInfo->setIcon(QIcon(":/res/images/log_on.png"));
	}
	else
	{
		this->m_pDockWidget_Log->setVisible(true);

		this->m_pToolButton_LogInfo->setText(ZFqt_T("hide log window"));
		this->m_pToolButton_LogInfo->setIcon(QIcon(":/res/images/log_off.png"));
	}
}

