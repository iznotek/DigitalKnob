//////////////////////////////
function DKMenuRightApp_Init()
{
	DKCreate("DKDev/DKMenuRightApp.html,DKMenuRight.html");
	DKAddEvent("SaveUserFile", "click", DKMenuRightApp_OnEvent);
	DKAddEvent("DKMenuRightApp.html", "Update", DKMenuRightApp_OnEvent);
	
	DKMenuRightApp_Update();
}

/////////////////////////////
function DKMenuRightApp_End()
{
	DKClose("DKMenuRightApp.html");
}

//////////////////////////////////////
function DKMenuRightApp_OnEvent(event)
{
	//DKLog("DKMenuRightApp_OnEvent("+event+") \n");
	
	if(DK_Id(event, "SaveUserFile")){
		DKMenuRightApp_SaveUserFile();
		return;
	}
	if(DK_Type(event, "Update")){
		DKMenuRightApp_Update();
		return;
	}
}

////////////////////////////////
function DKMenuRightApp_Update()
{
	//DKLog("DKMenuRightApp_Update() \n", DKDEBUG);
	var apppath = DKAssets_GetDataPath();
	var userfile = DKFile_FileToString(apppath+"User.js");
	DKWidget_SetValue("userfile", userfile);
}

//////////////////////////////////////
function DKMenuRightApp_SaveUserFile()
{
	//DKLog("DKMenuRightApp_SaveUserFile() \n");
	var apppath = DKAssets_GetDataPath();
	var value = DKWidget_GetValue("userfile");
	DKFile_StringToFile(value, apppath+"User.js");
}

