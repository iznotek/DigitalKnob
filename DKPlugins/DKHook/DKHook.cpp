#include "stdafx.h"

#ifdef WIN32
#include "DKHook.h"
#include "DKAssets.h"
#include "DKFile.h"
#include <tchar.h>

DKStringArray DKHook::_windows;
std::vector<HWND> DKHook::handle;

///////////////////
void DKHook::Init()
{
	DKCreate("DKHookJS");
	DKCreate("DKHookV8");
	DKString path = DKFile::local_assets + "DKHook/hookdll.dll";
	hModule = LoadLibrary(path.c_str());
	if(NULL == hModule){
		DKLog("DKHook::DKHook(): cannot find hookdll.dll \n", DKERROR);
	}
	highlight = false;
	currentHandle = 0;
}

//////////////////
void DKHook::End()
{
	if(hModule){FreeLibrary(hModule);}
}


////////////////////
bool DKHook::Click()
{
	if(handle.empty()){ return false; }
	SendMessage(handle[currentHandle], BM_CLICK, 0, 0);
	return true;
}

//////////////////////////
bool DKHook::DoHighlight()
{
	if(!highlight){ return false; }
	if(handle.empty()){ return false; }

	RedrawWindow(GetDesktopWindow(), NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW); //FIXME

	RECT rect;
	GetWindowRect(handle[currentHandle], &rect);
	HDC screenDC = ::GetDC(GetDesktopWindow());

	HPEN pen, oldPen;
	pen = CreatePen(PS_SOLID, 2, RGB(0, 0, 255)); 
	oldPen = (HPEN)SelectObject(screenDC, pen);

	::MoveToEx(screenDC, rect.left, rect.top, NULL);
	::LineTo(screenDC, rect.right, rect.top);
	::LineTo(screenDC, rect.right, rect.bottom);
	::LineTo(screenDC, rect.left, rect.bottom);
	::LineTo(screenDC, rect.left, rect.top);
	::ReleaseDC(0, screenDC);

	SelectObject(screenDC, oldPen); 
	DeleteObject(pen);
	return true;
}

/////////////////////////////////////
bool DKHook::GetClass(DKString& clas)
{
	char classname[256];
	if(!GetClassName(handle[currentHandle], classname, 256)){
		DKLog("DKHook::GetClass("+clas+")\n", DKWARN);
		return false; 
	}
	clas = classname;
	return true;
}

/////////////////////////
bool DKHook::GetHandles()
{
	if(handle.empty() || handle[0] == NULL){ //Window is not open so don't look for its items.
		DKLog("DKHook::GetHandles() window not found! \n", DKERROR);
		return false;
	}

	//Window IS open, so look for all of the items and assigne handles
	bool searchFromBase = true;
	unsigned int s = 0;
	unsigned int h = 1;
	while(s < handle.size()){
		HWND temp = NULL;
		if(searchFromBase){
			temp = FindWindowEx(handle[s], NULL, NULL, NULL);
			if(temp){ handle.push_back(temp); }
			searchFromBase = false;
		}
		else{
			temp = FindWindowEx(handle[s], handle[h-1], NULL, NULL);
			if(temp){ handle.push_back(temp); }
		}
		if(temp != NULL){
			h++;
		}
		else{
			s++; searchFromBase = true;
		}
	}

	return true;
}

///////////////////////////////
bool DKHook::GetLeft(int& left)
{
	if(handle.empty()){ return false; }
	RECT rect;
	GetWindowRect(handle[currentHandle], &rect);
	left = rect.left;
	return true;
}

////////////////////////////////////////
bool DKHook::GetParent(DKString& parent)
{
	if(handle.empty()){ return false; }
	HWND par = ::GetParent(handle[currentHandle]);
	if(!par){ return false; }
	int len = SendMessage(par, WM_GETTEXTLENGTH, 0, 0);
    char* buffer = new char[len];
    SendMessage(par, WM_GETTEXT, (WPARAM)len+1, (LPARAM)buffer);
    parent = buffer;
	return true;
}

//////////////////////////////////////
bool DKHook::GetString(DKString& text)
{
	if(handle.empty()){ return false; }
	int len = SendMessage(handle[currentHandle], WM_GETTEXTLENGTH, 0, 0);
	char* buffer = new char[len];
	SendMessage(handle[currentHandle], WM_GETTEXT, (WPARAM)len+1, (LPARAM)buffer);
	text = buffer;
	return true;
}

/////////////////////////////
bool DKHook::GetTop(int& top)
{
	if(handle.empty()){ return false; }
	RECT rect;
	GetWindowRect(handle[currentHandle], &rect);
	top = rect.top;
	return true;
}

///////////////////////////////////////////////
bool DKHook::GetWindows(DKStringArray& windows)
{
	_windows.clear();
	bool rval = EnumWindows(GetWindows, NULL);
	windows = _windows;
	return rval;
}

