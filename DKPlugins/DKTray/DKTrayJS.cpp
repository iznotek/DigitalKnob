#ifdef USE_DKDuktape 
#include "DKTray.h"
#include "DKTrayJS.h"


/////////////////////
void DKTrayJS::Init()
{
	DKDuktape::AttachFunction("DKTray_GetIcon", DKTrayJS::GetIcon, 0);
	DKDuktape::AttachFunction("DKTray_SetIcon", DKTrayJS::SetIcon, 1);

}

////////////////////////////////
int DKTrayJS::GetIcon(duk_context* ctx)
{
	DKString icon = DKTray::Get("DKTray0")->GetIcon();
	duk_push_string(ctx, icon.c_str());
	return 1;
}

///////////////////////////////
int DKTrayJS::SetIcon(duk_context* ctx)
{
	DKString file = duk_require_string(ctx, 0);
	DKTray::Get("DKTray0")->SetIcon(file);
	return 1;
}


#endif //USE_DKDuktape