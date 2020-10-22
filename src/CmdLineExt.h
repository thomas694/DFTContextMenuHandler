/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 5.01.0164 */
/* at Wed Feb 09 15:27:51 2000
 */
/* Compiler settings for D:\Development\Projects\Utilities\CmdLineExt\CmdLineExt.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __CmdLineExt_h__
#define __CmdLineExt_h__

#ifdef __cplusplus
extern "C"{
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

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 


#ifndef __CMDLINEEXTLib_LIBRARY_DEFINED__
#define __CMDLINEEXTLib_LIBRARY_DEFINED__

/* library CMDLINEEXTLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_CMDLINEEXTLib;

EXTERN_C const CLSID CLSID_CmdLineContextMenu;

#ifdef __cplusplus

class DECLSPEC_UUID("9869EFB4-18E9-11D3-A837-00104B9E30B5")
CmdLineContextMenu;
#endif
#endif /* __CMDLINEEXTLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
