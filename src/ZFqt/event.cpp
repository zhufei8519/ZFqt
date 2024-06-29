
#include "ZFqt/event.h"

void    ZFqt::PostQuitEvent(bool bNeedConfirm)
{
    emit ZFqt::EventMgr::Instance()->signal_Quit(bNeedConfirm);
}

ZFqt::EventMgr* ZFqt::EventMgr::m_spInstance = NULL;

