
#include "ZFqt/event.h"

void    ZFqt::PostQuitEvent(const QString& qstrHintMsg)
{
    emit ZFqt::EventMgr::Instance()->signal_QuitEvent(qstrHintMsg);
}

ZFqt::EventMgr* ZFqt::EventMgr::m_spInstance = NULL;

