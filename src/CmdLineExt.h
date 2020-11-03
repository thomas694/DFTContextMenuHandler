

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0500 */
/* at Fri May 15 20:36:34 2015
 */
/* Compiler settings for .\CmdLineExt.idl:
    Oicf, W1, Zp8, env=Win64 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__


#ifndef __CmdLineExt_h__
#define __CmdLineExt_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __CmdLineContextMenu_FWD_DEFINED__
#define __CmdLineContextMenu_FWD_DEFINED__

#ifdef __cplusplus
typedef class CmdLineContextMenu CmdLineContextMenu;
#else
typedef struct CmdLineContextMenu CmdLineContextMenu;
#endif /* __cplusplus */

#endif 	/* __CmdLineContextMenu_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __CMDLINEEXTLib_LIBRARY_DEFINED__
#define __CMDLINEEXTLib_LIBRARY_DEFINED__

/* library CMDLINEEXTLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_CMDLINEEXTLib;

EXTERN_C const CLSID CLSID_CmdLineContextMenu;

#ifdef __cplusplus

class DECLSPEC_UUID("F062AE83-FCD6-4845-BA4E-D74FC5D59B38")
CmdLineContextMenu;
#endif
#endif /* __CMDLINEEXTLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


