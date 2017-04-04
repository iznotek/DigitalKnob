#ifdef USE_DKCef
#include "DKHookV8.h"
#include "DKHook.h"

/////////////////////
void DKHookV8::Init()
{
	DKLog("DKHookV8::Init()\n", DKDEBUG);
	//DKV8::AttachFunction("Test", DKHookV8::Test);
	DKV8::AttachFunction("DKHook_SendHook", DKHookV8::SendHook);
	DKV8::AttachFunction("DKHook_GetValue", DKHookV8::GetValue);
	DKV8::AttachFunction("DKHook_SetValue", DKHookV8::SetValue);
	DKV8::AttachFunction("DKHook_Click", DKHookV8::Click);
	DKV8::AttachFunction("DKHook_PrevHandle", DKHookV8::PrevHandle);
	DKV8::AttachFunction("DKHook_NextHandle", DKHookV8::NextHandle);
	DKV8::AttachFunction("DKHook_ToggleHighlight", DKHookV8::ToggleHighlight);
	DKV8::AttachFunction("DKHook_SetWindowHandle", DKHookV8::SetWindowHandle);
	DKV8::AttachFunction("DKHook_GetWindows", DKHookV8::GetWindows);
	DKV8::AttachFunction("DKHook_CurrentHandle", DKHookV8::CurrentHandle);
	DKV8::AttachFunction("DKHook_WindowExists", DKHookV8::WindowExists);
	DKV8::AttachFunction("DKHook_WaitForWindow", DKHookV8::WaitForWindow);
}

///////////////////
void DKHookV8::End()
{
	DKLog("DKHookV8::End()\n", DKDEBUG);
}

/*
///////////////////////////////////////////////////
bool DKHookV8::Test(CefArgs args, CefReturn retval)
{
	DKLog("DKHookV8::Test(CefArgs,CefReturn)\n", DKDEBUG);
	DKString data = args[0]->GetStringValue();
	DKString result = data;
	retval = CefV8Value::CreateString(result);
	return true;
}
*/

///////////////////////////////////////////////////////
bool DKHookV8::SendHook(CefArgs args, CefReturn retval)
{
	DKString window = args[0]->GetStringValue();
	DKString handle = args[1]->GetStringValue();
	DKString data = args[2]->GetStringValue();

	if(!DKHook::Instance("DKHook")->SendHook(window, handle, data)){
		return false;
	}
	return true;
}

///////////////////////////////////////////////////////
bool DKHookV8::GetValue(CefArgs args, CefReturn retval)
{
	DKString value;
	if(!DKHook::Instance("DKHook")->GetString(value)){
		return false;
	}
	retval = CefV8Value::CreateString(value);
	return true;
}

///////////////////////////////////////////////////////
bool DKHookV8::SetValue(CefArgs args, CefReturn retval)
{
	DKString value = args[0]->GetStringValue();
	if(!DKHook::Instance("DKHook")->SetString(value)){
		return false;
	}
	return true;
}

////////////////////////////////////////////////////
bool DKHookV8::Click(CefArgs args, CefReturn retval)
{
	if(!DKHook::Instance("DKHook")->Click()){
		return false;
	}
	return true;
}

/////////////////////////////////////////////////////////
bool DKHookV8::PrevHandle(CefArgs args, CefReturn retval)
{
	if(!DKHook::Instance("DKHook")->PrevHandle()){
		return false;
	}
	return true;
}

/////////////////////////////////////////////////////////
bool DKHookV8::NextHandle(CefArgs args, CefReturn retval)
{
	if(!DKHook::Instance("DKHook")->NextHandle()){
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////
bool DKHookV8::ToggleHighlight(CefArgs args, CefReturn retval)
{
	DKHook::Instance("DKHook")->ToggleHighlight();
	return true;
}

//////////////////////////////////////////////////////////////
bool DKHookV8::SetWindowHandle(CefArgs args, CefReturn retval)
{
	DKString window = args[0]->GetStringValue();
	if(!DKHook::Instance("DKHook")->SetWindowHandle(window)){
		return false;
	}
	return true;
}

/////////////////////////////////////////////////////////
bool DKHookV8::GetWindows(CefArgs args, CefReturn retval)
{
	DKStringArray arry = DKHook::Instance("DKHook")->GetWindows();
	DKString list = toString(arry, ",");
	retval = CefV8Value::CreateString(list);
	return true;
}

////////////////////////////////////////////////////////////
bool DKHookV8::CurrentHandle(CefArgs args, CefReturn retval)
{
	DKString handle = toString(DKHook::Instance("DKHook")->currentHandle);
	retval = CefV8Value::CreateString(handle);
	return true;
}

///////////////////////////////////////////////////////////
bool DKHookV8::WindowExists(CefArgs args, CefReturn retval)
{
	DKString window = args[0]->GetStringValue();
	if(!DKHook::Instance("DKHook")->WindowExists(window)){
		return false;
	}
	retval = CefV8Value::CreateBool(true);
	return true;
}

////////////////////////////////////////////////////////////
bool DKHookV8::WaitForWindow(CefArgs args, CefReturn retval)
{
	DKString window = args[0]->GetStringValue();
	int timeout = args[1]->GetIntValue();
	if(!DKHook::Instance("DKHook")->WaitForWindow(window, timeout)){
		return false;
	}
	retval = CefV8Value::CreateBool(true);
	return true;
}

#endif //USE_DKCef