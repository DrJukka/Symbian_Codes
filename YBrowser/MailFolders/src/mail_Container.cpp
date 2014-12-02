/* 
	Copyright (c) 2006, 
	Jukka Silvennoinen
	All rights reserved 
*/
#include <coemain.h>
#include <aknnotewrappers.h> 
#include <aknutils.h>
#include <EIKPROGI.H>
#include <AknIconArray.h>
#include <eikapp.h>
#include <eikclbd.h>
#include <EIKLBV.H>
#include <eikmenup.h>
#include <eikmenub.h>
#include <APGCLI.H>
#include <f32file.h>
#include <AknGlobalNote.h>
#include <S32FILE.H>
#include <EIKCLB.H>
#include <smscmds.h>
#include <msvids.h>
#include <akncommondialogs.h> 
#include <miutset.h>
#include <MIUTMSG.H>
#include <EIKBTGPC.H>
#include <aknnavi.h> 
#include <aknnavide.h> 
#include <akntabgrp.h> 
#include <stringloader.h>

#include <mmsvattachmentmanagersync.h>
#include <MMsvAttachmentManager.h>


#include <msvuids.h>

#include "mail_Container.h"
#include "YuccaBrowser.hrh"

#include "Common.h"
#ifdef SONE_VERSION
    #include <YFB_2002B0AA.rsg>
#else
    #ifdef LAL_VERSION

    #else
        #ifdef __YTOOLS_SIGNED
            #include <YuccaBrowser_2000713D.rsg>
        #else
            #include <YuccaBrowser.rsg>
        #endif
    #endif
#endif

