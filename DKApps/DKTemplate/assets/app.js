var USE_CEF = 0;     //Desktop
var USE_WEBVIEW = 1; //Android, iOS?
var USE_SDL = 1;     //Use with caution
var USE_ROCKET = 1;  //Use with caution
var DKApp_url = "file:///"+DKAssets_LocalAssets()+"index.html";
//var DKApp_url = "http://digitalknob.com/DKTemplate";


DKCreate("DK/init.js", function(){});

///////////////////////
function app_LoadPage()
{
	DKLog("app_LoadPage()\n");
	
	DKCreate("DKWindow/DKWindow.js", function(){
	DKCreate("DKScale/DKScale.js", function(){
	DKCreate("DKGui/DKFrame.js", function(){
	DKCreate("DKGui/DKMenu.js", function(){
	DKCreate("DKDebug/DKDebug.js", function(){
	DKCreate("DKEditor/DKEditor.js", function(){
	//DKCreate("DKHook/DKHook.js", function(){
		//DKFrame_Widget("DKHook/DKHook.html");
		DKCreate("DKStats/DKStats.js", function(){
			DKFrame_Widget("DKStats/DKStats.html");
		});
		//DKCreate("DKRocketAudio/DKRocketAudio.js", function(){
		//	DKFrame_Widget("DKRocketAudio/DKRocketAudio.html");
		//	DKRocketAudio_Open("C:/test.ogg");
		//});
		
	//});		
	});
	});
	});
	});
	});
	});
}