/////////////////////////
bool DKHook::NextHandle()
{
	if(handle.empty()){
		DKLog("DKHook::NextHandle(): handle is empty\n", DKWARN);
		return false; 
	}
	if(currentHandle < handle.size()-1){currentHandle++;}
	if(currentHandle <= handle.size()){
		DoHighlight();
	}
	return true;
}

/////////////////////////
bool DKHook::PrevHandle()
{
	if(handle.empty()){
		DKLog("DKHook::PrevHandle(): handle is empty\n", DKWARN);
		return false; 
	}
	if(currentHandle > 0){currentHandle--;}
	if(currentHandle <= handle.size()){
		DoHighlight();
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////
bool DKHook::SendHook(const DKString& window, const DKString& handle, const DKString& data)
{
	DKLog("DKHook::SendHook("+window+", "+handle+", "+data+")\n", DKDEBUG);

	if(!SetWindowHandle(window, 1)){ return false; }
	if(!SetHandle(toInt(handle), 1)){ return false; }

	DKStringArray arry;
	toStringArray(arry, data, ",");

	if(same(arry[0], "click")){
		Click();
	}
	if(same(arry[0], "GetValue")){
		DKString value;
		GetString(value);
		DKLog("Hook GetValue returned: "+value+"\n", DKINFO);
	}
	if(same(arry[0], "SetValue")){
		SetString(arry[1]);
	}
	return true;
}

////////////////////////////////////////////////////////////////
bool DKHook::SetHandle(unsigned int index, unsigned int timeout)
{
	unsigned int t = 0;
	while(index > handle.size() && t < timeout){
		GetHandles();
		Sleep(1000);
		++t;
	}
	if(t >= timeout){
		DKLog("DKHook::SetHandle("+toString(index)+","+toString(timeout)+"): timed out.\n", DKWARN);
		return false;
	}
	currentHandle = index;
	return true;
}

///////////////////////////////////////////////////////////////////
bool DKHook::SetHandle(const DKString& value, unsigned int timeout)
{
	unsigned int t = 0;
	unsigned int h = 0;
	DKString text;
	while(t < timeout){
		GetHandles();
		for(h=0; h<handle.size(); h++){
			int len = SendMessage(handle[h], WM_GETTEXTLENGTH, 0, 0);
			char* buffer = new char[len];
			SendMessage(handle[h], WM_GETTEXT, (WPARAM)len+1, (LPARAM)buffer);
			text = buffer;
			if(text == value){
				currentHandle = h;
				return true;
			}
		}
		Sleep(1000);
		++t;
	}

	DKLog("DKHook::SetHandle("+value+","+toString(timeout)+"): timed out.\n", DKWARN);
	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////
bool DKHook::SetHandle(const DKString& clas, const DKString& value, unsigned int timeout)
{
	unsigned int t = 0;
	unsigned int h = 0;
	DKString text;
	char classname[256];

	while(t < timeout){
		GetHandles();
		for(h=0; h<handle.size(); h++){
			int len = SendMessage(handle[h], WM_GETTEXTLENGTH, 0, 0);
			char* buffer = new char[len];
			SendMessage(handle[h], WM_GETTEXT, (WPARAM)len+1, (LPARAM)buffer);
			DKString text = buffer;
			if (value.empty()) {
				int test = 0;
			}
			if(text == value){
				if(!GetClassName(handle[h], classname, 256)){
					DKLog("DKHook::SetHandle("+clas+","+value+"): GetClassName failed. \n", DKWARN);
					return false; 
				}
				if(clas == (DKString)classname){
					currentHandle = h;
					return true;
				}
			}
		}
		Sleep(1000);
		++t;
	}
	
	DKLog("DKHook::SetHandle("+clas+","+value+","+toString(timeout)+"): timed out.\n", DKWARN);
	return false;
}

////////////////////////////////////////////
bool DKHook::SetString(const DKString& text)
{
	if(handle.empty()){ return false; }
	SendMessage(handle[currentHandle], WM_SETTEXT, (WPARAM)text.size(), (LPARAM)text.c_str());
	return true;
}

/////////////////////////////////////////////////////////////////////////
bool DKHook::SetWindowHandle(const DKString& title, unsigned int timeout)
{
	handle.clear();
	HWNDname temp;
	temp.caption = title.c_str();
	unsigned int t = 0;
	while(EnumWindows(FindWindow, (LPARAM)&temp) && t < timeout){
		Sleep(1000);
		++t;
	}
	if(t >= timeout){
		DKLog("DKHook::SetWindowHandle("+title+","+toString(timeout)+"): timed out.\n", DKWARN);
		return false;
	}
	
	GetHandles();
	SetHandle(0,1);
	DKLog("Selected Window: "+title+"\n", DKINFO);
	return true;
}

//////////////////////////////////////////
bool DKHook::ShowWindow(unsigned int flag)
{
	::ShowWindow(handle[currentHandle], flag);
	return true;
}

//////////////////////////////
bool DKHook::ToggleHighlight()
{
	if(highlight){
		highlight = false;
		DoHighlight();
		return true;
	}
	highlight = true;
	DoHighlight();
	return true;
}

//////////////////////////////////////////////////////////////
bool DKHook::WaitForWindow(const DKString& title, int timeout)
{
	//FIXME - this is blocking,  thread this out
	int i = 0;
	while(!WindowExists(title) && i < timeout){
		Sleep(1000);
		++i;
	}
	if(i >= timeout){
		DKLog("DKHook::WaitForWindow("+title+","+toString(timeout)+"): timed out.\n", DKWARN);
		return false;
	}
	return true;
}

///////////////////////////////////////////////////////////
bool DKHook::WaitForHandle(unsigned int index, int timeout)
{
	int i = 0;
	while(index > handle.size() && i < timeout){
		GetHandles();
		Sleep(1000);
		++i;
	}
	if(i >= timeout){
		DKLog("DKHook::WaitForHandle("+toString(index)+","+toString(timeout)+"): timed out.\n", DKWARN);
		return false;
	}
	//currentHandle = index;
	return true;
}

////////////////////////////////////////////////////////////////////////////////////
bool DKHook::WaitForHandle(const DKString& clas, const DKString& value, int timeout)
{
	int i = 0;
	DKString text;
	char classname[256];
	
	while(text != value && clas != (DKString)classname && i < timeout){
		GetHandles();
		for(unsigned int i=0; i<handle.size(); i++){
			int len = SendMessage(handle[i], WM_GETTEXTLENGTH, 0, 0);
			char* buffer = new char[len];
			SendMessage(handle[i], WM_GETTEXT, (WPARAM)len+1, (LPARAM)buffer);
			DKString text = buffer;
			if(text == value){
				if(!GetClassName(handle[i], classname, 256)){
					DKLog("DKHook::SetHandle("+clas+","+value+"): GetClassName failed. \n", DKWARN);
					return false; 
				}
			}
		}
		Sleep(1000);
		++i;
	}
	if(i >= timeout){
		DKLog("DKHook::WaitForHandle("+clas+","+value+","+toString(timeout)+"): timed out.\n", DKWARN);
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////
bool DKHook::WaitForHandle(const DKString& value, int timeout)
{
	int i = 0;
	DKString text;
	while(text != value && i < timeout){
		GetHandles();
		for(unsigned int i=0; i<handle.size(); i++){
			int len = SendMessage(handle[i], WM_GETTEXTLENGTH, 0, 0);
			char* buffer = new char[len];
			SendMessage(handle[i], WM_GETTEXT, (WPARAM)len+1, (LPARAM)buffer);
			text = buffer;
		}
		Sleep(1000);
		++i;
	}
	if(i >= timeout){
		DKLog("DKHook::WaitForHandle("+value+","+toString(timeout)+"): timed out.\n", DKWARN);
		return false;
	}
	return true;
}

////////////////////////////////////////////////
bool DKHook::WindowExists(const DKString& title)
{
	HWNDname temp;
	temp.caption = title.c_str();
	if(EnumWindows(FindWindow, (LPARAM)&temp)){
		//TODO: GetLastError
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////
BOOL CALLBACK DKHook::EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	char class_name[80];
	char title[80];
	GetClassName(hwnd,class_name, sizeof(class_name));
	GetWindowText(hwnd,title,sizeof(title));

	/*
	if(!same("",title)){
	DKLog("Window: ");
	DKLog(title);
	DKLog(" Class: ");
	DKLog(class_name);
	DKLog("\n");
	}
	*/

	return TRUE;
}

//////////////////////////////////////////////////////////
BOOL CALLBACK DKHook::FindWindow(HWND hwnd, LPARAM lparam) 
{     
	HWNDname *temp = (HWNDname*)lparam;
	static TCHAR buffer[50];      
	GetWindowText(hwnd, buffer, 50);     
	if(strcmp(buffer, temp->caption) == 0){ 
		handle.push_back(hwnd);
		return FALSE;     
	}      
	return TRUE; 
}

/////////////////////////////////////////////////////////////////
BOOL CALLBACK DKHook::FindWindowPartial(HWND hwnd, LPARAM lparam) 
{     
	HWNDname *temp = (HWNDname*)lparam;
	static TCHAR buffer[50];      
	GetWindowText(hwnd, buffer, 50);     
	if(_tcsstr(buffer, temp->caption)) { 
		handle.push_back(hwnd);
		return FALSE;     
	}      
	return TRUE; 
}

//////////////////////////////////////////////////////////
BOOL CALLBACK DKHook::GetWindows(HWND hwnd, LPARAM lparam) 
{     
	static TCHAR buffer[50];      
	GetWindowText(hwnd, buffer, 50);
	DKString title = buffer;
	if(!title.empty()){
		_windows.push_back(title);
	}    
	return true; 
}

#endif //WIN32