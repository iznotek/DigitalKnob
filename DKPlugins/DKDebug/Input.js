
/////////////////////
function Input_Init()
{
	DKCreate("DKDebug/Input.html");
	DKAddEvent("Input_Text", "keydown", Input_OnEvent);
}

////////////////////
function Input_End()
{
	DKClose("DKDebug/Input.html");
}

/////////////////////////////
function Input_OnEvent(event)
{
	//DKLog("Input_OnEvent("+event+")\n", DKINFO);
	if(DK_Id(event, "Input_Text")){
		var key = DKWidget_GetValue(event);
		//DKLog("Input_Text: key="+key+"\n", DKINFO);
		if(key != 13){ return; }
		Input_Run(DKWidget_GetValue("Input_Text"))
	}
}

///////////////////////////
function Input_Run(command)
{
	DKLog("Input_Run("+command+")\n", DKINFO);
	DK_RunJavascript(command);
}
