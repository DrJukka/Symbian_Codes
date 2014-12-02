/* 
	Copyright (c) 2006-07, 
	Jukka Silvennoinen
	All rights reserved 
*/
#ifndef __MYMAILBOX_READER_H__
#define __MYMAILBOX_READER_H__


#include <coecntrl.h>
#include <AknProgressDialog.h>
#include <aknlists.h> 
#include <aknsfld.h> 
#include <msvapi.h> 
#include <AknsBasicBackgroundControlContext.h> 
#include <aknsdrawutils.h>
#include <aknscontrolcontext.h>
#include <AknsSkinInstance.h>
#include <aknsutils.h>
#include <CMSVATTACHMENT.H>
#include <maknfilefilter.h> 

#include "RFileCopier.h"

_LIT(KtxSisFileName					,"y_tasks_3rded.sis");

_LIT(KtxMyMifFile					,"\\resource\\apps\\YTools_A000257B_aif.mif");
_LIT(KtxTargetSisFileName			,"\\Private\\A000257B\\Y_Tasks_3rdEd.SIS");
_LIT(KtxTargetTxtFileName			,"\\Private\\A000257B\\Download.txt");

/*! 
  @class CMailBoxReader
  
  @discussion An instance of the Application View object for the HelloWorld 
  example application
  */
class CMailBoxReader : public CCoeControl,MMsvSessionObserver
,MYBrowserRFileCopyObserver
    {
public:
	void ConstructL();
    CMailBoxReader();
     ~CMailBoxReader();
public:  
	void ShowNoteL(const TDesC&  aMessage);
protected:
	void RFileCopyProgressL(TInt aProgress,TInt aFinalValue);
	void RFileCopyDoneL(CRFileCopier* aObject,TInt aError);
	virtual void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3);
private:
	TBool GetAttachmentsL(MMsvAttachmentManager& attMngr);
	void ReadMailFolderL(void);
private:		
	CMsvSession* 	iSession;
	CRFileCopier*	iRFileCopier;
    };


#endif // __MYMAILBOX_READER_H__
