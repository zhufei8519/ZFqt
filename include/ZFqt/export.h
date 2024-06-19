
#ifndef ZFqt_EXPORT_H
#define ZFqt_EXPORT_H

#ifdef ZFqt_BUILD_DLL
#	define ZFqt_API	Q_DECL_EXPORT
#else //!ZFqt_BUILD_DLL
#	define ZFqt_API	Q_DECL_IMPORT
#endif //end ZFqt_BUILD_DLL + !ZFqt_BUILD_DLL

#endif //end ZFqt_EXPORT_H

