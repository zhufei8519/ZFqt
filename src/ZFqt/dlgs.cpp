
#include "ZFqt/dlgs.h"
#include "ZFqt/locale.h"
#include "ZFqt/threaded/log.h"

#include <QGridLayout>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QCompleter>

#include <QMessageBox>

namespace ZFqt
{
	namespace Dlgs
	{
		class DlgInput : public QDialog
		{
			//Q_OBJECT

		public:
			DlgInput(const QString& qstrTitle, ZFqt_TVecNVItems& vecNVItems);
			virtual ~DlgInput();

			QString	GetValue(int nIndex);

		protected:
			QPushButton*	m_pPushButton_Ok;
			QPushButton*	m_pPushButton_Cancel;

			std::vector< QLineEdit* >	m_vecLineEdits;
		};
	};
};

ZFqt::Dlgs::DlgInput::DlgInput(const QString& qstrTitle, ZFqt_TVecNVItems& vecNVItems)
	:QDialog(nullptr)
{
	this->setWindowTitle(qstrTitle);

	QGridLayout*	pGridLayout_Main	=	new QGridLayout(this);
	this->setLayout(pGridLayout_Main);

	int	nCntItems	=	(int)vecNVItems.size();
	for (int i=0; i<nCntItems; ++i)
	{
		QLabel*		pLabel_Name = new QLabel(vecNVItems[i].qstrName, this);
		QLineEdit*	pLineEdit_Value = new QLineEdit(vecNVItems[i].qstrValue, this);
		if (vecNVItems[i].bIsPassword)
			pLineEdit_Value->setEchoMode(QLineEdit::Password);
		if (vecNVItems[i].bIsReadOnly)
		{
			pLineEdit_Value->setEnabled(false);
		}

		pGridLayout_Main->addWidget(pLabel_Name, i, 0);
		pGridLayout_Main->addWidget(pLineEdit_Value, i, 1);

		this->m_vecLineEdits.push_back(pLineEdit_Value);
	}

	this->m_pPushButton_Ok		=	new QPushButton(ZFqt_T("Ok"), this);
	this->m_pPushButton_Cancel	=	new QPushButton(ZFqt_T("Cancel"), this);
	pGridLayout_Main->addWidget(this->m_pPushButton_Ok, nCntItems, 1);
	pGridLayout_Main->addWidget(this->m_pPushButton_Cancel, nCntItems, 0);

	connect(this->m_pPushButton_Ok, &QPushButton::clicked, this, &QDialog::accept);
	connect(this->m_pPushButton_Cancel, &QPushButton::clicked, this, &QDialog::reject);
}

ZFqt::Dlgs::DlgInput::~DlgInput()
{
	disconnect(this->m_pPushButton_Ok, &QPushButton::clicked, this, &QDialog::accept);
	disconnect(this->m_pPushButton_Cancel, &QPushButton::clicked, this, &QDialog::reject);
}

QString	ZFqt::Dlgs::DlgInput::GetValue(int nIndex)
{
	if ((nIndex < 0) || (nIndex >= this->m_vecLineEdits.size()))
	{
		return "";
	}

	return this->m_vecLineEdits[nIndex]->text();
}

int32_t ZFqt::Dlgs::GetInput(const QString& qstrTitle, ZFqt_TVecNVItems& vecNVItems)
{
	int	nCntItems = (int)vecNVItems.size();

	if (nCntItems <= 0)
	{
		return ZFqt::E_Errno_ERR_INVALID_PARAMS;
	}

	ZFqt::Dlgs::DlgInput	dlgInput(qstrTitle, vecNVItems);
	if (dlgInput.exec() != QDialog::Accepted)
	{
		return ZFqt::E_Errno_ERR_GENERAL;
	}

	for (int i = 0; i < nCntItems; ++i)
	{
		vecNVItems[i].qstrValue = dlgInput.GetValue(i);
	}

	return ZFqt::E_Errno_SUCCESS;
}

int32_t ZFqt::Dlgs::GetLoginInput(ZFqt_TLoginInfo& loginInfo,
								const ZFqt_TVecLoginInfos& vecLoginInfos, const QString& qstrLastLogonAccount)
{
	loginInfo.Clear();

	int32_t	nRetCode	=	ZFqt::Dlgs::DlgLogin::E_RetCode_Cancel;

	ZFqt::Dlgs::DlgLogin	dlgLogin(vecLoginInfos, qstrLastLogonAccount);
	nRetCode	=	dlgLogin.exec();
	if (nRetCode == ZFqt::Dlgs::DlgLogin::E_RetCode_Login)
	{
		dlgLogin.GetCurLoginInfo(loginInfo);
	}

	return nRetCode;
}

		// Yes - ZFqt::E_Errno_SUCCESS, No - else
