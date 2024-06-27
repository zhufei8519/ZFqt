
#ifndef ZFqt_TYPES_H
#define ZFqt_TYPES_H

#include <stdint.h>
#include <string.h>
#include <vector>
#include <memory>
#include "ZFqt/errno.h"
#include "ZFqt/consts.h"
#include "ZFqt/export.h"

#include <QObject>
#include <QThread>
#include <QString>
#include <QDateTime>
#include <QDir>
#include <QFile>

#include <QWaitCondition>
#include <QMutex>
#include <QMutexLocker>

#ifndef snprintf
#	define	snprintf	_snprintf
#endif //end snprintf

#if defined(_MSC_VER)
#	define	strcasecmp	_stricmp
#endif //end _MSC_VER

namespace ZFqt
{
    struct NVItem
    {
        QString qstrName;
        QString qstrValue;
        bool    bIsPassword;

        NVItem()
            :bIsPassword(false)
        {}
        NVItem(const QString& qstrNameParam, const QString& qstrValueParam, bool bIsPasswordParam = false)
            :qstrName(qstrNameParam), qstrValue(qstrValueParam), bIsPassword(bIsPasswordParam)
        {}
        NVItem(const NVItem& rhs)
            :qstrName(rhs.qstrName), qstrValue(rhs.qstrValue), bIsPassword(rhs.bIsPassword)
        {}
    };

    struct TCachedLoginInfo
    {
        QString qstrAccount;
        QString qstrPassword;

        int32_t nAutoLogin;

        TCachedLoginInfo()
            :nAutoLogin(0)
        {}
        TCachedLoginInfo(const QString& qstrAccountParam, const QString& qstrPasswordParam,
                        int32_t nAutoLoginParam)
            :qstrAccount(qstrAccountParam), qstrPassword(qstrPasswordParam)
            , nAutoLogin(nAutoLoginParam)
        {}
        TCachedLoginInfo(const TCachedLoginInfo& rhs)
            :qstrAccount(rhs.qstrAccount), qstrPassword(rhs.qstrPassword)
            , nAutoLogin(rhs.nAutoLogin)
        {}

        void    Clear()
        {
            qstrAccount.clear();
            qstrPassword.clear();

            nAutoLogin  =   0;
        }
    };

    struct TLoginInfo
    {
        QString qstrAccount;
        QString qstrPassword;

        bool    bSaveAccount;
        bool    bSavePassword;
        bool    bAutoLogin;

        // should NOT use constructor directly, use FromCachedLoginInfo instead
        TLoginInfo()
            :bSaveAccount(false), bSavePassword(false), bAutoLogin(false)
        {}
        TLoginInfo(const QString& qstrAccountParam, const QString& qstrPasswordParam,
                    bool bSaveAccountParam, bool bSavePasswordParam, bool bAutoLoginParam)
            :qstrAccount(qstrAccountParam), qstrPassword(qstrPasswordParam)
            , bSaveAccount(bSaveAccountParam), bSavePassword(bSavePasswordParam), bAutoLogin(bAutoLoginParam)
        {}
        TLoginInfo(const TLoginInfo& rhs)
            :qstrAccount(rhs.qstrAccount), qstrPassword(rhs.qstrPassword)
            , bSaveAccount(rhs.bSaveAccount), bSavePassword(rhs.bSavePassword), bAutoLogin(rhs.bAutoLogin)
        {}

        void    Clear()
        {
            qstrAccount.clear();
            qstrPassword.clear();

            bSaveAccount    =   false;
            bSavePassword   =   false;
            bAutoLogin      =   false;
        }

        void    FromCachedLoginInfo(const TCachedLoginInfo& cachedLoginInfo)
        {
            this->Clear();

            if (!cachedLoginInfo.qstrAccount.isEmpty())
            {
                this->qstrAccount   =   cachedLoginInfo.qstrAccount;
                this->qstrPassword  =   cachedLoginInfo.qstrPassword;

                this->bSaveAccount  =   true;
                this->bSavePassword =   cachedLoginInfo.qstrPassword.isEmpty() ? false : true;
                this->bAutoLogin    =   (cachedLoginInfo.nAutoLogin != 0);
            }
        }

        void    ToCachedLoginInfo(TCachedLoginInfo& cachedLoginInfo)
        {
            cachedLoginInfo.Clear();

            if (this->bSaveAccount)     cachedLoginInfo.qstrAccount     =   this->qstrAccount;
            if (this->bSavePassword)    cachedLoginInfo.qstrPassword    =   this->qstrPassword;
            cachedLoginInfo.nAutoLogin  =   this->bAutoLogin ? 1 : 0;
        }
    };
};

typedef ZFqt::NVItem                ZFqt_TNVItem;
typedef std::vector< ZFqt_TNVItem > ZFqt_TVecNVItems;

typedef ZFqt::TCachedLoginInfo              ZFqt_TCachedLoginInfo;
typedef std::list< ZFqt_TCachedLoginInfo >  ZFqt_TListCachedLoginInfos;

typedef ZFqt::TLoginInfo                ZFqt_TLoginInfo;
typedef std::vector< ZFqt_TLoginInfo >  ZFqt_TVecLoginInfos;

#endif //end ZFqt_TYPES_H