/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
CMailBoxContainer* CMailBoxContainer::NewL(const TRect& aRect,MYBrowserFileHandlerUtils& aUtils,CYBrowserFileHandler& aHandler)
    {
    CMailBoxContainer* self = CMailBoxContainer::NewLC(aRect,aUtils,aHandler);
    CleanupStack::Pop(self);
    return self;
    }
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
CMailBoxContainer* CMailBoxContainer::NewLC(const TRect& aRect,MYBrowserFileHandlerUtils& aUtils,CYBrowserFileHandler& aHandler)
    {
    CMailBoxContainer* self = new (ELeave) CMailBoxContainer(aUtils,aHandler);
    CleanupStack::PushL(self);
    self->ConstructL(aRect);
    return self;
    }
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
CMailBoxContainer::CMailBoxContainer(MYBrowserFileHandlerUtils& aUtils,CYBrowserFileHandler& aHandler)
:iUtils(aUtils),iHandler(aHandler)
    {
	
    }
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
CMailBoxContainer::~CMailBoxContainer()
{
	if(iProgressDialog)
	{
		iProgressDialog->ProcessFinishedL(); 
	}

	iProgressDialog = NULL;
	iProgressInfo = NULL;	
	
	RemoveNavipanel();
	
	iFolderArray.ResetAndDestroy();
	
	delete iRFileCopier;
	iRFileCopier = NULL;
	
	delete iSelectionBox;
	iSelectionBox = NULL;

	delete iListArray;
	iListArray = NULL;

	delete iBgContext;
	iBgContext = NULL;
	
	delete iIDArray;
	iIDArray = NULL;
	
	delete iSession;
	iSession = NULL;
}
	
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CMailBoxContainer::ConstructL(const TRect& aRect)
{
    CreateWindowL();
    
    iUtils.GetFileUtils().GetCurrentPath(iCurrPath);
    
    iBgContext = CAknsBasicBackgroundControlContext::NewL(KAknsIIDQsnBgAreaMain,TRect(0,0,aRect.Width(),aRect.Height()), ETrue);
	iBgContext->SetParentPos(aRect.iTl);
	
    SetRect(aRect);
    
  	ActivateL();	
	SetMenuL();
	
	CEikStatusPane*		sp = ((CAknAppUi*)iEikonEnv->EikAppUi())->StatusPane();
	if(sp)
	{		
		if(sp->CurrentLayoutResId() != R_AVKON_STATUS_PANE_LAYOUT_USUAL)//R_AVKON_STATUS_PANE_LAYOUT_SMALL_WITH_SIGNAL_PANE)
		{
			sp->SwitchLayoutL(R_AVKON_STATUS_PANE_LAYOUT_USUAL);
		}
	}
	
	iSession = CMsvSession::OpenSyncL(*this);
	

	iCurrentFolder = 0;
    GetFoldersL();
    MakeNavipanelL();
    
	if(iFolderArray.Count() <= iCurrentFolder)
	{
		iCurrentFolder = 0;
	}
	
	if(iCurrentFolder < iFolderArray.Count())
	{
		if(iFolderArray[iCurrentFolder])
		{
			ReadMailFolderL(iFolderArray[iCurrentFolder]->iMscId);
		}
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMailBoxContainer::RemoveNavipanel(void) 
{
	iTabGroup = NULL;
	
	if (!iNaviPane)
    {
        CEikStatusPane *sp = ((CAknAppUi*)iEikonEnv->EikAppUi())->StatusPane();
        iNaviPane = (CAknNavigationControlContainer *)sp->ControlL(TUid::Uid(EEikStatusPaneUidNavi));
    }

	if(iNaviPane)
	    iNaviPane->Pop(NULL); 
	
	iNaviPane = NULL;
	
    delete iNaviDecoratorForTabs;
    iNaviDecoratorForTabs = NULL;
    delete iNaviDecorator;
    iNaviDecorator = NULL;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMailBoxContainer::MakeNavipanelL(void)
{
	CEikStatusPane *sp = ((CAknAppUi*)iEikonEnv->EikAppUi())->StatusPane();
	iNaviPane = (CAknNavigationControlContainer *)sp->ControlL(TUid::Uid(EEikStatusPaneUidNavi));

	CAknNavigationDecorator* iNaviDecoratorForTabsTemp = iNaviPane->CreateTabGroupL();
	delete iNaviDecoratorForTabs;
    iNaviDecoratorForTabs = NULL;
	iNaviDecoratorForTabs = iNaviDecoratorForTabsTemp;
   	iTabGroup = STATIC_CAST(CAknTabGroup*, iNaviDecoratorForTabs->DecoratedControl());
	iTabGroup->SetTabFixedWidthL(EAknTabWidthWithThreeLongTabs);//EAknTabWidthWithOneTab);

	TBuf<255> FldBuffer;

	for(TInt i=0; i < iFolderArray.Count(); i++)
	{
		FldBuffer.Zero();
		
		if(iFolderArray[i])
		{
			if(iFolderArray[i]->iTitle)
			{
				FldBuffer.Copy(*iFolderArray[i]->iTitle);	
			}
		}
		
		iTabGroup->AddTabL(i,FldBuffer);
	}
	
	if(iTabGroup->TabCount())
	{
		iTabGroup->SetActiveTabByIndex(0);
	}
	iNaviPane->PushL(*iNaviDecoratorForTabs);
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CMailBoxContainer::GetFoldersL(void)
{
	iFolderArray.ResetAndDestroy();
	
	if(iSession)
	{	
		
		CMsvEntry *localServiceEntry = iSession->GetEntryL(KMsvLocalServiceIndexEntryId);
		CleanupStack::PushL(localServiceEntry);
		CMsvEntrySelection *folders = localServiceEntry->ChildrenWithTypeL(KUidMsvFolderEntry);
		CleanupStack::PushL(folders);

		for (TInt i = 0;i <  folders->Count();i++)
		{
			TMsvEntry ExtraFolder = localServiceEntry->ChildDataL((*folders)[i]);
			
			CMailFldItem* NewIttem = new(ELeave)CMailFldItem();
			CleanupStack::PushL(NewIttem);
			
			NewIttem->iTitle = ExtraFolder.iDetails.AllocL();
			NewIttem->iMscId = 	ExtraFolder.Id();

			CleanupStack::Pop();//NewIttem
			iFolderArray.Append(NewIttem);
		}
		
		CleanupStack::PopAndDestroy(2); // localServiceEntry, folders
	
	
		AddEmailFoldersL(iSession);
	}
}

	
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CMailBoxContainer::AddEmailFoldersL(CMsvSession* aSession)
{
	if(aSession)
	{	
		CMsvEntry *localServiceEntry = aSession->GetEntryL(KMsvRootIndexEntryId);//KMsvLocalServiceIndexEntryId);
		CleanupStack::PushL(localServiceEntry);
		CMsvEntrySelection *folders = localServiceEntry->ChildrenL();
		CleanupStack::PushL(folders);

		if(folders->Count() > 1) // first is local folder
		{
			for (TInt i = 1;i <  folders->Count();i++)
			{
				TMsvEntry ExtraFolder = localServiceEntry->ChildDataL((*folders)[i]);
				
				CMailFldItem* NewIttem = new(ELeave)CMailFldItem();
				CleanupStack::PushL(NewIttem);
					
				NewIttem->iTitle = ExtraFolder.iDetails.AllocL();
				NewIttem->iMscId = 	ExtraFolder.Id();
			
				CleanupStack::Pop();//NewIttem
				iFolderArray.Append(NewIttem);
			}
		}
		
		CleanupStack::PopAndDestroy(2); // localServiceEntry, folders
	}
}

/*
-----------------------------------------------------------------------------
RFile GetAttachmentFileL(TMsvAttachmentId aId)
----------------------------------------------------------------------------
*/
void CMailBoxContainer::CopyFileL(const TDesC& aFolderName)
{

	TFileName fileName,FullFilName;

	if(iSession && iSelectionBox && iListArray)
	{	
		TInt CurrItmIndex = iSelCopyItemIndex;
		iSelCopyItemIndex = -1;
		
		if(CurrItmIndex < 0 || CurrItmIndex >= iIDArray->Count())
		{
			CurrItmIndex = iSelectionBox->CurrentItemIndex();
		}
		
		TMsvSelectionOrdering sort;
		sort.SetShowInvisibleEntries(ETrue);
		sort.SetSorting(EMsvSortByDate);
		// Take a handle to the folder entry
		CMsvEntry* parentEntry = CMsvEntry::NewL(*iSession,iCurrentMailBox,sort);
		CleanupStack::PushL(parentEntry);
		
		// A selection of all BT entries
		CMsvEntrySelection* entries = parentEntry->ChildrenL();//ChildrenWithMtmL(KUidMsgTypeBt);
		CleanupStack::PushL(entries);
			
		//Process all entries
		if(CurrItmIndex >= 0 && CurrItmIndex < iIDArray->Count())
		{
			//Get entry
			CMsvEntry* btEntry = iSession->GetEntryL(iIDArray->At(CurrItmIndex).iEnt);
			CleanupStack::PushL(btEntry);
			
			//Then get entrys child
			CMsvEntrySelection* btChildren = btEntry->ChildrenL();
			CleanupStack::PushL(btChildren);
		
			btEntry->SetEntryL(iIDArray->At(CurrItmIndex).iMsg);
									
	
			if (btEntry->HasStoreL())
			{
				CMsvStore* store = btEntry->ReadStoreL();
				CleanupStack::PushL(store);
				
				MMsvAttachmentManager& attMngr = store->AttachmentManagerL();
				CMsvAttachment* attachment = attMngr.GetAttachmentInfoL(iIDArray->At(CurrItmIndex).iAtt);
				if(attachment)
				{
					fileName.Copy(attachment->AttachmentName());
					
					delete attachment;
					attachment = NULL;
				
					TBool OkToSave(EFalse),SkipMe(EFalse);
				
					do
					{
						FullFilName.Zero();
						StringLoader::Load(FullFilName,R_SH_STR_FILENAME);
					
						CAknTextQueryDialog* Dialog = CAknTextQueryDialog::NewL(fileName,CAknQueryDialog::ENoTone);
						Dialog->PrepareLC(R_ASK_NAME_DIALOG);
						Dialog->SetPromptL(FullFilName);
						if(Dialog->RunLD())
						{
							FullFilName.Copy(aFolderName);
							FullFilName.Append(fileName);
			
							if(BaflUtils::FileExists(CEikonEnv::Static()->FsSession(),FullFilName))
							{
								HBufC* TmpText = StringLoader::LoadLC(R_SH_STR_FILOVERRT);
								
								CAknQueryDialog* dlg = CAknQueryDialog::NewL();
								if(dlg->ExecuteLD(R_QUERY,*TmpText))
								{
									TInt Err = CEikonEnv::Static()->FsSession().Delete(FullFilName);
									if(Err == KErrNone)
									{
										OkToSave = ETrue;
									}
									else
									{
										iUtils.GetFileUtils().ShowFileErrorNoteL(FullFilName,Err);
									}
								}
								
								CleanupStack::PopAndDestroy(TmpText);
							}
							else
							{
								OkToSave = ETrue;
							}
						}
						else
						{
							SkipMe = ETrue;
						}
						
					}while(!SkipMe && !OkToSave);
					
					if(OkToSave)
					{
						
						delete iRFileCopier;
						iRFileCopier = NULL;
						
						iRFileCopier = CRFileCopier::NewL(*this,CEikonEnv::Static()->FsSession(),ETrue);
						
						RFile SourceFil(attMngr.GetAttachmentFileL(iIDArray->At(CurrItmIndex).iAtt));
						iRFileCopier->StartCopyL(SourceFil,FullFilName);
						SourceFil.Close();
					}
				}
				
				CleanupStack::PopAndDestroy(store);
			}
			
			CleanupStack::PopAndDestroy(2);
		}				
		CleanupStack::PopAndDestroy(2);
	}

}

/*
-----------------------------------------------------------------------------
RFile GetAttachmentFileL(TMsvAttachmentId aId)
----------------------------------------------------------------------------
*/
void CMailBoxContainer::OpenFileL(TBool /*aOpenWith*/)
{
	if(iSession && iSelectionBox)
	{	
		TInt CurrItmIndex = iSelectionBox->CurrentItemIndex();
		
		TMsvSelectionOrdering sort;
		sort.SetShowInvisibleEntries(ETrue);
		sort.SetSorting(EMsvSortByDate);
		// Take a handle to the folder entry
		CMsvEntry* parentEntry = CMsvEntry::NewL(*iSession,iCurrentMailBox,sort);
		CleanupStack::PushL(parentEntry);
		
		// A selection of all BT entries
		CMsvEntrySelection* entries = parentEntry->ChildrenL();//ChildrenWithMtmL(KUidMsgTypeBt);
		CleanupStack::PushL(entries);
			
		//Process all entries
		if(CurrItmIndex >= 0 && CurrItmIndex < iIDArray->Count())
		{
			//Get entry
			CMsvEntry* btEntry = iSession->GetEntryL(iIDArray->At(CurrItmIndex).iEnt);
			CleanupStack::PushL(btEntry);
			
			//Then get entrys child
			CMsvEntrySelection* btChildren = btEntry->ChildrenL();
			CleanupStack::PushL(btChildren);
		
			btEntry->SetEntryL(iIDArray->At(CurrItmIndex).iMsg);
									
			if (btEntry->HasStoreL())
			{
				CMsvStore* store = btEntry->ReadStoreL();
				CleanupStack::PushL(store);
				
				MMsvAttachmentManager& attMngr = store->AttachmentManagerL();
				CMsvAttachment* attachment = attMngr.GetAttachmentInfoL(iIDArray->At(CurrItmIndex).iAtt);
				if(attachment)
				{
					RFile SourceFil(attMngr.GetAttachmentFileL(iIDArray->At(CurrItmIndex).iAtt));
					iUtils.GetFileUtils().OpenFileWithHandler(SourceFil);
					SourceFil.Close();
				}
				
				CleanupStack::PopAndDestroy(store);
			}
			
			CleanupStack::PopAndDestroy(2);
		}				
		CleanupStack::PopAndDestroy(2);
	}
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CMailBoxContainer::SelectedFilesL(const CDesCArray& /*aArray*/)
{
	MakeNavipanelL();
	// not called, we only use folder selector
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CMailBoxContainer::SelectedFolderL(const TDesC& aFolderName)
{
	iCurrPath.Copy(aFolderName);
	MakeNavipanelL();
	iHandler.SetFocusL(ETrue);
	CopyFileL(aFolderName);
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CMailBoxContainer::SelectorCancelledL(void)
{
	MakeNavipanelL();
	iHandler.SetFocusL(ETrue);
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CMailBoxContainer::RFileCopyProgressL(TInt /*aProgress*/,TInt /*aFinalValue*/)
{
	// not used, since Copier is showing the progress note already	
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/

void CMailBoxContainer::RFileCopyDoneL(CRFileCopier* /*aObject*/,TInt aError)
{
	TBuf<100> Errbuf;
	
	if(aError == KErrNone)
	{
		StringLoader::Load(Errbuf,R_SH_STR_FILCOPIED);			
	}
	else
	{
		StringLoader::Load(Errbuf,R_SH_STR_COPFAILED);
		Errbuf.AppendNum(aError);
	}
	
	ShowNoteL(Errbuf, ETrue);
}


/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CMailBoxContainer::ReadMailFolderL(TMsvId aSelected)
{
	iCurrentMailBox = aSelected;

	delete iIDArray;
	iIDArray = NULL;
	
	if(iSession && iSelectionBox)
	{
		TInt OrgCount = iSelectionBox->Model()->ItemTextArray()->MdcaCount();
		STATIC_CAST(CDesCArray*,iSelectionBox->Model()->ItemTextArray())->Reset();
		
		RPointerArray<CFFileTypeItem> ItemTypeArray  = iUtils.GetFileUtils().GetItemTypeArray();

		iIDArray = new(ELeave)CArrayFixFlat<TMyMailEntry>(10);
	
		TMsvSelectionOrdering sort;
		sort.SetShowInvisibleEntries(ETrue);
		sort.SetSorting(EMsvSortByDate);
		// Take a handle to the folder entry
		CMsvEntry* parentEntry = CMsvEntry::NewL(*iSession,aSelected,sort);
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
				
					if(btEntry->Entry().iMtm == KUidMsgTypePOP3)
					{
						CImEmailMessage* emailMessage = CImEmailMessage::NewL(*btEntry);
						CleanupStack::PushL(emailMessage);
						
						MMsvAttachmentManager& attMngr = emailMessage->AttachmentManager(); 
						GetAttachmentsL(attMngr,*btEntry,(*entries)[ii],btAtt);
						
						CleanupStack::PopAndDestroy(emailMessage);
					}
					else if (btEntry->HasStoreL())
					{
						CMsvStore* store = btEntry->ReadStoreL();
						CleanupStack::PushL(store);
						
						MMsvAttachmentManager& attMngr = store->AttachmentManagerL();
						GetAttachmentsL(attMngr,*btEntry,(*entries)[ii],btAtt);
						
						CleanupStack::PopAndDestroy(store);
					}

				}
				
			}
			
			CleanupStack::PopAndDestroy(2);
		}	
					
		CleanupStack::PopAndDestroy(2);

		CDesCArray* itemArray = STATIC_CAST( CDesCArray* ,iSelectionBox->Model()->ItemTextArray());		
		if(itemArray->Count() > OrgCount)
		{
			iSelectionBox->HandleItemAdditionL();
			iSelectionBox->SetCurrentItemIndex(0); 
		}
		else
		{
			iSelectionBox->HandleItemRemovalL();
			
			if(itemArray->Count())
			{
				iSelectionBox->SetCurrentItemIndex(0); 
			}
		}
		
		UpdateScrollBar(iSelectionBox);
		DrawNow();
	}
}

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CMailBoxContainer::GetAttachmentsL(MMsvAttachmentManager& attMngr,CMsvEntry& aEntry,TMsvId& aMsvId,TMsvId& aBtAtt)
{
	TBuf8<255> FileTypeBuf;
	TBuf<255> Hjelpper;
	TBuf<50> SmallHjelp; 

	TInt attcount = attMngr.AttachmentCount();
	for(TInt j=0; j<attcount; j++)
	{
		CMsvAttachment* attachment = attMngr.GetAttachmentInfoL(j);
		if(attachment)
		{
			if(attachment->AttachmentName().Length())
			{
				FileTypeBuf.Zero();
				
				TInt ReadSize(0);
				
				RFile SourceFil(attMngr.GetAttachmentFileL(attachment->Id()));
				if(SourceFil.Size(ReadSize) == KErrNone)
				{
					if(ReadSize > 250)
					{
						SourceFil.Read(FileTypeBuf,250);
					}
					else
					{
						SourceFil.Read(FileTypeBuf,ReadSize);
					}
				}
				
				SourceFil.Close();
				
				CYBRecognitionResult* Res = new(ELeave)CYBRecognitionResult(); 
				CleanupStack::PushL(Res);
				
				iUtils.GetFileUtils().RecognizeData(*Res,FileTypeBuf,attachment->AttachmentName());
									
				if(Res->iIdString)
				{
					Hjelpper.Copy(Res->iIdString->Des());
				}
				else
				{
					Hjelpper.Copy(attachment->MimeType());
				}
				
				CleanupStack::PopAndDestroy(Res);

				Hjelpper.CopyLC(Hjelpper);
				Hjelpper.TrimAll();
				
				Hjelpper.Num(iUtils.GetIconUtils().GetIconIndex(Hjelpper));
				Hjelpper.Append(_L("\t"));
				Hjelpper.Append(attachment->AttachmentName());
				Hjelpper.Append(_L("\t"));
				
				TTime AttModified(aEntry.Entry().iDate);
				
				TInt Dayyy = (AttModified.DateTime().Day() + 1);
				TInt Monthh= (AttModified.DateTime().Month() + 1);
				TInt Yrrrrr= (AttModified.DateTime().Year());
				TInt Hrrrrr= (AttModified.DateTime().Hour());
				TInt Minsss= (AttModified.DateTime().Minute());

				if(Yrrrrr >= 2000)
				{
					Yrrrrr = Yrrrrr - 2000;
				}
				else if(Yrrrrr >= 1900)
				{
					Yrrrrr = Yrrrrr - 1900;
				}
					
				if(Dayyy < 10)
				{
					Hjelpper.AppendNum(0);
				}
				Hjelpper.AppendNum(Dayyy);
				Hjelpper.Append(_L("/"));
				if(Monthh < 10)
				{
					Hjelpper.AppendNum(0);
				}
				
				Hjelpper.AppendNum(Monthh);
				Hjelpper.Append(_L("/"));
				if(Yrrrrr < 10)
				{
					Hjelpper.AppendNum(0);
				}
				
				Hjelpper.AppendNum(Yrrrrr);
				Hjelpper.Append(_L(" - "));
				
				if(Hrrrrr < 10)
				{
					Hjelpper.AppendNum(0);
				}
				Hjelpper.AppendNum(Hrrrrr);
				Hjelpper.Append(_L(":"));
				
				if(Minsss < 10)
				{
					Hjelpper.AppendNum(0);
				}
				Hjelpper.AppendNum(Minsss);
				Hjelpper.Append(_L(", "));
				
				
				if(attachment->Size() != 0)
				{
					ReadSize = attachment->Size();
				}

				
				if(ReadSize > (1024* 1024 * 10))
				{
					Hjelpper.AppendNum((ReadSize / (1024* 1024)));
					
					StringLoader::Load(SmallHjelp,R_SH_STR_MB);
					Hjelpper.Append(SmallHjelp);
				}
				else if(ReadSize > (1024* 10))
				{
					Hjelpper.AppendNum((ReadSize / 1024));
					StringLoader::Load(SmallHjelp,R_SH_STR_KB);
					Hjelpper.Append(SmallHjelp);
				}
				else
				{
					Hjelpper.AppendNum(ReadSize);
					StringLoader::Load(SmallHjelp,R_SH_STR_BYTES);
					Hjelpper.Append(SmallHjelp);
				}
				
				TMyMailEntry NewEntry;
				
				NewEntry.iEnt = aMsvId; 
				NewEntry.iMsg = aBtAtt;
				NewEntry.iAtt = attachment->Id();

				iIDArray->AppendL(NewEntry);
				
				CDesCArray* itemArray = STATIC_CAST( CDesCArray* ,iSelectionBox->Model()->ItemTextArray());
				itemArray->AppendL(Hjelpper);
			}
		}

		delete attachment;
		attachment = NULL;
	}
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CMailBoxContainer::HandleSessionEventL(TMsvSessionEvent /*aEvent*/, TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/)
{
	
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CMailBoxContainer::DialogDismissedL (TInt /*aButtonId*/)
{		  		
	iProgressDialog = NULL;
	iProgressInfo = NULL;	
}
/*
-----------------------------------------------------------------------
CEikFormattedCellListBox 
-----------------------------------------------------------------------
*/
void CMailBoxContainer::DisplayListBoxL(void)
{	
	TInt curIndex(-1);
	
	if(iSelectionBox)
	{
		curIndex = iSelectionBox->CurrentItemIndex();
	}
	
	delete iSelectionBox;
	iSelectionBox = NULL;
	iSelectionBox   = new( ELeave ) CAknDoubleGraphicStyleListBox();//CAknSingleGraphicStyleListBox();//CAknDoubleNumberStyleListBox();
	iSelectionBox->ConstructL(this,EAknListBoxMarkableList);
		
	if(!iListArray)
	{
		iListArray = new(ELeave)CDesCArrayFlat(10);
	}
	
	iSelectionBox->Model()->SetItemTextArray(iListArray);
    
    iSelectionBox->Model()->SetOwnershipType(ELbmDoesNotOwnItemArray);
	iSelectionBox->CreateScrollBarFrameL( ETrue );
    iSelectionBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
	iSelectionBox->SetRect(Rect());	
	
	TBuf<100> Hejlpper;
    StringLoader::Load(Hejlpper,R_SH_STR_NOATTACHM);
	
	iSelectionBox->View()->SetListEmptyTextL(Hejlpper);
	
	iSelectionBox->ItemDrawer()->ColumnData()->SetIconArray(iUtils.GetIconUtils().GetIconArrayL(GetIconSize()));
	iSelectionBox->ActivateL();
	
	if(iSelectionBox->Model()->ItemTextArray()->MdcaCount() > 0)
	{
		if(curIndex >=0 && curIndex < iSelectionBox->Model()->ItemTextArray()->MdcaCount())
			iSelectionBox->SetCurrentItemIndex(curIndex);
		else
			iSelectionBox->SetCurrentItemIndex(0);
	}
	
	UpdateScrollBar(iSelectionBox);
}
/*
-----------------------------------------------------------------------
CEikFormattedCellListBox 
-----------------------------------------------------------------------
*/
TSize CMailBoxContainer::GetIconSize(void)
{
	TSize IconSiz(13,13);
	TInt Multiple = (CEikonEnv::Static()->ScreenDevice()->SizeInPixels().iWidth / 170);
	TInt MultipleTwo = (CEikonEnv::Static()->ScreenDevice()->SizeInPixels().iHeight/ 200);

	if(MultipleTwo <= Multiple && MultipleTwo > 1)
	{
		IconSiz.iWidth  = IconSiz.iWidth * 2;
		IconSiz.iHeight = IconSiz.iHeight * 2;
	}
	
	return IconSiz;
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CMailBoxContainer::UpdateScrollBar(CEikListBox* aListBox)
    {
    if (aListBox)
        {   
        TInt pos(aListBox->View()->CurrentItemIndex());
        if (aListBox->ScrollBarFrame())
            {
            aListBox->ScrollBarFrame()->MoveVertThumbTo(pos);
            }
        }
    }

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CMailBoxContainer::HandleViewCommandL(TInt aCommand)
    {
    switch(aCommand)
        {
  /*      case EOpenWith:
        	{
   				OpenFileL(ETrue);
        	}
        	break;	*/
		case EMFOpenFile:
			{
				OpenFileL(EFalse);
			}
			break;
        case EMFCopyFile:
        	if(iSelectionBox)
        	{
        		iSelCopyItemIndex = iSelectionBox->CurrentItemIndex();
        		
        		
        		TBuf<100> Hejlpper;
        		StringLoader::Load(Hejlpper,R_SH_STR_SELFOLDER);
        		RemoveNavipanel();
        		iUtils.GetFileUtils().StartSelectionViewL(this,ETrue,iCurrPath,Hejlpper,NULL);
        	}
        	break;
        default:
            break;
        }
  }
	
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMailBoxContainer::SizeChanged()
{
	if ( iBgContext )
	{
		iBgContext->SetRect(Rect());

		if ( &Window() )
		{
			iBgContext->SetParentPos( PositionRelativeToScreen() );
		}
	}
	
	DisplayListBoxL();
}
/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
TTypeUid::Ptr CMailBoxContainer::MopSupplyObject(TTypeUid aId)
{	
	if (iBgContext)
	{
		return MAknsControlContext::SupplyMopObject(aId, iBgContext );
	}
	
	return CCoeControl::MopSupplyObject(aId);
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMailBoxContainer::HandleResourceChange(TInt aType)
{
	TRect rect;
	TBool SetR(EFalse);
	
    if ( aType==KEikDynamicLayoutVariantSwitch )
    {  	
    	SetR = ETrue;
        AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, rect);
    }
  
 
 	if(SetR)
 	{	    
	    SetRect(rect);
 	}
 	
	CCoeControl::HandleResourceChange(aType);
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
TKeyResponse CMailBoxContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
	TKeyResponse Ret = EKeyWasNotConsumed;
	
	switch (aKeyEvent.iCode)
    {
    case EKeyDevice3:
    	CEikonEnv::Static()->AppUiFactory()->MenuBar()->TryDisplayMenuBarL();
		break;
	case EKeyRightArrow:
		if(iTabGroup)
		{
			if(iCurrentFolder < (iTabGroup->TabCount() - 1))
			{
				iCurrentFolder++;
			
				if(iCurrentFolder >=0 && iCurrentFolder < iFolderArray.Count())
				{
					if(iFolderArray[iCurrentFolder])
					{
						iTabGroup->SetActiveTabByIndex(iCurrentFolder);
						ReadMailFolderL(iFolderArray[iCurrentFolder]->iMscId);
					}
				}
			}
		}
		DrawNow();
		break;
	case EKeyLeftArrow:
		if(iTabGroup)
		{
			if(iCurrentFolder > 0)
			{
				iCurrentFolder--;
			
				if(iCurrentFolder >=0 && iCurrentFolder < iFolderArray.Count())
				{
					if(iFolderArray[iCurrentFolder])
					{
						iTabGroup->SetActiveTabByIndex(iCurrentFolder);
						ReadMailFolderL(iFolderArray[iCurrentFolder]->iMscId);
					}
				}
			}
		}
		DrawNow();
		break;
	case EKeyDownArrow:
	case EKeyUpArrow:
	default:
		if(iSelectionBox)
		{
			Ret = iSelectionBox->OfferKeyEventL(aKeyEvent,aType);
		}
		break;
	}
	

	return Ret;
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
TInt CMailBoxContainer::GetSelectedIndexL(void)
{
	TInt Ret(-1);

	if(iSelectionBox)
	{
		Ret = iSelectionBox->CurrentItemIndex();
	}
	
	return Ret;
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMailBoxContainer::SetMenuL(void)
{	
	CEikonEnv::Static()->AppUiFactory()->MenuBar()->SetMenuTitleResourceId(R_MAILBOX_MENUBAR);	
	
	iUtils.GetCba().SetCommandSetL(R_PLUG_MAIN_CBA);				
	iUtils.GetCba().DrawDeferred();
}

/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
void CMailBoxContainer::InitMenuPanelL(TInt aResourceId, CEikMenuPane* aMenuPane)
{			
	if(R_MAILBOX_MENU == aResourceId)
	{
		TInt CurrSel(-1);
		
		if(iSelectionBox)
		{
			CurrSel = iSelectionBox->CurrentItemIndex();
		}
	
		if(CurrSel < 0)
		{
			aMenuPane->SetItemDimmed(EMFCopyFile,ETrue);
			aMenuPane->SetItemDimmed(EMFOpenFile,ETrue);
		}
	}
}
/*
-----------------------------------------------------------------------

-----------------------------------------------------------------------
*/
TInt CMailBoxContainer::CountComponentControls() const
{
	if(iSelectionBox)
	{
		return 1;
	}
	else
		return 0;
}

/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
CCoeControl* CMailBoxContainer::ComponentControl(TInt /*aIndex*/) const
{	
	return iSelectionBox;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMailBoxContainer::ShowNoteL(const TDesC&  aMessage, TBool aError)
{
	if(aError)
	{
		CAknGlobalNote* dialog = CAknGlobalNote::NewLC();
		TInt NoteId(0); 
		NoteId = dialog->ShowNoteL(EAknGlobalErrorNote,aMessage);
		User::After(2000000);
		dialog->CancelNoteL(NoteId);
		CleanupStack::PopAndDestroy(dialog);
	}
	else
	{
		CAknInformationNote* informationNote = new (ELeave) CAknInformationNote;
		informationNote->ExecuteLD(aMessage);
	}	
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CMailBoxContainer::Draw(const TRect& /*aRect*/) const
{
	if(iBgContext)
	{
		CWindowGc& gc = SystemGc();
 		// draw background skin first.
  		MAknsSkinInstance* skin = AknsUtils::SkinInstance();
		AknsDrawUtils::Background( skin, iBgContext, this, gc, Rect() );
	}
}


