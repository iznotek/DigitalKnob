var currentFile;

/////////////////////////
function DKPaint_Init()
{
	DKCreate("DKPaint/DKPaint.html");
	DKAddEvent("DKPaint_Save", "click", DKPaint_OnEvent);
	DKAddEvent("DKPaint_Text", "contextmenu", DKPaint_OnEvent);
	DKAddEvent("DKPaint_File", "click", DKPaint_OnEvent);
}

////////////////////////
function DKPaint_End()
{
	DKClose("DKPaint/DKPaint.html");
}

///////////////////////////////
function DKPaint_OnEvent(event)
{
	if(DK_Id(event,"DKPaint_Save")){
		DKNotepad_Save();
	}
	if(DK_Type(event, "contextmenu")){
		DKCreate("DKPaint/DKPaintMenu.js");
	}
	if(DK_Id(event,"DKPaint_File")){
		DKCreate("DKPaint/DKPaintFile.js");
	}
}

/////////////////////////
function DKPaint_Save()
{
	var text = DKWidget_GetValue("DKPaint_Text");
	
	//TODO
	//DKCreate("DKMessage/DKMessage.js");
	//DKMessageBox("", "ShowMessage", "Saved File");
}

/////////////////////////////
function DKPaint_Open(file)
{
	DKLog("DKPaint_Open("+file+") \n");
	currentFile = file;
	DKWidget_SetAttribute("DKPaint_Image", "src", file);
}
