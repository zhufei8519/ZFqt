
#include "ZFqt/dlgs.h"
#include "ZFqt/locale.h"
#include <QMessageBox>

int32_t ZFqt::GetInput(const QString& qstrTitle, ZFqt_TVecNVItems& vecNVItems)
{
    int32_t nErrno  =   ZFqt::E_Errno_ERR_GENERAL;

    //TODO

    return nErrno;
}

		// Yes - ZFqt::E_Errno_SUCCESS, No - else
void	ZFqt::ShowInformationMsgBox(const QString& qstrTitle, const QString& qstrMsg)
{
	QMessageBox	msgBoxInfo(QMessageBox::Information, qstrTitle, qstrMsg, QMessageBox::Ok);
	msgBoxInfo.setButtonText(QMessageBox::Ok, ZFqt_T("Ok"));

	msgBoxInfo.exec();
}

void	ZFqt::ShowWarningMsgBox(const QString& qstrTitle, const QString& qstrMsg)
{
	QMessageBox	msgBoxWarning(QMessageBox::Warning, qstrTitle, qstrMsg, QMessageBox::Ok);
	msgBoxWarning.setButtonText(QMessageBox::Ok, ZFqt_T("Ok"));

	msgBoxWarning.exec();
}

void	ZFqt::ShowCriticalMsgBox(const QString& qstrTitle, const QString& qstrMsg)
{
	QMessageBox	msgBoxWarning(QMessageBox::Critical, qstrTitle, qstrMsg, QMessageBox::Ok);
	msgBoxWarning.setButtonText(QMessageBox::Ok, ZFqt_T("Ok"));

	msgBoxWarning.exec();
}

		// Yes - 0, No - else
int		ZFqt::ExecQuestionMsgBox(const QString& qstrTitle, const QString& qstrMsg)
{
	QMessageBox	msgBoxQuestion(QMessageBox::Question, qstrTitle, qstrMsg, QMessageBox::Yes | QMessageBox::No);
	msgBoxQuestion.setButtonText(QMessageBox::Yes, ZFqt_T("Yes"));
	msgBoxQuestion.setButtonText(QMessageBox::No, ZFqt_T("No"));

	return (msgBoxQuestion.exec() == QMessageBox::Yes) ? ZFqt::E_Errno_SUCCESS : ZFqt::E_Errno_ERR_GENERAL;
}

