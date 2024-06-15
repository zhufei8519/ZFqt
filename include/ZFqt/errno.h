
#ifndef ZFqt_ERRNO_H
#define ZFqt_ERRNO_H

namespace ZFqt
{
	enum ET_Errno
	{
		E_Errno_SUCCESS		=	0,
		E_Errno_ERR_GENERAL	=	-1,

		E_Errno_ERR_INVALID_PARAMS	=	-11,
		E_Errno_ERR_INVALID_STATUS	=	-12,
		E_Errno_ERR_NO_MORE_MEM		=	-13,
		E_Errno_ERR_NO_MORE_RES		=	-14,
		E_Errno_ERR_ACCESS_DENIED	=	-15,
		E_Errno_ERR_EXEC_SQL		=	-16,
	};
};

#endif //end ZFqt_ERRNO_H

