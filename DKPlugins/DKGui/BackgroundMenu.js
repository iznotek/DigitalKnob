//////////////////////////////
function BackgroundMenu_Init()
{
	DKCreate("DKGui/BackgroundMenu.html");
	var file = DKWidget_GetAttribute("Background","src");
	//DKLog("file="+file+"\n");
	DKWidget_SetAttribute("BackgroundMenu_Image","src",file);
}

/////////////////////////////
function BackgroundMenu_End()
{
	DKClose("DKGui/BackgroundMenu.html");
}

//////////////////////////////////////
function BackgroundMenu_OnEvent(event)
{
	DKLog("BackgroundMenu_OnEvent("+DK_GetId(event)+","+DK_GetType(event)+","+DK_GetValue(event)+")\n");
}
