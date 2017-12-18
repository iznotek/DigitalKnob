#include "DK/stdafx.h"
#include "DKSDLText.h"
#include "DK/DKFile.h"

//////////////////////
void DKSDLText::Init()
{
	dkSdlWindow = DKSDLWindow::Get("DKSDLWindow0");
	if(!dkSdlWindow){
		return; //SDL window not available
	}

	TTF_Init();
	DKString file = DKFile::local_assets+"DKSDLText/arial.ttf";
	font = TTF_OpenFont(file.c_str(), 20);
	color = {100, 100, 255};
	SetText(toString("Test String"));

	DKSDLWindow::AddDrawFunc(&DKSDLText::Draw, this);
}

/////////////////////
void DKSDLText::End()
{
	//SDL_FreeSurface(surface);
	//SDL_DestroyTexture(texture);
	//TTF_CloseFont(font);
}

//////////////////////////////////////
void DKSDLText::SetText(DKString text)
{
	surface = TTF_RenderText_Solid(font, text.c_str(), color);
	texture = SDL_CreateTextureFromSurface(dkSdlWindow->sdlren, surface);
	SDL_FreeSurface(surface);
}

//////////////////////
void DKSDLText::Draw()
{
	//Draw fps
	unsigned int fps;
	DKUtil::GetFps(fps);
	DKString fpsString = toString(fps)+"fps";
	SetText(fpsString);
	int texW = 0;
	int texH = 0;
	SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
	int left = 5;
	int top = dkSdlWindow->height - texH;
	SDL_Rect dstrect = {left, top, texW, texH};
	SDL_RenderCopy(dkSdlWindow->sdlren, texture, NULL, &dstrect);
	SDL_DestroyTexture(texture);
}