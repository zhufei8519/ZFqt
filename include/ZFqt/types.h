
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
};

typedef ZFqt::NVItem                ZFqt_TNVItem;
typedef std::vector< ZFqt_TNVItem > ZFqt_TVecNVItems;

#endif //end ZFqt_TYPES_H

