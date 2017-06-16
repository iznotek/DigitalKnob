#ifdef USE_DKCef
#include "DKWindowV8.h"
#include "DKWindow.h"
#include "DKApp.h"


///////////////////////
void DKWindowV8::Init()
{
	DKLog("DKWindowV8::Init()\n", DKDEBUG);
	DKV8::AttachFunction("TestInt", DKWindowV8::TestInt);
	DKV8::AttachFunction("TestString", DKWindowV8::TestString);
	DKV8::AttachFunction("TestReturnInt", DKWindowV8::TestReturnInt);
	DKV8::AttachFunction("TestReturnString", DKWindowV8::TestReturnString);
}

//////////////////////
void DKWindowV8::End()
{
	DKLog("DKWindowV8::End()\n", DKDEBUG);
}


////////////////////////////////////////////////////////
bool DKWindowV8::TestInt(CefArgs args, CefReturn retval)
{
	DKLog("DKWindowV8::TestInt(CefArgs,CefReturn)\n", DKINFO);

	int data = args->GetInt(0);
	int result = data;
	retval->SetInt(0, result);
	return true;
}

///////////////////////////////////////////////////////////
bool DKWindowV8::TestString(CefArgs args, CefReturn retval)
{
	DKLog("DKWindowV8::TestString(CefArgs,CefReturn)\n", DKINFO);

	DKString data = args->GetString(0);
	DKString result = data;
	retval->SetString(0, result);
	return true;
}

//////////////////////////////////////////////////////////////
bool DKWindowV8::TestReturnInt(CefArgs args, CefReturn retval)
{
	DKLog("DKWindowV8::TestReturnInt(CefArgs,CefReturn)\n", DKINFO);

	int result = 12345;
	retval->SetInt(0, result);
	return true;
}

/////////////////////////////////////////////////////////////////
bool DKWindowV8::TestReturnString(CefArgs args, CefReturn retval)
{
	DKLog("DKWindowV8::TestReturnString(CefArgs,CefReturn)\n", DKINFO);

	DKString result = "test string";
	retval->SetString(0, result);
	return true;
}



#endif //USE_DKCef