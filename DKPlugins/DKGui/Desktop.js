///////////////////////
function Desktop_Init()
{
	//DKCreate("DKGui/Desktop.html,DKGui/DKOS.html");
	DKCreate("DKGui/Desktop.html");
	DKAddEvent("Background", "contextmenu", Desktop_OnEvent);
}

//////////////////////
function Desktop_End()
{
	DKClose("DKGui/Desktop.html");
}

///////////////////////////////
function Desktop_OnEvent(event)
{
	DKLog("Desktop_OnEvent("+DK_GetId(event)+","+DK_GetType(event)+","+DK_GetValue(event)+")\n");
	
	if(DK_Id(event, "Background") && DK_Type(event, "contextmenu")){
		DKCreate("DKGui/DesktopMenu.js", function(){});
	}
}