/* 
	Copyright (c) 2006-07, 
	Jukka Silvennoinen
	All rights reserved 
*/


#include <mmsvattachmentmanagersync.h>
#include <MMsvAttachmentManager.h>
#include <msvuids.h>
#include <MSVIDS.H>
#include <AknGlobalNote.h>
#include <aknmessagequerydialog.h>

#include "InboxReader.h"
#include "YTools_A000257A_res.rsg"


_LIT(KtxSuperOk		,"Y-SuperDistribution is now enabled\n You can delete the installation file from the inbox now\n");
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
CMailBoxReader::CMailBoxReader()
    {
	
    }
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
CMailBoxReader::~CMailBoxReader()
{
	delete iRFileCopier;
	iRFileCopier = NULL;
	
	delete iSession;
	iSession = NULL;
}
	
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CMailBoxReader::ConstructL(void)
{
	TFindFile MyIconFile(CCoeEnv::Static()->FsSession());
	if(KErrNone == MyIconFile.FindByDir(KtxTargetSisFileName, KNullDesC))	
	{
		//ShowNoteL(_L("Already ok"));
	}
	else
	{
		//ShowNoteL(_L("Starting..."));
		iSession = CMsvSession::OpenSyncL(*this);
		ReadMailFolderL();
	}
}

				
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMailBoxReader::ShowNoteL(const TDesC& aMessage)
{
	CAknGlobalNote* dialog = CAknGlobalNote::NewLC();
	TInt NoteId = dialog->ShowNoteL(EAknGlobalInformationNote,aMessage);
	User::After(2000000);
	dialog->CancelNoteL(NoteId);
	CleanupStack::PopAndDestroy(dialog);;
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CMailBoxReader::RFileCopyProgressL(TInt /*aProgress*/,TInt /*aFinalValue*/)
{
	// not used, since Copier is showing the progress note already	
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CMailBoxReader::RFileCopyDoneL(CRFileCopier* /*aObject*/,TInt aError)
{
	if(aError == KErrNone)
	{
		HBufC* Abbout = KtxSuperOk().AllocLC();
		TPtr Pointter(Abbout->Des());
		
		CAknMessageQueryDialog* dlg = CAknMessageQueryDialog::NewL(Pointter);
		dlg->PrepareLC(R_ABOUT_HEADING_PANE);
		dlg->SetHeaderTextL(_L("Y-SuperDistribution"));  
		dlg->RunLD();
			
		CleanupStack::PopAndDestroy(1);//,Abbout		
	}
	else
	{
		TBuf<100> Errbuf(_L("Y-SuperDistribution error: "));
		Errbuf.AppendNum(aError);
		
		ShowNoteL(Errbuf);	
	}
}


/*
-----------------------------------------------------------------------------

----------------------------------------------------------------------------
*/
void CMailBoxReader::ReadMailFolderL(void )
{	
	TMsvSelectionOrdering sort;
	sort.SetShowInvisibleEntries(ETrue);
	sort.SetSorting(EMsvSortByDate);
	// Take a handle to the folder entry
	CMsvEntry* parentEntry = CMsvEntry::NewL(*iSession,KMsvGlobalInBoxIndexEntryId,sort);
	CleanupStack::PushL(parentEntry);
	
	// A selection of all BT entries
	CMsvEntrySelection* entries = parentEntry->ChildrenL();//ChildrenWithMtmL(KUidMsgTypeBt);
	CleanupStack::PushL(entries);
		
	//Process all entries
	for(TInt ii = 0; ii < entries->Count(); ii++)
	{
		//Get entry
		CMsvEntry* btEntry = iSession->GetEntryL((*entries)[ii]);
		CleanupStack::PushL(btEntry);
		
		//Then get entrys child
		CMsvEntrySelection* btChildren = btEntry->ChildrenL();
		CleanupStack::PushL(btChildren);
		
		TInt childc = btChildren->Count();
		if (childc>0)
		{
			for(TInt i=0; i<childc; i++)
			{
				TMsvId btAtt = (*btChildren)[i];
				btEntry->SetEntryL(btAtt);
			
				if (btEntry->HasStoreL())
				{
					CMsvStore* store = btEntry->ReadStoreL();
					CleanupStack::PushL(store);
					
					MMsvAttachmentManager& attMngr = store->AttachmentManagerL();
					if(GetAttachmentsL(attMngr))
					{
						break;
					}
					
					CleanupStack::PopAndDestroy(store);
				}
			}
		}
		
		CleanupStack::PopAndDestroy(2);
	}	
				
	CleanupStack::PopAndDestroy(2);
}

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
TBool CMailBoxReader::GetAttachmentsL(MMsvAttachmentManager& attMngr)
{
	TBool Ret(EFalse);
	
	TInt attcount = attMngr.AttachmentCount();
	for(TInt j=0; j<attcount; j++)
	{
		CMsvAttachment* attachment = attMngr.GetAttachmentInfoL(j);
		if(attachment)
		{
			if(attachment->AttachmentName().Length())
			{
				TBuf<255> Hjelpper;
				Hjelpper.CopyLC(attachment->AttachmentName());
				Hjelpper.TrimAll();
				
				if(Hjelpper == KtxSisFileName)
				{
					TFindFile MyIconFile(CCoeEnv::Static()->FsSession());
					if(KErrNone == MyIconFile.FindByDir(KtxMyMifFile, KNullDesC))	
					{
						TParsePtrC DrvHelp(MyIconFile.File());
						Hjelpper.Copy(DrvHelp.Drive());
						Hjelpper.Append(KtxTargetSisFileName);
				
						Ret = ETrue;
						delete iRFileCopier;
						iRFileCopier = NULL;
						
						iRFileCopier = CRFileCopier::NewL(*this,CEikonEnv::Static()->FsSession(),ETrue);
						
						RFile SourceFil(attMngr.GetAttachmentFileL(attachment->Id()));
						iRFileCopier->StartCopyL(SourceFil,KtxTargetSisFileName);
						SourceFil.Close();
					}
				}
	
			}
		}

		delete attachment;
		attachment = NULL;
	}
	
	return Ret;
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CMailBoxReader::HandleSessionEventL(TMsvSessionEvent /*aEvent*/, TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/)
{
	
}
