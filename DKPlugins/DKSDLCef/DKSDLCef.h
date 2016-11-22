#pragma once
#ifndef DKSDLCef_H
#define DKSDLCef_H

#include "DK.h"
#include "DKApp.h"
#include <include/cef_client.h>
#include <include/cef_render_handler.h>
#include <include/cef_browser_process_handler.h>
#include <include/cef_display_handler.h>
#include "DKSDLCefKeyboard.h"
#include "DKSDLWindow.h"
#include "SDL.h"
#include "DKCef.h"

#ifndef WIN32
#define  UNREFERENCED_PARAMETER(P) (P)
#endif 

class DKSDLCefHandler;

///////////////////////////////////////////
class DKSDLCef : public DKObjectT<DKSDLCef>
{
public:
	void Init();
	void End();

	void* OnResize(void* data);
	void* GetTexture(void*);
	void SetupCef();

	bool handle(SDL_Event *event);
	void Draw();
	bool transparentPixel(SDL_Event *event);
	CefBrowserHost::MouseButtonType getCefMouseButton(int button);
	bool getScrollDeltas(SDL_Event *event, float &deltaX, float &deltaY);
	//void setSize(unsigned int width, unsigned int height);
	
	DKSDLWindow* dkSdlWindow;
	DKCef* dkCef;
	DKString id;
	bool _mouseLMBdown;
	DKSDLCefKeyboard _keyAdapter;
	float _scrollFactor;

	SDL_Texture* cef_image;
	SDL_Texture* background_image;
	SDL_Texture* popup_image;
	CefRect popup_rect;
	DKSDLCefHandler* cefHandler;
};

/*/
///////////////////////////////////////
class DKV8Handler : public CefV8Handler 
{
public:
	DKV8Handler(){}
	virtual bool Execute(const CefString& name,
                       CefRefPtr<CefV8Value> object,
                       const CefV8ValueList& arguments,
                       CefRefPtr<CefV8Value>& retval,
                       CefString& exception) OVERRIDE {
		if (name == "myfunc") {
			// Return my string value.
			retval = CefV8Value::CreateString("My Value!");
			return true;
		}

		// Function does not exist.
		return false;
	}

	// Provide the reference counting implementation for this class.
	IMPLEMENT_REFCOUNTING(DKV8Handler);
};
*/

