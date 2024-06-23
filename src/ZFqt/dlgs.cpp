
#include "ZFqt/dlgs.h"
#include "ZFqt/locale.h"

#include <QDialog>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

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

	QGridLayout*	pGridLayout_Main	=	new QGridLayout();
	this->setLayout(pGridLayout_Main);

	int	nCntItems	=	(int)vecNVItems.size();
	for (int i=0; i<nCntItems; ++i)
	{
		QLabel*		pLabel_Name = new QLabel(vecNVItems[i].qstrName);
		QLineEdit*	pLineEdit_Value = new QLineEdit(vecNVItems[i].qstrValue);
		if (vecNVItems[i].bIsPassword)
			pLineEdit_Value->setEchoMode(QLineEdit::Password);

		pGridLayout_Main->addWidget(pLabel_Name, i, 0);
		pGridLayout_Main->addWidget(pLineEdit_Value, i, 1);

		this->m_vecLineEdits.push_back(pLineEdit_Value);
	}

	this->m_pPushButton_Ok		=	new QPushButton(ZFqt_T("Ok"));
	this->m_pPushButton_Cancel	=	new QPushButton(ZFqt_T("Cancel"));
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

