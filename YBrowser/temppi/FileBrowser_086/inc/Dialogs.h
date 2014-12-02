/* 
	Copyright (c) 2006-2007, 
	Jukka Silvennoinen
	All rights reserved 
*/
#ifndef __YB_DIALOGS_H
#define __YB_DIALOGS_H

#include <AknDialog.h>


////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////
class CFTypeDialog : public CAknDialog
{
public:
	static TBool RunDlgLD(CDesCArray& aSelectionArray);
private:
	TBool OkToExitL(TInt aKeycode); 
	void PostLayoutDynInitL();
	void PageChangedL(TInt aPageId);
	CFTypeDialog(CDesCArray& aSelectionArray);
	~CFTypeDialog();
private:
	CDesCArray& 	iSelectionArray;
};


#endif //__YB_DIALOGS_H

