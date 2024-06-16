
#ifndef ZFqt_TYPES_H
#define ZFqt_TYPES_H

#include <stdint.h>
#include <string.h>
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

#endif //end ZFqt_TYPES_H

