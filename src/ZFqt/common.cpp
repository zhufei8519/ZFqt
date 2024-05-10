
#include "ZFqt/common.h"
#include <QApplication>

namespace ZFqt
{
	static volatile bool	s_bIsMainWindowCreated	=	false;
};

bool	ZFqt::IsZSTR(const char* pstr)
{
	if (pstr == NULL)
	{
		return true;
	}

	if (::strlen(pstr) == 0)
	{
		return true;
	}

	return false;
}

bool	ZFqt::IsMainWindowCreated()
{
	if (!ZFqt::s_bIsMainWindowCreated)
	{
		QWidgetList	listWidgets = QApplication::topLevelWidgets();
		if (!listWidgets.isEmpty())
		{
			ZFqt::s_bIsMainWindowCreated	=	true;
		}
	}

	return ZFqt::s_bIsMainWindowCreated;
}

