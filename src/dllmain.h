// dllmain.h : Declaration of module class.
#include "CmdLineExt_i.h"

class CCmdLineExtModule : public ATL::CAtlDllModuleT< CCmdLineExtModule >
{
public :
	DECLARE_LIBID(LIBID_CmdLineExtLib)
	HINSTANCE hInstance;
};

extern class CCmdLineExtModule _AtlModule;
