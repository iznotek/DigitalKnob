#pragma once
#ifndef DKDebug_H
#define DKDebug_H
#include "DK/DK.h"
#include "DKCurl/DKCurl.h"

/////////////////////////////////////////
class DKDebug : public DKObjectT<DKDebug>
{
public:
	bool Init();
	bool End();
	
	static bool SendBugReport(const DKString& filename);
	static bool ShowStackTrace();
};


REGISTER_OBJECT(DKDebug, true);

#endif //DKDebug_H