void	ZFqt::Dlgs::ShowInformationMsgBox(const QString& qstrTitle, const QString& qstrMsg)
{
	QMessageBox	msgBoxInfo(QMessageBox::Information, qstrTitle, qstrMsg, QMessageBox::Ok);
	msgBoxInfo.setButtonText(QMessageBox::Ok, ZFqt_T("Ok"));

	msgBoxInfo.exec();
}

void	ZFqt::Dlgs::ShowWarningMsgBox(const QString& qstrTitle, const QString& qstrMsg)
{
	QMessageBox	msgBoxWarning(QMessageBox::Warning, qstrTitle, qstrMsg, QMessageBox::Ok);
	msgBoxWarning.setButtonText(QMessageBox::Ok, ZFqt_T("Ok"));

	msgBoxWarning.exec();
}

void	ZFqt::Dlgs::ShowCriticalMsgBox(const QString& qstrTitle, const QString& qstrMsg)
{
	QMessageBox	msgBoxWarning(QMessageBox::Critical, qstrTitle, qstrMsg, QMessageBox::Ok);
	msgBoxWarning.setButtonText(QMessageBox::Ok, ZFqt_T("Ok"));

	msgBoxWarning.exec();
}

		// Yes - 0, No - else
int		ZFqt::Dlgs::ExecQuestionMsgBox(const QString& qstrTitle, const QString& qstrMsg)
{
	QMessageBox	msgBoxQuestion(QMessageBox::Question, qstrTitle, qstrMsg, QMessageBox::Yes | QMessageBox::No);
	msgBoxQuestion.setButtonText(QMessageBox::Yes, ZFqt_T("Yes"));
	msgBoxQuestion.setButtonText(QMessageBox::No, ZFqt_T("No"));

	return (msgBoxQuestion.exec() == QMessageBox::Yes) ? ZFqt::E_Errno_SUCCESS : ZFqt::E_Errno_ERR_GENERAL;
}

