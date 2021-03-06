#ifdef USE_DKDuktape 
#include "DKDebug/DKDebug.h"
#include "DKDebug/DKDebugJS.h"


//////////////////////
bool DKDebugJS::Init()
{
	DKDuktape::AttachFunction("DKDebug_SendBugReport", DKDebugJS::SendBugReport);
	DKDuktape::AttachFunction("DKDebug_ShowStackTrace", DKDebugJS::ShowStackTrace);
	return true;
}

//////////////////////////////////////////////
int DKDebugJS::SendBugReport(duk_context* ctx)
{
	DKString filename = duk_require_string(ctx, 0);
	if(!DKDebug::SendBugReport(filename)){ return 0; }
	return 1;
}

///////////////////////////////////////////////
int DKDebugJS::ShowStackTrace(duk_context* ctx)
{
	if(!DKDebug::ShowStackTrace()){ return 0; }
	return 1;
}

#endif //USE_DKDuktape