///////////////////////////////////////////////////////////////////////
class DKSDLCefHandler : public CefClient, public CefRenderHandler, 
	public CefLoadHandler, public CefLifeSpanHandler, public CefContextMenuHandler,
	public CefDownloadHandler, public CefDisplayHandler, public CefV8Handler, public CefRenderProcessHandler
{
public:
	DKSDLCefHandler(){}
	DKSDLWindow* dkSdlWindow;
	DKCef* dkCef;
	DKSDLCef* dkSdlCef;


	virtual CefRefPtr<CefRenderHandler> GetRenderHandler(){ return this; }
	virtual CefRefPtr<CefLoadHandler> GetLoadHandler(){ return this; }
	virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler(){ return this; }
	virtual CefRefPtr<CefContextMenuHandler> GetContextMenuHandler(){ return this; }
	virtual CefRefPtr<CefDownloadHandler> GetDownloadHandler(){ return this; }
	virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler(){ return this; }
	virtual CefRefPtr<CefV8Handler> GetCefV8Handler(){ return this; }
	virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler(){ return this; }
	
	/////////////////////////////////////////
	void DoFrame(){ CefDoMessageLoopWork(); }

	//////////////////////////////////////////////////////////////
	bool GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect)
	{
		rect = CefRect(0, 0, dkCef->width, dkCef->height);
		return true;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList &dirtyRects, const void *buffer, int width, int height)
	{
		//if(!dkCef->current_browser){ return; }
		if(browser->GetIdentifier() != dkCef->current_browser->GetIdentifier()){ return; }
		if(type == PET_VIEW){
			if(dirtyRects.size() == 0){ return; }
			SDL_Surface* surface = SDL_GetWindowSurface(dkSdlWindow->sdlwin);
			if(!surface){ return; }

			int w, h;
			SDL_QueryTexture(dkSdlCef->cef_image, NULL, NULL, &w, &h);
			if(w != width || h != height){
				dkSdlCef->cef_image = SDL_CreateTexture(dkSdlWindow->sdlren, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, width, height);
			}

			if (!dkSdlCef->background_image) {
				dkSdlCef->background_image = SDL_CreateTexture(dkSdlWindow->sdlren, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, width, height);
			}
			int w2, h2;
			SDL_QueryTexture(dkSdlCef->background_image, NULL, NULL, &w2, &h2);
			if (w2 != width || h2 != height) {
				dkSdlCef->background_image = SDL_CreateTexture(dkSdlWindow->sdlren, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, width, height);
			}

			if(SDL_LockTexture(dkSdlCef->background_image, NULL, reinterpret_cast<void**>(&surface->pixels), &surface->pitch) == 0){
				//copies whole cef bitmap to sdl texture
				std::memcpy(surface->pixels, buffer, width * height * 4);
				SDL_UnlockTexture(dkSdlCef->background_image);
			}
		}
		else if(type == PET_POPUP){ //FIXME
			//if(dirtyRects.size() == 0){ return; }
			if(!dkSdlCef->popup_image){
				dkSdlCef->popup_image = SDL_CreateTexture(dkSdlWindow->sdlren, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, width, height);
			}
			int w, h;
			SDL_QueryTexture(dkSdlCef->popup_image, NULL, NULL, &w, &h);
			if(w != width || h != height){
				dkSdlCef->popup_image = SDL_CreateTexture(dkSdlWindow->sdlren, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, width, height);
			}
			void* mPixels;
			int mPitch;
			if(SDL_LockTexture(dkSdlCef->popup_image, NULL, &mPixels, &mPitch) == 0){
				//copies popup bitmap to sdl texture
				std::memcpy(mPixels, buffer, width * height * 4);
				SDL_UnlockTexture(dkSdlCef->popup_image);
			}
		}

		if(dkSdlCef->cef_image) {	
			SDL_SetRenderTarget(dkSdlWindow->sdlren, dkSdlCef->cef_image);
			SDL_RenderCopy(dkSdlWindow->sdlren, dkSdlCef->background_image, NULL, NULL);
			if(dkSdlCef->popup_image){
				SDL_Rect popup;
				popup.x = dkSdlCef->popup_rect.x;
				popup.y = dkSdlCef->popup_rect.y;
				popup.w = dkSdlCef->popup_rect.width;
				popup.h = dkSdlCef->popup_rect.height;
				SDL_RenderCopy(dkSdlWindow->sdlren, dkSdlCef->popup_image, NULL, &popup);	
			}
			SDL_SetRenderTarget(dkSdlWindow->sdlren, NULL);
		}
	}

	///////////////////////////////////////////////////////////
	void OnPopupShow(CefRefPtr<CefBrowser> browser, bool show){
		if(!show){
			dkSdlCef->popup_image = NULL;
			return;
		}
	}

	/////////////////////////////////////////////////////////////////////
	void OnPopupSize(CefRefPtr<CefBrowser> browser, const CefRect& rect){
		if(rect.width <= 0 || rect.height <= 0){ return; }
		dkSdlCef->popup_rect = rect;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void OnLoadingStateChange(CefRefPtr<CefBrowser> browser, bool isLoading, bool canGoBack, bool canGoForward){
		//DKLog("BrowserClient::OnLoadingStateChange() \n");
		for(unsigned int i=0; i<dkCef->browsers.size(); ++i){
			if(browser->GetIdentifier() == dkCef->browsers[i]->GetIdentifier()){
				DKEvent::SendEvent("GLOBAL", "DKCef_OnLoadingStateChange", toString(i));
				return;
			}
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void OnLoadError( CefRefPtr< CefBrowser > browser, CefRefPtr< CefFrame > frame, CefLoadHandler::ErrorCode errorCode, const CefString& errorText, const CefString& failedUrl){ 
		DKEvent::SendEvent("GLOBAL", "DKCef_OnLoadError", toString(errorCode));
	}

	////////////////////////////////////////////////////////////////////////////
	void OnFullscreenModeChange(CefRefPtr<CefBrowser> browser, bool fullscreen){
		if(fullscreen){
			dkCef->fullscreen = true;
			//SDL_SetWindowFullscreen(dkSdlWindow->sdlwin, SDL_WINDOW_FULLSCREEN_DESKTOP);
		}
		else{
			dkCef->fullscreen = false;
			//SDL_SetWindowFullscreen(dkSdlWindow->sdlwin, 0);
		}
		DKEvent::SendEvent("GLOBAL", "DKCef_OnFullscreen", toString(fullscreen));
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool OnBeforePopup(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& target_url, const CefString& target_frame_name, CefLifeSpanHandler::WindowOpenDisposition target_disposition, bool user_gesture, const CefPopupFeatures& popupFeatures, CefWindowInfo& windowInfo, CefRefPtr<CefClient>& client, CefBrowserSettings& settings, bool* no_javascript_access)
	{
		//DKLog("DKSDLCefHandler::OnBeforePopup() \n", DKINFO);
		//DKEvent::SendEvent("GLOBAL", "DKCef_OnBeforePopup", target_url);
		dkCef->queue_new_browser = target_url;
		return true;
		//return false;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void OnBeforeDownload(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDownloadItem> download_item, const CefString& suggested_name, CefRefPtr<CefBeforeDownloadCallback> callback)
	{
		UNREFERENCED_PARAMETER(browser);
		UNREFERENCED_PARAMETER(download_item);
		callback->Continue(suggested_name, true);
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void OnBeforeContextMenu(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model)
	{
		DKLog("DKSDLCefHandler::OnBeforeContextMenu("+dkCef->id+") \n", DKINFO);
		model->Clear(); //remove original context menu
		
		DKString data;
		data += params->GetSelectionText();
		data += ",";
		data += params->GetSourceUrl();
		data += ",";
		data += params->GetLinkUrl();

		SendEvent("GLOBAL", "DKCef_ContextMenu", data);
	}

	// FIXME: OnContextCreated never called
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context)
	{
		DKLog("DKSDLCefHandler::OnContextCreated("+dkCef->id+") \n", DKINFO);
		// Retrieve the context's window object.
		CefRefPtr<CefV8Value> object = context->GetGlobal();
		CefRefPtr<CefV8Handler> handler = (CefV8Handler*)this;
		CefRefPtr<CefV8Value> func = CefV8Value::CreateFunction("myfunc", handler);
		//object->SetValue("register", CefV8Value::CreateFunction("register", handler), V8_PROPERTY_ATTRIBUTE_NONE);
	}

	////////////////////////////////////////
	bool Execute(const CefString& name,
			CefRefPtr<CefV8Value> object,
			const CefV8ValueList& arguments,
			CefRefPtr<CefV8Value>& retval,
			CefString& exception) OVERRIDE {

			DKLog("DKSDLCefHandler::Execute("+dkCef->id+") \n", DKINFO);
			if (name == "myfunc") {
				// Return my string value.
				retval = CefV8Value::CreateString("My Value!");
				return true;
			}

			// Function does not exist.
			return false;
	}

	IMPLEMENT_REFCOUNTING(DKSDLCefHandler);
};

REGISTER_OBJECT(DKSDLCef, false);

#endif //DKSDLCef_H