#include "DK/stdafx.h"
#include "DKSFMLWindow/DKSFMLWindow.h"
#include <SFML/Graphics.hpp>

std::vector<boost::function<bool(sf::Event& e)> > DKSFMLWindow::event_funcs;
std::vector<boost::function<void()> > DKSFMLWindow::draw_funcs;


/////////////////////////
bool DKSFMLWindow::Init()
{
	DKLog("DKSFMLWindow::Init()\n", DKINFO);

	window.create(sf::VideoMode(800, 600), "My window");

	DKClass::RegisterFunc("DKSFMLWindow::TestInt", &DKSFMLWindow::TestInt, this);
	DKClass::RegisterFunc("DKSFMLWindow::TestString", &DKSFMLWindow::TestString, this);
	DKClass::RegisterFunc("DKSFMLWindow::TestReturnInt", &DKSFMLWindow::TestReturnInt, this);
	DKClass::RegisterFunc("DKSFMLWindow::TestReturnString", &DKSFMLWindow::TestReturnString, this);

	DKClass::RegisterFunc("DKSFMLWindow::Fullscreen", &DKSFMLWindow::Fullscreen, this);
	DKClass::RegisterFunc("DKSFMLWindow::GetClipboard", &DKSFMLWindow::GetClipboard, this);
	DKClass::RegisterFunc("DKSFMLWindow::GetHandle", &DKSFMLWindow::GetHandle, this);
	DKClass::RegisterFunc("DKSFMLWindow::GetHeight", &DKSFMLWindow::GetHeight, this);
	DKClass::RegisterFunc("DKSFMLWindow::GetMouseX", &DKSFMLWindow::GetMouseX, this);
	DKClass::RegisterFunc("DKSFMLWindow::GetMouseY", &DKSFMLWindow::GetMouseY, this);
	DKClass::RegisterFunc("DKSFMLWindow::GetWidth", &DKSFMLWindow::GetWidth, this);
	DKClass::RegisterFunc("DKSFMLWindow::GetX", &DKSFMLWindow::GetX, this);
	DKClass::RegisterFunc("DKSFMLWindow::GetY", &DKSFMLWindow::GetY, this);
	DKClass::RegisterFunc("DKSFMLWindow::Hide", &DKSFMLWindow::Hide, this);
	DKClass::RegisterFunc("DKSFMLWindow::IsFullscreen", &DKSFMLWindow::IsFullscreen, this);
	DKClass::RegisterFunc("DKSFMLWindow::IsVisible", &DKSFMLWindow::IsVisible, this);
	DKClass::RegisterFunc("DKSFMLWindow::MessageBox", &DKSFMLWindow::MessageBox, this);
	DKClass::RegisterFunc("DKSFMLWindow::Minimize", &DKSFMLWindow::Minimize, this);
	DKClass::RegisterFunc("DKSFMLWindow::Restore", &DKSFMLWindow::Restore, this);
	DKClass::RegisterFunc("DKSFMLWindow::SetClipboard", &DKSFMLWindow::SetClipboard, this);
	DKClass::RegisterFunc("DKSFMLWindow::SetHeight", &DKSFMLWindow::SetHeight, this);
	DKClass::RegisterFunc("DKSFMLWindow::SetIcon", &DKSFMLWindow::SetIcon, this);
	DKClass::RegisterFunc("DKSFMLWindow::SetWidth", &DKSFMLWindow::SetWidth, this);
	DKClass::RegisterFunc("DKSFMLWindow::SetX", &DKSFMLWindow::SetX, this);
	DKClass::RegisterFunc("DKSFMLWindow::SetY", &DKSFMLWindow::SetY, this);
	DKClass::RegisterFunc("DKSFMLWindow::Show", &DKSFMLWindow::Show, this);
	DKClass::RegisterFunc("DKSFMLWindow::Windowed", &DKSFMLWindow::Windowed, this);

	DKApp::AppendLoopFunc(&DKSFMLWindow::Process, this);
	DKSFMLWindow::AddEventFunc(&DKSFMLWindow::handle, this);

	return true;
}

