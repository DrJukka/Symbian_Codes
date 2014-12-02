/* 
	Copyright (c) 2006-2007, 
	Jukka Silvennoinen
	All rights reserved 
*/

#include "Dialogs.h"

#include "YuccaBrowser.h"
#include "YuccaBrowser.hrh"
#ifdef __YTOOLS_SIGNED
	#include <YuccaBrowser_2000713D.rsg>
#else
	#include <YuccaBrowser.rsg>
#endif


////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////

TBool CFTypeDialog::RunDlgLD(CDesCArray& aSelectionArray)
	{
	CFTypeDialog* dialog = new (ELeave) CFTypeDialog(aSelectionArray);
	return (dialog->ExecuteLD(R_FILETYPE_DIALOG));
	}


CFTypeDialog::CFTypeDialog(CDesCArray& aSelectionArray)
:iSelectionArray(aSelectionArray)
{
}

CFTypeDialog::~CFTypeDialog()
{

}

////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////

TBool  CFTypeDialog::OkToExitL(TInt /*aKeycode*/)
{

/*	switch (aKeycode)
	{			
	case CbaBut00: //Exit 
		
		return ETrue;
	case CbaBut09: // Send Info as 
		SendAsDialog::RunDlgLD(*message,*aFile,aPoint,iDrive);
		return EFalse;
	case CbaBut01: // Call 
		{
			TBuf<60> Buffer;
			GetLabelText(Buffer,LabelTel);
			MakeACall(Buffer);	
		}
		return EFalse;
	case CbaBut18://fax
		{
			TBuf<60> Buffer;
			GetLabelText(Buffer,LabelFax);
			if(Buffer.Size() < 2)
			{
				eikonEnv->InfoMsg(KTsyNoFax);	
				return EFalse;
			}
			CGMessagingDialog::RunDlgLD(CG_Fax_Messaging,KtxFax,Buffer);
		}
		return EFalse;
	case CbaBut19: //email			
		{
			TBuf<60> Buffer;
			GetLabelText(Buffer,LabelEmail);
			if(Buffer.Size() < 5)
			{
				eikonEnv->InfoMsg(KTsyNoEmail);	
				return EFalse;
			}
			CGMessagingDialog::RunDlgLD(CG_Mail_Messaging,KtxEmail,Buffer);
		}
		return EFalse;
	 }
	 */
return ETrue;
} 



////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////	



void CFTypeDialog::PostLayoutDynInitL()
{

//	ButtonGroupContainer().SetRect(TRect(540,0,640,200));
//	ButtonGroupContainer().SetCommandL (0, CbaBut09,KTxtSendInfo);
//	ButtonGroupContainer().SetCommandL (1, CbaBut99,KTxtEmpty);
//	ButtonGroupContainer().SetCommandL (2, CbaBut99,KTxtEmpty);
//	ButtonGroupContainer().SetCommandL (3, CbaBut00,KTxtBack);
//	ButtonGroupContainer().DrawNow();


//	STATIC_CAST(CEikLabel*,Control(LabelType))->SetSize(LongListboxsize);
//	STATIC_CAST(CEikLabel*,Control(LabellName))->SetSize(LongListboxsize);

}


void CFTypeDialog::PageChangedL(TInt aPageId)
{
		switch (aPageId)
		{
		case EFTypePage1:

			break;
		case EFTypePage2:

			break;
		}
}

