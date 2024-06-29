
#ifndef ZFqt_EVENT_H
#define ZFqt_EVENT_H

#include "ZFqt/export.h"
#include <QString>

namespace ZFqt
{
    class ZFqt_API EventMgr: public QObject
    {
        Q_OBJECT

    public:
        EventMgr()          {}
        virtual ~EventMgr() {}

    signals:
        void    signal_OnLocaleChanged(QString);

    public:
        static EventMgr*    Instance();
        static void         Destroy();

    private:
        static EventMgr*    m_spInstance;
    };
};

inline
ZFqt::EventMgr* ZFqt::EventMgr::Instance()
{
    if (ZFqt::EventMgr::m_spInstance == NULL)
    {
        ZFqt::EventMgr::m_spInstance    =   new ZFqt::EventMgr();
    }

    return ZFqt::EventMgr::m_spInstance;
}

inline
void    ZFqt::EventMgr::Destroy()
{
    if (ZFqt::EventMgr::m_spInstance != NULL)
    {
        delete ZFqt::EventMgr::m_spInstance;
        ZFqt::EventMgr::m_spInstance    =   NULL;
    }
}

#endif //end ZFqt_EVENT_H