////////////////////////
bool DKSFMLWindow::End()
{
	window.close();
	return true;
}

////////////////////////////
void DKSFMLWindow::Process()
{
	sf::Event e;
	while(window.pollEvent(e)){
		for(unsigned int i = 0; i < event_funcs.size(); ++i){
			if(event_funcs[i](e)){ //Call event functions
				i = event_funcs.size();	//eat the event
			}; 
		}
	}

	// Activate the window for OpenGL rendering
	window.setActive();

	// OpenGL drawing commands go here...

	// End the current frame and display its contents on screen
	window.display();
}

///////////////////////////////////////
bool DKSFMLWindow::handle(sf::Event& e)
{
	if(e.type == sf::Event::Closed){
		DKApp::Exit();
		return false;
	}
	return false; //allow event to continue
}


///////////////////////////////////////////////////////////
bool DKSFMLWindow::TestInt(const void* input, void* output)
{
	int in = *(int*)input;
	int out = in;
	*(int*)output = out;
	return true;
}

//////////////////////////////////////////////////////////////
bool DKSFMLWindow::TestString(const void* input, void* output)
{
	std::string in = *(std::string*)input;
	std::string out = in;
	*(std::string*)output = out;
	return true;
}

/////////////////////////////////////////////////////////////////
bool DKSFMLWindow::TestReturnInt(const void* input, void* output)
{
	int var = 1234;
	*(int*)output = var;
	return true;
}

////////////////////////////////////////////////////////////////////
bool DKSFMLWindow::TestReturnString(const void* input, void* output)
{
	std::string var = "Return test";
	*(std::string*)output = var;
	return true;
}


