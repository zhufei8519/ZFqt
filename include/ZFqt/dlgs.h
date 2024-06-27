
#ifndef ZFqt_DLGS_H
#define ZFqt_DLGS_H

#include "ZFqt/types.h"
#include <QDialog>

class QComboBox;
class QLineEdit;
class QCheckBox;
class QPushButton;
namespace ZFqt
{
	namespace Dlgs
	{
		ZFqt_API	int32_t GetInput(const QString& qstrTitle, ZFqt_TVecNVItems& vecNVItems);
		ZFqt_API	int32_t	GetLoginInput(ZFqt_TLoginInfo& loginInfo,
								const ZFqt_TVecLoginInfos& vecLoginInfos, const QString& qstrLastLogonAccount);

		ZFqt_API	void	ShowInformationMsgBox(const QString& qstrTitle, const QString& qstrMsg);
		ZFqt_API	void	ShowWarningMsgBox(const QString& qstrTitle, const QString& qstrMsg);
		ZFqt_API	void	ShowCriticalMsgBox(const QString& qstrTitle, const QString& qstrMsg);
			// Yes - ZFqt::E_Errno_SUCCESS, No - else
		ZFqt_API	int32_t	ExecQuestionMsgBox(const QString& qstrTitle, const QString& qstrMsg);

		class ZFqt_API DlgLogin: public QDialog
		{
			Q_OBJECT

		public:
			DlgLogin(const ZFqt_TVecLoginInfos& vecLoginInfos, const QString& qstrLastLogonAccount);
			~DlgLogin();

			void	GetCurLoginInfo(ZFqt_TLoginInfo& curLoginInfo) const;

		enum ET_RetCode
		{
			E_RetCode_CreateAccount	=	101,
			E_RetCode_Login			=	102,
			E_RetCode_Cancel		=	103,
		};

		protected:
			QPushButton*	m_pPushButton_Login;
			QPushButton*	m_pPushButton_Cancel;

			QComboBox*		m_pComboBox_Account;
			QLineEdit*		m_pLineEdit_Password;

			QCheckBox*		m_pCheckBox_SaveAccount;
			QCheckBox*		m_pCheckBox_SavePassword;
			QCheckBox*		m_pCheckBox_AutoLogin;

			QPushButton*	m_pPushButton_CreateAccount;

			const ZFqt_TVecLoginInfos&	m_vecLoginInfos;

		protected slots:
			void	slot_OnComboBoxTextChanged_Account(const QString& text);

			void	slot_OnCheckBoxStateChanged_SaveAccount(int state);
			void	slot_OnCheckBoxStateChanged_SavePassword(int state);
			void	slot_OnCheckBoxStateChanged_AutoLogin(int state);

			void	slot_OnPushButtonClicked_CreateAccount();

			void	slot_OnPushButtonClicked_Login();
			void	slot_onPushButtonClicked_Cancel();
		};
	};
};

#endif //end ZFqt_DLGS_H

