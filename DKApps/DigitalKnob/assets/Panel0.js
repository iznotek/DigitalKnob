//////////////////////
function Panel0_Init()
{
	DKCreate("Panel0.html");
	//DKCreate("Content.html,Panel0_content");
	DKCreate("OsInfo.js");
}

/////////////////////
function Panel0_End()
{
	DKClose("Panel0.html");
}

//////////////////////////////
function Panel0_OnEvent(event)
{
	//DKLog("Panel0_OnEvent("+event+") \n", DKDEBUG);

}