ZFqt::Dlgs::DlgLogin::DlgLogin(const ZFqt_TVecLoginInfos& vecLoginInfos, const QString& qstrLastLogonAccount)
	:m_vecLoginInfos(vecLoginInfos)
{
	this->setWindowTitle(ZFqt_T("User Login"));

	QGridLayout*	pGridLayout_Main	=	new QGridLayout(this);
	this->setLayout(pGridLayout_Main);

	int	nLineNo	=	0;

	QLabel*	pLabel_Account		=	new QLabel(ZFqt_T("Account"), this);
	this->m_pComboBox_Account	=	new QComboBox(this);
	pGridLayout_Main->addWidget(pLabel_Account, nLineNo, 1);
	pGridLayout_Main->addWidget(this->m_pComboBox_Account, nLineNo, 2);
	++ nLineNo;

	QLabel*	pLabel_Password		=	new QLabel(ZFqt_T("Password"), this);
	this->m_pLineEdit_Password	=	new QLineEdit(this);
	pGridLayout_Main->addWidget(pLabel_Password, nLineNo, 1);
	pGridLayout_Main->addWidget(this->m_pLineEdit_Password, nLineNo, 2);
	++ nLineNo;

	this->m_pCheckBox_SaveAccount	=	new QCheckBox(ZFqt_T("save account"), this);
	this->m_pCheckBox_SavePassword	=	new QCheckBox(ZFqt_T("save password"), this);
	this->m_pCheckBox_AutoLogin		=	new QCheckBox(ZFqt_T("auto login"), this);
	pGridLayout_Main->addWidget(this->m_pCheckBox_SaveAccount, nLineNo, 0);
	pGridLayout_Main->addWidget(this->m_pCheckBox_SavePassword, nLineNo, 1);
	pGridLayout_Main->addWidget(this->m_pCheckBox_AutoLogin, nLineNo, 2);
	++ nLineNo;

	this->m_pPushButton_CreateAccount	=	new QPushButton(ZFqt_T("create account"), this);
	pGridLayout_Main->addWidget(this->m_pPushButton_CreateAccount, nLineNo, 2);
	++ nLineNo;

	this->m_pPushButton_Login	=	new QPushButton(ZFqt_T("Login"), this);
	this->m_pPushButton_Cancel	=	new QPushButton(ZFqt_T("Cancel"), this);
	pGridLayout_Main->addWidget(this->m_pPushButton_Login, nLineNo, 2);
	pGridLayout_Main->addWidget(this->m_pPushButton_Cancel, nLineNo, 1);
	++ nLineNo;

	this->m_pPushButton_Login->setDefault(true);

	// prepare data
	QStringList	qslLogonAccounts;
	ZFqt_TVecLoginInfos::const_iterator	iterVecLoginInfos;
	for (iterVecLoginInfos = vecLoginInfos.begin(); iterVecLoginInfos != vecLoginInfos.end(); ++iterVecLoginInfos)
	{
		qslLogonAccounts << iterVecLoginInfos->qstrAccount;
	}

	// set control attributes
	this->m_pComboBox_Account->setEditable(true);
	this->m_pComboBox_Account->addItems(qslLogonAccounts);
		QCompleter*	pCompleter	=	new QCompleter(qslLogonAccounts, this);
		pCompleter->setCaseSensitivity(Qt::CaseInsensitive);
		this->m_pComboBox_Account->setCompleter(pCompleter);

	this->m_pLineEdit_Password->setEchoMode(QLineEdit::QLineEdit::Password);

	this->m_pCheckBox_SaveAccount->setCheckState(Qt::Checked);

	connect(this->m_pComboBox_Account, SIGNAL(editTextChanged(const QString&)), this, SLOT(slot_OnComboBoxTextChanged_Account(const QString&)));
	connect(this->m_pCheckBox_SaveAccount, SIGNAL(stateChanged(int)), this, SLOT(slot_OnCheckBoxStateChanged_SaveAccount(int)));
	connect(this->m_pCheckBox_SavePassword, SIGNAL(stateChanged(int)), this, SLOT(slot_OnCheckBoxStateChanged_SavePassword(int)));
	connect(this->m_pCheckBox_AutoLogin, SIGNAL(stateChanged(int)), this, SLOT(slot_OnCheckBoxStateChanged_AutoLogin(int)));

	connect(this->m_pPushButton_CreateAccount, &QPushButton::clicked, this, &ZFqt::Dlgs::DlgLogin::slot_OnPushButtonClicked_CreateAccount);

	connect(this->m_pPushButton_Login, &QPushButton::clicked, this, &ZFqt::Dlgs::DlgLogin::slot_OnPushButtonClicked_Login);
	connect(this->m_pPushButton_Cancel, &QPushButton::clicked, this, &ZFqt::Dlgs::DlgLogin::slot_onPushButtonClicked_Cancel);

	if (!qstrLastLogonAccount.isEmpty())
	{
		this->m_pLineEdit_Password->setFocus();
	}

	// prepare last logon data
	this->m_pComboBox_Account->setCurrentText(qstrLastLogonAccount);
	emit this->m_pComboBox_Account->editTextChanged(qstrLastLogonAccount);
}

ZFqt::Dlgs::DlgLogin::~DlgLogin()
{
	disconnect(this->m_pComboBox_Account, SIGNAL(editTextChanged(const QString&)), this, SLOT(slot_OnComboBoxTextChanged_Account(const QString&)));
	disconnect(this->m_pCheckBox_SaveAccount, SIGNAL(stateChanged(int)), this, SLOT(slot_OnCheckBoxStateChanged_SaveAccount(int)));
	disconnect(this->m_pCheckBox_SavePassword, SIGNAL(stateChanged(int)), this, SLOT(slot_OnCheckBoxStateChanged_SavePassword(int)));
	disconnect(this->m_pCheckBox_AutoLogin, SIGNAL(stateChanged(int)), this, SLOT(slot_OnCheckBoxStateChanged_AutoLogin(int)));

	disconnect(this->m_pPushButton_Login, &QPushButton::clicked, this, &QDialog::accept);
	disconnect(this->m_pPushButton_Cancel, &QPushButton::clicked, this, &QDialog::reject);
}

