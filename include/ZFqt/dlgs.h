
#ifndef ZFqt_DLGS_H
#define ZFqt_DLGS_H

#include "ZFqt/types.h"

namespace ZFqt
{
	ZFqt_API	int32_t GetInput(const QString& qstrTitle, ZFqt_TVecNVItems& vecNVItems);

	ZFqt_API	void	ShowInformationMsgBox(const QString& qstrTitle, const QString& qstrMsg);
	ZFqt_API	void	ShowWarningMsgBox(const QString& qstrTitle, const QString& qstrMsg);
	ZFqt_API	void	ShowCriticalMsgBox(const QString& qstrTitle, const QString& qstrMsg);
		// Yes - ZFqt::E_Errno_SUCCESS, No - else
	ZFqt_API	int32_t	ExecQuestionMsgBox(const QString& qstrTitle, const QString& qstrMsg);
};

#endif //end ZFqt_DLGS_H