//////////////////////////////////////////////////////////////
bool DKSFMLWindow::Fullscreen(const void* input, void* output)
{
#ifdef WIN32
	HWND hwnd = window.getSystemHandle();
	DWORD dwStyle = GetWindowLong(hwnd, GWL_STYLE);
	MONITORINFO mi = { sizeof(mi) };
	if(GetWindowPlacement(hwnd, &g_wpPrev) && GetMonitorInfo(MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY), &mi)){
		SetWindowLong(hwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
		SetWindowPos(hwnd, HWND_TOP,
			mi.rcMonitor.left, mi.rcMonitor.top,
			mi.rcMonitor.right - mi.rcMonitor.left,
			mi.rcMonitor.bottom - mi.rcMonitor.top,
			SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
	}
	return true;
#endif
#ifdef MAC
	//TODO
	NSView* nsview = window.getSystemHandle();
	if(!nsview){ return false; }
#endif
#ifdef LINUX
	GdkWindow* gdk_window = gdk_window_foreign_new(window.getSystemHandle());
	if(!gdk_window){ return false; }
	gdk_window_fullscreen(gdk_window);
	isFullscreen = true;
	return true;
#endif
	DKLog("DKCefWindow::Fullscreen(): not implemented on this OS\n", DKWARN);
	return false;
}

////////////////////////////////////////////////////////////////
bool DKSFMLWindow::GetClipboard(const void* input, void* output)
{
	DKString text;
	if(!DKUtil::GetClipboard(text)){ return false; }
	*(DKString*)output = text;
	return true;
}

/////////////////////////////////////////////////////////////
bool DKSFMLWindow::GetHandle(const void* input, void* output)
{
#ifdef WIN32
	HWND hwnd = window.getSystemHandle();
	*(HWND*)output = hwnd;
	return true;
#endif
#ifdef MAC
	NSView* nsview = window.getSystemHandle();
	if(!nsview){ return false }
	*(NSView*)output = nsview;
	return true;
#endif
#ifdef LINUX
	GdkWindow* gdk_window = gdk_window_foreign_new(window.getSystemHandle());
	if(!gdk_window){ return false; }
	*(GdkWindow*)output = gdk_window;
	return true;
#endif
	DKLog("DKSFMLWindow::GetHandle(): not implemented on this OS\n", DKWARN);
	return false;
}

/////////////////////////////////////////////////////////////
bool DKSFMLWindow::GetHeight(const void* input, void* output)
{
	sf::Vector2u size = window.getSize();
	*(int*)output = size.y;
	return true;
}

/////////////////////////////////////////////////////////////
bool DKSFMLWindow::GetMouseX(const void* input, void* output)
{
	//TODO
	return false;
}

/////////////////////////////////////////////////////////////
bool DKSFMLWindow::GetMouseY(const void* input, void* output)
{
	//TODO
	return false;
}

////////////////////////////////////////////////////////////
bool DKSFMLWindow::GetWidth(const void* input, void* output)
{
	sf::Vector2u size = window.getSize();
	*(int*)output = size.x;
	return true;
}

////////////////////////////////////////////////////////
bool DKSFMLWindow::GetX(const void* input, void* output)
{
	sf::Vector2i position = window.getPosition();
	*(int*)output = position.x;
	return true;
}

////////////////////////////////////////////////////////
bool DKSFMLWindow::GetY(const void* input, void* output)
{
	sf::Vector2i position = window.getPosition();
	*(int*)output = position.x;
	return true;
}

////////////////////////////////////////////////////////
bool DKSFMLWindow::Hide(const void* input, void* output)
{
	window.setVisible(false);
	return true;
}

////////////////////////////////////////////////////////////////
bool DKSFMLWindow::IsFullscreen(const void* input, void* output)
{
	//TODO
	return false;
}

/////////////////////////////////////////////////////////////
bool DKSFMLWindow::IsVisible(const void* input, void* output)
{
	//TODO
	return false;
}

//////////////////////////////////////////////////////////////
bool DKSFMLWindow::MessageBox(const void* input, void* output)
{
	//TODO
	return false;
}

////////////////////////////////////////////////////////////
bool DKSFMLWindow::Minimize(const void* input, void* output)
{
	//TODO
	return false;
}

///////////////////////////////////////////////////////////
bool DKSFMLWindow::Restore(const void* input, void* output)
{
	//TODO
	return false;
}

////////////////////////////////////////////////////////////////
bool DKSFMLWindow::SetClipboard(const void* input, void* output)
{
	//TODO
	return false;
}

/////////////////////////////////////////////////////////////
bool DKSFMLWindow::SetHeight(const void* input, void* output)
{
	sf::Vector2u size = window.getSize();
	size.y = *(int*)input;
	window.setSize(size);
	return true;
}

///////////////////////////////////////////////////////////
bool DKSFMLWindow::SetIcon(const void* input, void* output)
{
	//TODO
	return false;
	//window.setIcon(unsigned int width, unsigned int height, const Uint8 *pixels);
}

////////////////////////////////////////////////////////////
bool DKSFMLWindow::SetWidth(const void* input, void* output)
{
	sf::Vector2u size = window.getSize();
	size.x = *(int*)input;
	window.setSize(size);	
	return true;
}

////////////////////////////////////////////////////////
bool DKSFMLWindow::SetX(const void* input, void* output)
{
	sf::Vector2i position = window.getPosition();
	position.x = *(int*)input;
	window.setPosition(position);	
	return true;
}

////////////////////////////////////////////////////////
bool DKSFMLWindow::SetY(const void* input, void* output)
{
	sf::Vector2i position = window.getPosition();
	position.y = *(int*)input;
	window.setPosition(position);	
	return true;
}

////////////////////////////////////////////////////////
bool DKSFMLWindow::Show(const void* input, void* output)
{
	window.setVisible(true);
	return true;
}

////////////////////////////////////////////////////////////
bool DKSFMLWindow::Windowed(const void* input, void* output)
{
	//TODO
	return false;
}