void	ZFqt::Dlgs::DlgLogin::GetCurLoginInfo(ZFqt_TLoginInfo& curLoginInfo) const
{
	curLoginInfo.Clear();

	curLoginInfo.qstrAccount	=	this->m_pComboBox_Account->currentText();
	curLoginInfo.qstrPassword	=	this->m_pLineEdit_Password->text();
	curLoginInfo.bSaveAccount	=	(this->m_pCheckBox_SaveAccount->checkState() == Qt::Checked) ? true : false;
	curLoginInfo.bSavePassword	=	(this->m_pCheckBox_SavePassword->checkState() == Qt::Checked) ? true : false;
	curLoginInfo.bAutoLogin		=	(this->m_pCheckBox_AutoLogin->checkState() == Qt::Checked) ? true : false;
}

void	ZFqt::Dlgs::DlgLogin::slot_OnComboBoxTextChanged_Account(const QString& text)
{
	// disconnect login controls' context event
	disconnect(this->m_pCheckBox_SaveAccount, SIGNAL(stateChanged(int)), this, SLOT(slot_OnCheckBoxStateChanged_SaveAccount(int)));
	disconnect(this->m_pCheckBox_SavePassword, SIGNAL(stateChanged(int)), this, SLOT(slot_OnCheckBoxStateChanged_SavePassword(int)));
	disconnect(this->m_pCheckBox_AutoLogin, SIGNAL(stateChanged(int)), this, SLOT(slot_OnCheckBoxStateChanged_AutoLogin(int)));

	int	nIndex	=	-1;
	int	nCntLoginInfos	=	(int)(this->m_vecLoginInfos.size());
	for (int i=0; i<nCntLoginInfos; ++i)
	{
		if (this->m_vecLoginInfos[i].qstrAccount == text)
		{
			nIndex	=	i;
			break;
		}
	}

	if (nIndex != -1)
	{
		this->m_pLineEdit_Password->setText(this->m_vecLoginInfos[nIndex].qstrPassword);

		this->m_pCheckBox_SaveAccount->setCheckState(this->m_vecLoginInfos[nIndex].bSaveAccount ? Qt::Checked : Qt::Unchecked);
		this->m_pCheckBox_SavePassword->setCheckState(this->m_vecLoginInfos[nIndex].bSavePassword ? Qt::Checked : Qt::Unchecked);
		this->m_pCheckBox_AutoLogin->setCheckState(this->m_vecLoginInfos[nIndex].bAutoLogin ? Qt::Checked : Qt::Unchecked);
	}
	else
	{
		this->m_pLineEdit_Password->setText("");
	}

	// reconnect login controls' context event
	connect(this->m_pCheckBox_SaveAccount, SIGNAL(stateChanged(int)), this, SLOT(slot_OnCheckBoxStateChanged_SaveAccount(int)));
	connect(this->m_pCheckBox_SavePassword, SIGNAL(stateChanged(int)), this, SLOT(slot_OnCheckBoxStateChanged_SavePassword(int)));
	connect(this->m_pCheckBox_AutoLogin, SIGNAL(stateChanged(int)), this, SLOT(slot_OnCheckBoxStateChanged_AutoLogin(int)));
}

void	ZFqt::Dlgs::DlgLogin::slot_OnCheckBoxStateChanged_SaveAccount(int state)
{
	// disconnect login controls' context event
	disconnect(this->m_pCheckBox_SaveAccount, SIGNAL(stateChanged(int)), this, SLOT(slot_OnCheckBoxStateChanged_SaveAccount(int)));
	disconnect(this->m_pCheckBox_SavePassword, SIGNAL(stateChanged(int)), this, SLOT(slot_OnCheckBoxStateChanged_SavePassword(int)));
	disconnect(this->m_pCheckBox_AutoLogin, SIGNAL(stateChanged(int)), this, SLOT(slot_OnCheckBoxStateChanged_AutoLogin(int)));

	if (state == Qt::Checked)
	{
		//do-nothing
	}
	else
	{
		this->m_pCheckBox_SavePassword->setCheckState(Qt::Unchecked);
		this->m_pCheckBox_AutoLogin->setCheckState(Qt::Unchecked);
	}

	// reconnect login controls' context event
	connect(this->m_pCheckBox_SaveAccount, SIGNAL(stateChanged(int)), this, SLOT(slot_OnCheckBoxStateChanged_SaveAccount(int)));
	connect(this->m_pCheckBox_SavePassword, SIGNAL(stateChanged(int)), this, SLOT(slot_OnCheckBoxStateChanged_SavePassword(int)));
	connect(this->m_pCheckBox_AutoLogin, SIGNAL(stateChanged(int)), this, SLOT(slot_OnCheckBoxStateChanged_AutoLogin(int)));
}

void	ZFqt::Dlgs::DlgLogin::slot_OnCheckBoxStateChanged_SavePassword(int state)
{
	// disconnect login controls' context event
	disconnect(this->m_pCheckBox_SaveAccount, SIGNAL(stateChanged(int)), this, SLOT(slot_OnCheckBoxStateChanged_SaveAccount(int)));
	disconnect(this->m_pCheckBox_SavePassword, SIGNAL(stateChanged(int)), this, SLOT(slot_OnCheckBoxStateChanged_SavePassword(int)));
	disconnect(this->m_pCheckBox_AutoLogin, SIGNAL(stateChanged(int)), this, SLOT(slot_OnCheckBoxStateChanged_AutoLogin(int)));

	if (state == Qt::Checked)
	{
		this->m_pCheckBox_SaveAccount->setCheckState(Qt::Checked);
	}
	else
	{
		this->m_pCheckBox_AutoLogin->setCheckState(Qt::Unchecked);
	}

	// reconnect login controls' context event
	connect(this->m_pCheckBox_SaveAccount, SIGNAL(stateChanged(int)), this, SLOT(slot_OnCheckBoxStateChanged_SaveAccount(int)));
	connect(this->m_pCheckBox_SavePassword, SIGNAL(stateChanged(int)), this, SLOT(slot_OnCheckBoxStateChanged_SavePassword(int)));
	connect(this->m_pCheckBox_AutoLogin, SIGNAL(stateChanged(int)), this, SLOT(slot_OnCheckBoxStateChanged_AutoLogin(int)));
}

void	ZFqt::Dlgs::DlgLogin::slot_OnCheckBoxStateChanged_AutoLogin(int state)
{
	// disconnect login controls' context event
	disconnect(this->m_pCheckBox_SaveAccount, SIGNAL(stateChanged(int)), this, SLOT(slot_OnCheckBoxStateChanged_SaveAccount(int)));
	disconnect(this->m_pCheckBox_SavePassword, SIGNAL(stateChanged(int)), this, SLOT(slot_OnCheckBoxStateChanged_SavePassword(int)));
	disconnect(this->m_pCheckBox_AutoLogin, SIGNAL(stateChanged(int)), this, SLOT(slot_OnCheckBoxStateChanged_AutoLogin(int)));

	if (state == Qt::Checked)
	{
		this->m_pCheckBox_SaveAccount->setCheckState(Qt::Checked);
		this->m_pCheckBox_SaveAccount->setCheckState(Qt::Checked);
	}
	else
	{
		//do-nothing
	}

	// reconnect login controls' context event
	connect(this->m_pCheckBox_SaveAccount, SIGNAL(stateChanged(int)), this, SLOT(slot_OnCheckBoxStateChanged_SaveAccount(int)));
	connect(this->m_pCheckBox_SavePassword, SIGNAL(stateChanged(int)), this, SLOT(slot_OnCheckBoxStateChanged_SavePassword(int)));
	connect(this->m_pCheckBox_AutoLogin, SIGNAL(stateChanged(int)), this, SLOT(slot_OnCheckBoxStateChanged_AutoLogin(int)));
}

void	ZFqt::Dlgs::DlgLogin::slot_OnPushButtonClicked_CreateAccount()
{
	QDialog::done(ZFqt::Dlgs::DlgLogin::E_RetCode_CreateAccount);
}

void	ZFqt::Dlgs::DlgLogin::slot_OnPushButtonClicked_Login()
{
	if (this->m_pComboBox_Account->currentText().isEmpty())
	{
		ZFqt::Dlgs::ShowWarningMsgBox(ZFqt_T("User Login"), ZFqt_T("account cannot be EMPTY"));

		this->m_pComboBox_Account->setFocus();
	}
	else if (this->m_pLineEdit_Password->text().isEmpty())
	{
		ZFqt::Dlgs::ShowWarningMsgBox(ZFqt_T("User Login"), ZFqt_T("password cannot be EMPTY"));

		this->m_pLineEdit_Password->setFocus();
	}
	else
	{
		QDialog::done(ZFqt::Dlgs::DlgLogin::E_RetCode_Login);
	}
}

void	ZFqt::Dlgs::DlgLogin::slot_onPushButtonClicked_Cancel()
{
	QDialog::done(ZFqt::Dlgs::DlgLogin::E_RetCode_Cancel);
}

