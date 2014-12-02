/* 
	Copyright (c) 2006, 
	Jukka Silvennoinen
	All rights reserved 
*/
#include <AknIconArray.h>
#include <EIKDEF.H>
#include <EIKENV.H>
#include <COEMAIN.H>
#include <GULICON.H>
#include <APGCLI.H>
#include <BAUTILS.H>
#include <EIKFUTIL.H>
#include <aknglobalnote.h> 
#include <AknLists.h>
#include <AknPopup.h>
#include <documenthandler.h> 
#include <AknSelectionList.h>
#include <stringloader.h> 
#include <avkon.mbg>


#include "Folder_reader.h"// for CFileItem
#include "IconHandler.h"

const TInt KMaxDisplayedFullNameLenInMsg 	= 20;

#include "YuccaBrowser.h"
#include "YuccaBrowser.hrh"
#ifdef __YTOOLS_SIGNED
	#include <YuccaBrowser_2000713D.rsg>
#else
	#include <YuccaBrowser.rsg>
#endif



/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CIconHandler::CIconHandler(MYBrowserFileUtilsHelp& aFileUtilsHelp,MAknServerAppExitObserver* aParent)
:iParent(aParent),iYBrowserFileUtilsHelp(aFileUtilsHelp)
{	
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CIconHandler::~CIconHandler()
{
	delete iDocumentHandler;
	iDocumentHandler = NULL;
	
	delete iGeneralFile;
	iGeneralFile = NULL;

	iItemTypeArray.ResetAndDestroy();
	iItemIconArray.ResetAndDestroy();
	iFHandlerArray.ResetAndDestroy();
	iExtraFileRecognizer.ResetAndDestroy();
	REComSession::FinalClose();
}

/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CIconHandler::SetNaviTextL(const TDesC& aText)
{
	iYBrowserFileUtilsHelp.SetNaviTextL(aText);
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CIconHandler::StartSelectionViewL(MYbSelectorCallBack* aCallback,TBool aFolderSelector,const TDesC& aFolder,const TDesC& aTitle,CDesCArrayFlat* aTypeFilter)
{
	iYBrowserFileUtilsHelp.StartSelectionViewL(aCallback,aFolderSelector,aFolder,aTitle,aTypeFilter);
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CIconHandler::GetCurrentPath(TDes& aFullPath)
{
	iYBrowserFileUtilsHelp.GetCurrentPath(aFullPath);
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CIconHandler::ShowFileErrorNoteL(const TDesC& aFileName, TInt aError) 
{	
	TBuf<160> Hjelpper;
	TFileName errorMsg;
	TBuf<KMaxDisplayedFullNameLenInMsg> abbrevSourceName;
	EikFileUtils::AbbreviateFileName(aFileName, abbrevSourceName);

	TEntry FileEntry;	
	CCoeEnv::Static()->FsSession().Entry(aFileName,FileEntry);
	
	if (aError == KErrAlreadyExists)
	{
		StringLoader::Load(Hjelpper,R_SH_STR_ALREDYEXIST);
		errorMsg.Copy(abbrevSourceName);
		errorMsg.Append(Hjelpper);
	}
	else if (aError == KErrInUse)
	{
		StringLoader::Load(Hjelpper,R_SH_STR_ISOPEN);
		errorMsg.Copy(abbrevSourceName);
		errorMsg.Append(Hjelpper);
	}
	else if (aError == KErrAccessDenied)
	{
		if(FileEntry.IsReadOnly())
			StringLoader::Load(Hjelpper,R_SH_STR_ISREADONLY);
		else
			StringLoader::Load(Hjelpper,R_SH_STR_ACCDENIED);
	
		errorMsg.Copy(abbrevSourceName);
		errorMsg.Append(Hjelpper);
	}
	else if (aError == KErrDiskFull)
	{
		StringLoader::Load(errorMsg,R_SH_STR_DISKFULL);
	}
	else if (aError == KErrNotReady)
	{
		StringLoader::Load(errorMsg,R_SH_STR_DISCKNOTREADY);
	}
	else if (aError == KErrCorrupt)
	{
		StringLoader::Load(errorMsg,R_SH_STR_CORRDISCK);
	}
	else if (aError == KErrBadName)
	{
		StringLoader::Load(Hjelpper,R_SH_STR_INVALIDNAME);
		errorMsg.Copy(abbrevSourceName);
		errorMsg.Append(Hjelpper);
	}
	else if (aError == KErrNoMemory)
	{
		StringLoader::Load(Hjelpper,R_SH_STR_NOTMEMFORFF);
		errorMsg.Copy(Hjelpper);
		errorMsg.Append(abbrevSourceName);
	}
	else
	{	
		StringLoader::Load(Hjelpper,R_SH_STR_OPERFAILED);
		errorMsg.Format(Hjelpper,&abbrevSourceName, aError);
	}
	
	if(errorMsg.Length())
	{
		ShowNoteL(errorMsg,ETrue);
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CIconHandler::ShowNoteL(const TDesC&  aMessage, TBool aError)
{
	CAknGlobalNote* dialog = CAknGlobalNote::NewLC();
		
	TInt NoteId(0); 
	
	if(aError)
		NoteId = dialog->ShowNoteL(EAknGlobalErrorNote,aMessage);
	else
		NoteId = dialog->ShowNoteL(EAknGlobalInformationNote,aMessage);
	
	User::After(1200000);
	dialog->CancelNoteL(NoteId);
	CleanupStack::PopAndDestroy(dialog);
}

/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
TInt CIconHandler::OpenFileWithHandler(RFile& aFile)
{
	TInt Ret(KErrNone);
	
	TFileName FilName;
	TInt FilSize(0);
	
	aFile.Name(FilName);
	if(aFile.Size(FilSize)  == KErrNone)
	{
		CYBRecognitionResult* Rec = new(ELeave)CYBRecognitionResult();

		TBuf8<255> FileBuffer;
			
		if(FilSize > 255)
		{
			FilSize = 255;
		}
		
		if(aFile.Read(FileBuffer, FilSize) == KErrNone)
		{
			RecognizeData(*Rec,FileBuffer,FilName);
		}

		TPoint Partial(-1,-1);
		TPoint Full(-1,-1);
		TBuf<255> FileType,TmpFileType;	
		
		if(Rec->iIdString)
		{
			for(TInt i=0; i < iFHandlerArray.Count(); i++)
			{
				if(iFHandlerArray[i] && Rec->iIdString->Des().Length())
				{
					
					for(TInt o=0; o < iFHandlerArray[i]->iSupArray.Count(); o++)
					{
						if(iFHandlerArray[i]->iSupArray[o])
						{
							if(iFHandlerArray[i]->iSupArray[o]->iFlags & EYBFilehandlerSupportsRFile)
							{	
								if(iFHandlerArray[i]->iSupArray[o]->iIdString)
								{
									TmpFileType.CopyLC(iFHandlerArray[i]->iSupArray[o]->iIdString->Des());
									TmpFileType.TrimAll();
									if(TmpFileType.Length())
									{
										if(iFHandlerArray[i]->iSupArray[o]->iPartialSting)
										{
											if(Rec->iIdString->Des().Length() > TmpFileType.Length())
											{
												if(Rec->iIdString->Des().Left(TmpFileType.Length()) == TmpFileType)
												{
													FileType.Copy(TmpFileType);
													Partial.iX = i;
													Partial.iY = o;
													break;
												}
											}
										}
										else if(Rec->iIdString->Des() == TmpFileType)
										{
											FileType.Copy(TmpFileType);
											
											Full.iX = i;
											Full.iY = o;
											break;
										}
									}
								}
							}
						}
					}
				}
			}
		}
		
		FileBuffer.Zero();
		
		if(Rec)
		{
			if(Rec->iIdString)
			{
				FileBuffer.Copy(Rec->iIdString->Des());
			}
		}
		
		delete Rec;
		Rec = NULL;
		
		
		
		TBool Added(EFalse);
		
		if(Full.iX >= 0 && Full.iY >= 0 && iFHandlerArray.Count() > Full.iX)
		{
			if(iFHandlerArray[Full.iX])
			{
				if(iFHandlerArray[Full.iX])
				{
					Added = OpenWithHandlerL(*iFHandlerArray[Full.iX],aFile,FileType);		
				}
			}
		}
		else if(Partial.iX >= 0 && Partial.iY >= 0 && iFHandlerArray.Count() > Partial.iX)
		{
			if(iFHandlerArray[Partial.iX])
			{
				if(iFHandlerArray[Partial.iX])
				{
					Added = OpenWithHandlerL(*iFHandlerArray[Partial.iX],aFile,FileType);		
				}
			}
		}
	
		if(!Added && iDocumentHandler)
		{
			RApaLsSession ls;
			User::LeaveIfError(ls.Connect());
			CleanupClosePushL(ls);

			TDataType empty(FileBuffer);
	
			TRAP(Ret,
				if(iDocumentHandler->CanOpenL(empty))
				{
					TUid HandlerAppUid;
					TPckgBuf<TApaAppCapability> TApaAppCapabilityBuf;
					
					iDocumentHandler->HandlerAppUid(HandlerAppUid);
					
					ls.GetAppCapability(TApaAppCapabilityBuf,HandlerAppUid);

			        iDocumentHandler->SetExitObserver(iParent); 
			        
			        if(TApaAppCapabilityBuf().iEmbeddability == TApaAppCapability::ENotEmbeddable)
			        {
			        	iDocumentHandler->OpenFileL(aFile,empty);
			        }
			        else
			        {
    			    	iDocumentHandler->OpenFileEmbeddedL(aFile,empty);
			        }
				}
    			);
		
			CleanupStack::PopAndDestroy(1);//ls,
			
			if(Ret != KErrNone)
			{
				delete iDocumentHandler;
				iDocumentHandler = NULL;
				iDocumentHandler =  CDocumentHandler::NewL();
				iDocumentHandler->SetExitObserver(iParent);
			}
		}
	}
	else
	{
		Ret = KErrNotFound;
	}
	
	return Ret;
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
TInt CIconHandler::OpenFileWithHandler(const TDesC& aFileName) 
{	
	TInt Ret(KErrNone);
	
	if(aFileName.Length()
	&& BaflUtils::FileExists(CCoeEnv::Static()->FsSession(),aFileName))
	{
		CYBRecognitionResult* Rec = new(ELeave)CYBRecognitionResult();
		RecognizeFile(*Rec,aFileName);
	
		TPoint Partial(-1,-1);
		TPoint Full(-1,-1);
		TBuf<255> FileType,TmpFileType;	
		
		if(Rec->iIdString)
		{
			for(TInt i=0; i < iFHandlerArray.Count(); i++)
			{
				if(iFHandlerArray[i] && Rec->iIdString->Des().Length())
				{
					for(TInt o=0; o < iFHandlerArray[i]->iSupArray.Count(); o++)
					{
						if(iFHandlerArray[i]->iSupArray[o])
						{
							if(iFHandlerArray[i]->iSupArray[o]->iIdString)
							{
								TmpFileType.CopyLC(iFHandlerArray[i]->iSupArray[o]->iIdString->Des());
								TmpFileType.TrimAll();
								if(TmpFileType.Length())
								{
									if(iFHandlerArray[i]->iSupArray[o]->iPartialSting)
									{
										if(Rec->iIdString->Des().Length() > TmpFileType.Length())
										{
											if(Rec->iIdString->Des().Left(TmpFileType.Length()) == TmpFileType)
											{
												FileType.Copy(TmpFileType);
												Partial.iX = i;
												Partial.iY = o;
												break;
											}
										}
									}
									else if(Rec->iIdString->Des() == TmpFileType)
									{
										FileType.Copy(TmpFileType);
										
										Full.iX = i;
										Full.iY = o;
										break;
									}
								}
							}
						}
					}
				}
			}
		}
		
		TBuf8<255> FileBuffer;
		
		if(Rec)
		{
			if(Rec->iIdString)
			{
				FileBuffer.Copy(Rec->iIdString->Des());
			}
		}
		
		
		delete Rec;
		Rec = NULL;
		
		TBool Added(EFalse);
		
		if(Full.iX >= 0 && Full.iY >= 0 && iFHandlerArray.Count() > Full.iX)
		{
			if(iFHandlerArray[Full.iX])
			{
				if(iFHandlerArray[Full.iX])
				{
					Added = OpenWithHandlerL(*iFHandlerArray[Full.iX],aFileName,FileType);		
				}
			}
		}
		else if(Partial.iX >= 0 && Partial.iY >= 0 && iFHandlerArray.Count() > Partial.iX)
		{
			if(iFHandlerArray[Partial.iX])
			{
				if(iFHandlerArray[Partial.iX])
				{
					Added = OpenWithHandlerL(*iFHandlerArray[Partial.iX],aFileName,FileType);		
				}
			}
		}
	
		if(!Added && iDocumentHandler)
		{
			RApaLsSession ls;
			User::LeaveIfError(ls.Connect());
			CleanupClosePushL(ls);

			TDataType empty(FileBuffer);
	
			TRAP(Ret,
				if(iDocumentHandler->CanOpenL(empty))
				{
					TUid HandlerAppUid;
					TPckgBuf<TApaAppCapability> TApaAppCapabilityBuf;
					
					iDocumentHandler->HandlerAppUid(HandlerAppUid);
					
					ls.GetAppCapability(TApaAppCapabilityBuf,HandlerAppUid);

			        iDocumentHandler->SetExitObserver(iParent); 
			        
			        if(TApaAppCapabilityBuf().iEmbeddability == TApaAppCapability::ENotEmbeddable)
			        {
			        	iDocumentHandler->OpenFileL(aFileName,empty);
			        }
			        else
			        {
    			    	iDocumentHandler->OpenFileEmbeddedL(aFileName,empty);
			        }
				}
    			);
		
			CleanupStack::PopAndDestroy(1);//ls,
			
			if(Ret != KErrNone)
			{
				delete iDocumentHandler;
				iDocumentHandler = NULL;
				iDocumentHandler =  CDocumentHandler::NewL();
				iDocumentHandler->SetExitObserver(iParent);
			}	
		}
	}
	else
	{
		Ret = KErrNotFound;
	}
	
	return Ret;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CIconHandler::SetUtils(MYBrowserFileHandlerUtils* aExitHandler)
{
	iExitHandler = aExitHandler;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TBool CIconHandler::OpenExtraFoldersL(const TDesC& aName,TInt32& aUID)
{
	TBool Ret(EFalse);
	
	for(TInt HandlerUid=0; HandlerUid < iFHandlerArray.Count(); HandlerUid++)
	{
		if(iFHandlerArray[HandlerUid])
		{
			if(iFHandlerArray[HandlerUid]->iName)
			{
				if(iFHandlerArray[HandlerUid]->iName->Des().Length()
				&& iFHandlerArray[HandlerUid]->iUid.iUid == aUID)
				{
					if(aName == iFHandlerArray[HandlerUid]->iName->Des())
					{
						TInt Err(KErrNone);
						TRAP(Err,
						TAny *impl;  // pointer to interface implementation
						RImplInfoPtrArray infoArray;		
						
						REComSession::ListImplementationsL(KUidYHandlerUID, infoArray);
		
						for ( TInt i = 0; i < infoArray.Count(); i++ )
						{
							if(infoArray[i])
							{
								if(iFHandlerArray[HandlerUid]->iUid == infoArray[i]->ImplementationUid() 
								&& infoArray[i]->DisplayName().Length())
								{
									if(infoArray[i]->DisplayName() == aName)	
									{
										impl = REComSession::CreateImplementationL(infoArray[i]->ImplementationUid(), _FOFF(CYBrowserFileHandler,iDestructorIDKey));
										if ( impl )
										{
											CYBrowserFileHandler* NewHandler = ((CYBrowserFileHandler*)impl);				
											NewHandler->SetUtils(iExitHandler);
											NewHandler->ConstructL();
											NewHandler->SetFocusL(ETrue);
											
											iYBrowserFileUtilsHelp.AddFileHandler(NewHandler);
											
											Ret = ETrue;
											break;
										}
										
										impl = NULL;
									}
								}
							}
						}

						infoArray.ResetAndDestroy();
						);
						
						if(Err != KErrNone)
						{
							ShowFileErrorNoteL(iFHandlerArray[HandlerUid]->iName->Des(),Err);
						}
					}
				}
			}
		}
	}
	
	return Ret;
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TBool CIconHandler::MakeNewFileL(void)
{
	TBool Ret(EFalse);
	TInt Err(KErrNone);
	
	TBuf<60> FilTypeID;
	TFileName HandlerName;
	TInt HandlerUid(-1);
	
	CArrayFixFlat<TInt>* IDArray = new(ELeave)CArrayFixFlat<TInt>(iFHandlerArray.Count());
	CleanupStack::PushL(IDArray);
	CDesCArrayFlat* FilesArray = new(ELeave)CDesCArrayFlat(iFHandlerArray.Count());
	CleanupStack::PushL(FilesArray);
	
	for(TInt i=0; i < iFHandlerArray.Count(); i++)
	{
		if(iFHandlerArray[i])
		{
			if(iFHandlerArray[i]->iName)
			{
				if(iFHandlerArray[i]->iName->Des().Length())
				{
					for(TInt ii=0;ii < iFHandlerArray[i]->iSupArray.Count(); ii++)
					{
						if(iFHandlerArray[i]->iSupArray[ii])
						{
							if((iFHandlerArray[i]->iSupArray[ii]->iFlags & EYBFilehandlerSupportsNewFileNoFiles)
							&& iFHandlerArray[i]->iSupArray[ii]->iIdString)
							{
								IDArray->AppendL(i);
								
								HandlerName.CopyLC(iFHandlerArray[i]->iSupArray[ii]->iIdString->Des());
								HandlerName.Num(GetIconIndex(HandlerName));
								HandlerName.Append(_L("\t"));
								HandlerName.Append(iFHandlerArray[i]->iSupArray[ii]->iIdString->Des());
								
								FilesArray->AppendL(HandlerName);
							}
						}
					}
				}
			}
		}
	}
	
	HandlerName.Zero();
	
	TInt Selected(-1);
	
	if(FilesArray->Count())
	{
		CAknSingleGraphicPopupMenuStyleListBox* list = new(ELeave) CAknSingleGraphicPopupMenuStyleListBox;
	    CleanupStack::PushL(list);

		CAknPopupList* popupList = CAknPopupList::NewL(list,R_AVKON_SOFTKEYS_SELECT_CANCEL);
	    CleanupStack::PushL(popupList);
		
	    list->ConstructL(popupList, CEikListBox::ELeftDownInViewRect);
		list->CreateScrollBarFrameL(ETrue);
	    list->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,CEikScrollBarFrame::EAuto);
	    
	    list->Model()->SetItemTextArray(FilesArray);
		list->Model()->SetOwnershipType(ELbmDoesNotOwnItemArray);
		list->ItemDrawer()->ColumnData()->SetIconArray(GetIconArrayL());
		
		StringLoader::Load(HandlerName,R_STR_NEWFILE);
		popupList->SetTitleL(HandlerName);
		if(popupList->ExecuteLD())
		{
			Selected = list->CurrentItemIndex();
		}
		
	    CleanupStack::Pop();            // popuplist
	    CleanupStack::PopAndDestroy();  // list
	}
	
	if(Selected >= 0 
	&& Selected < IDArray->Count())
	{
		HandlerUid = IDArray->At(Selected);
		
		if(HandlerUid >= 0 && HandlerUid < iFHandlerArray.Count())
		{
			if(iFHandlerArray[HandlerUid])
			{
				if(iFHandlerArray[HandlerUid]->iName)
				{
				TRAP(Err,
					TAny *impl;  // pointer to interface implementation
					RImplInfoPtrArray infoArray;		
							
					REComSession::ListImplementationsL(KUidYHandlerUID, infoArray);
							
					for ( TInt i = 0; i < infoArray.Count(); i++ )
					{
						if(infoArray[i])
						{
							if(iFHandlerArray[HandlerUid]->iUid == infoArray[i]->ImplementationUid() 
							&& infoArray[i]->DisplayName().Length())
							{
								if(infoArray[i]->DisplayName() == iFHandlerArray[HandlerUid]->iName->Des())	
								{
									impl = REComSession::CreateImplementationL(infoArray[i]->ImplementationUid(), _FOFF(CYBrowserFileHandler,iDestructorIDKey));
									if ( impl )
									{
										CYBrowserFileHandler* NewHandler = ((CYBrowserFileHandler*)impl);				
										NewHandler->SetUtils(iExitHandler);
										NewHandler->ConstructL();
										NewHandler->SetFocusL(ETrue);
										
										TFileName CurrPath;
										iYBrowserFileUtilsHelp.GetCurrentPath(CurrPath);
											
										iYBrowserFileUtilsHelp.AddFileHandler(NewHandler);
										
										NewHandler->NewFileL(CurrPath,FilTypeID);
										
										Ret = ETrue;
										break;
									}
									
									impl = NULL;
								}
							}
						}
					}

					infoArray.ResetAndDestroy();
				);
				
					if(Err != KErrNone)
					{
						ShowFileErrorNoteL(iFHandlerArray[HandlerUid]->iName->Des(),Err);
					}
				}
			}
		}
	}

	CleanupStack::PopAndDestroy(2);//UIDArray,FilesArray

	return Ret;
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TBool CIconHandler::PasteFilesToHandlerL(MDesCArray& aFileArray)
{
	TBool Ret(EFalse);
	TInt Err(KErrNone);
	
	TBuf<60> FilTypeID;
	TFileName HandlerName;
	TInt HandlerUid(-1);
	
	if(iFHandlerArray.Count() > 0)
	{
		CArrayFixFlat<TInt>* IDArray = new(ELeave)CArrayFixFlat<TInt>(iFHandlerArray.Count());
		CleanupStack::PushL(IDArray);
		CDesCArrayFlat* FilesArray = new(ELeave)CDesCArrayFlat(iFHandlerArray.Count());
		CleanupStack::PushL(FilesArray);
		
		for(TInt i=0; i < iFHandlerArray.Count(); i++)
		{
			if(iFHandlerArray[i])
			{
				if(iFHandlerArray[i]->iName)
				{
					if(iFHandlerArray[i]->iName->Des().Length())
					{
						for(TInt ii=0;ii < iFHandlerArray[i]->iSupArray.Count(); ii++)
						{
							if(iFHandlerArray[i]->iSupArray[ii])
							{
								if((iFHandlerArray[i]->iSupArray[ii]->iFlags & EYBFilehandlerSupportsNewFileMultiple)
								&& iFHandlerArray[i]->iSupArray[ii]->iIdString)
								{
									IDArray->AppendL(i);
									
									HandlerName.CopyLC(iFHandlerArray[i]->iSupArray[ii]->iIdString->Des());
									HandlerName.Num(GetIconIndex(HandlerName));
									HandlerName.Append(_L("\t"));
									HandlerName.Append(iFHandlerArray[i]->iSupArray[ii]->iIdString->Des());
									
									FilesArray->AppendL(HandlerName);
								}
							}
						}
					}
				}
			}
		}
		
		HandlerName.Zero();
		
		TInt Selected(-1);
		
		if(FilesArray->Count())
		{
			CAknSingleGraphicPopupMenuStyleListBox* list = new(ELeave) CAknSingleGraphicPopupMenuStyleListBox;
		    CleanupStack::PushL(list);

			CAknPopupList* popupList = CAknPopupList::NewL(list,R_AVKON_SOFTKEYS_SELECT_CANCEL);
		    CleanupStack::PushL(popupList);
			
		    list->ConstructL(popupList, CEikListBox::ELeftDownInViewRect);
			list->CreateScrollBarFrameL(ETrue);
		    list->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,CEikScrollBarFrame::EAuto);
		    
		    list->Model()->SetItemTextArray(FilesArray);
			list->Model()->SetOwnershipType(ELbmDoesNotOwnItemArray);
			list->ItemDrawer()->ColumnData()->SetIconArray(GetIconArrayL());
			
			StringLoader::Load(HandlerName,R_STR_NEWFILE);
			popupList->SetTitleL(HandlerName);
			if(popupList->ExecuteLD())
			{
				Selected = list->CurrentItemIndex();
			}
			
		    CleanupStack::Pop();            // popuplist
		    CleanupStack::PopAndDestroy();  // list
		}
		
		if(Selected >= 0 
		&& Selected < IDArray->Count())
		{
			HandlerUid = IDArray->At(Selected);
		}
		
		CleanupStack::PopAndDestroy(2);//UIDArray,FilesArray
	}
	
	if(HandlerUid >= 0 && HandlerUid < iFHandlerArray.Count())
	{
		if(iFHandlerArray[HandlerUid])
		{
			if(iFHandlerArray[HandlerUid]->iName)
			{
			TRAP(Err,
				TAny *impl;  // pointer to interface implementation
				RImplInfoPtrArray infoArray;		
						
				REComSession::ListImplementationsL(KUidYHandlerUID, infoArray);
						
				for ( TInt i = 0; i < infoArray.Count(); i++ )
				{
					if(infoArray[i])
					{
						if(iFHandlerArray[HandlerUid]->iUid == infoArray[i]->ImplementationUid() 
						&& infoArray[i]->DisplayName().Length())
						{
							if(infoArray[i]->DisplayName() == iFHandlerArray[HandlerUid]->iName->Des())	
							{
								impl = REComSession::CreateImplementationL(infoArray[i]->ImplementationUid(), _FOFF(CYBrowserFileHandler,iDestructorIDKey));
								if ( impl )
								{
									CYBrowserFileHandler* NewHandler = ((CYBrowserFileHandler*)impl);				
									NewHandler->SetUtils(iExitHandler);
									NewHandler->ConstructL();
									NewHandler->SetFocusL(ETrue);
									
									TFileName CurrPath;
									iYBrowserFileUtilsHelp.GetCurrentPath(CurrPath);
										
									iYBrowserFileUtilsHelp.AddFileHandler(NewHandler);
									
									NewHandler->NewFileL(CurrPath,FilTypeID,aFileArray);
									
									Ret = ETrue;
									break;
								}
								
								impl = NULL;
							}
						}
					}
				}

				infoArray.ResetAndDestroy();
			);
			
				if(Err != KErrNone)
				{
					ShowFileErrorNoteL(iFHandlerArray[HandlerUid]->iName->Des(),Err);
				}
			}
		}
	}
	
	return Ret;
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CIconHandler::SendFilesL(MDesCArray& aFileArray,TBool& aSendWithSendUI)
{	
	aSendWithSendUI = EFalse;
	
	TFileName ShowName;
	
	RPointerArray<CFHandlerItem> OpenerArray;
	GetSenderReceiverL(OpenerArray,ETrue);
	
	// list ask here...
	CAknSinglePopupMenuStyleListBox* list = new(ELeave) CAknSinglePopupMenuStyleListBox;
    CleanupStack::PushL(list);

	CAknPopupList* popupList = CAknPopupList::NewL(list,R_AVKON_SOFTKEYS_OK_CANCEL);
    CleanupStack::PushL(popupList);
	
    list->ConstructL(popupList, CEikListBox::ELeftDownInViewRect);
	list->CreateScrollBarFrameL(ETrue);
    list->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,CEikScrollBarFrame::EAuto);
    
    CDesCArrayFlat* Ittems = new(ELeave)CDesCArrayFlat(5);
    CleanupStack::PushL(Ittems);
    
    StringLoader::Load(ShowName,R_STR_SENDUI);
    Ittems->AppendL(ShowName);
	
	for(TInt i=0; i < OpenerArray.Count(); i++)
	{
		if(OpenerArray[i])
		{
			if(OpenerArray[i]->iName)
			{
				Ittems->AppendL(*OpenerArray[i]->iName);		
			}
		}
	}	
	
    CleanupStack::Pop(Ittems);
    list->Model()->SetItemTextArray(Ittems);
	list->Model()->SetOwnershipType(ELbmOwnsItemArray);

	StringLoader::Load(ShowName,R_SH_STR_SENDFFVIA);
	popupList->SetTitleL(ShowName);
	
	if (popupList->ExecuteLD())
    {
		TInt Ret = list->CurrentItemIndex();
		if(Ret > 0)
		{
			Ret = Ret - 1;
			if(Ret < OpenerArray.Count())
			{
				TInt Err(KErrNone);
				
				
				TRAP(Err,
				TAny *impl;  // pointer to interface implementation
				RImplInfoPtrArray infoArray;		
						
				REComSession::ListImplementationsL(KUidYHandlerUID, infoArray);
								
				for ( TInt i = 0; i < infoArray.Count(); i++ )
				{
					if(infoArray[i] && OpenerArray[Ret])
					{
						if(OpenerArray[Ret]->iUid == infoArray[i]->ImplementationUid() 
						&& infoArray[i]->DisplayName().Length()
						&& OpenerArray[Ret]->iName)
						{
							if(infoArray[i]->DisplayName() == OpenerArray[Ret]->iName->Des())	
							{
								ShowName.Copy(infoArray[i]->DisplayName());
								
								impl = REComSession::CreateImplementationL(infoArray[i]->ImplementationUid(), _FOFF(CYBrowserFileHandler,iDestructorIDKey));
								if ( impl )
								{
									CYBrowserFileSender* NewHandler = ((CYBrowserFileSender*)impl);				
									NewHandler->SetUtils(iExitHandler);
									NewHandler->ConstructL();
									NewHandler->SetFocusL(ETrue);
										
									iYBrowserFileUtilsHelp.AddFileHandler(NewHandler);
									NewHandler->SendFilesL(aFileArray);
									
									break;
								}
								
								impl = NULL;
							}
						}
					}
				}

				infoArray.ResetAndDestroy();
				);
				
				if(Err != KErrNone)
				{
					ShowFileErrorNoteL(ShowName,Err);
				}
			}
		}
		else
		{
			aSendWithSendUI = ETrue;
		}
	}				
	
    CleanupStack::Pop();            // popuplist
    CleanupStack::PopAndDestroy();  // list
	OpenerArray.Reset();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TBool CIconHandler::OpenWithHandlerL(CFHandlerItem& aHandler,RFile& aFile,const TDesC& aTypeID)
{
	TBool Ret(EFalse);
	TInt Err(KErrNone);
	
	TRAP(Err,
		TAny *impl;  // pointer to interface implementation
		RImplInfoPtrArray infoArray;		
				
		REComSession::ListImplementationsL(KUidYHandlerUID, infoArray);
						
		for ( TInt i = 0; i < infoArray.Count(); i++ )
		{
			if(infoArray[i])
			{
				if(aHandler.iUid == infoArray[i]->ImplementationUid() 
				&& infoArray[i]->DisplayName().Length()
				&& aHandler.iName)
				{
					if(infoArray[i]->DisplayName() == aHandler.iName->Des())	
					{
						impl = REComSession::CreateImplementationL(infoArray[i]->ImplementationUid(), _FOFF(CYBrowserFileHandler,iDestructorIDKey));
						if ( impl )
						{
							CYBrowserFileHandler* NewHandler = ((CYBrowserFileHandler*)impl);				
							NewHandler->SetUtils(iExitHandler);
							NewHandler->ConstructL();
							NewHandler->SetFocusL(ETrue);
								
							iYBrowserFileUtilsHelp.AddFileHandler(NewHandler);
							NewHandler->OpenFileL(aFile,aTypeID);
							Ret = ETrue;
							break;
						}
						
						impl = NULL;
					}
				}
			}
		}

		infoArray.ResetAndDestroy();
		);
	
	if(Err != KErrNone)
	{
		TFileName FilName;
		aFile.Name(FilName);
	
		ShowFileErrorNoteL(FilName,Err);
	}

	return Ret;
}		
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TBool CIconHandler::OpenWithHandlerL(CFHandlerItem& aHandler,const TDesC& aFileName,const TDesC& aTypeID)
{
	TBool Ret(EFalse);
	TInt Err(KErrNone);
	
	TRAP(Err,
		TAny *impl;  // pointer to interface implementation
		RImplInfoPtrArray infoArray;		
				
		REComSession::ListImplementationsL(KUidYHandlerUID, infoArray);
						
		for ( TInt i = 0; i < infoArray.Count(); i++ )
		{
			if(infoArray[i])
			{
				if(aHandler.iUid == infoArray[i]->ImplementationUid() 
				&& infoArray[i]->DisplayName().Length()
				&& aHandler.iName)
				{
					if(infoArray[i]->DisplayName() == aHandler.iName->Des())	
					{
						impl = REComSession::CreateImplementationL(infoArray[i]->ImplementationUid(), _FOFF(CYBrowserFileHandler,iDestructorIDKey));
						if ( impl )
						{
							CYBrowserFileHandler* NewHandler = ((CYBrowserFileHandler*)impl);				
							NewHandler->SetUtils(iExitHandler);
							NewHandler->ConstructL();
							NewHandler->SetFocusL(ETrue);
								
							iYBrowserFileUtilsHelp.AddFileHandler(NewHandler);
							NewHandler->OpenFileL(aFileName,aTypeID);
							Ret = ETrue;
							break;
						}
						
						impl = NULL;
					}
				}
			}
		}

		infoArray.ResetAndDestroy();
		);
	
	if(Err != KErrNone)
	{
		ShowFileErrorNoteL(aFileName,Err);
	}

	return Ret;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CIconHandler::RecognizeData(CYBRecognitionResult& aResult,const TDesC8& aFileData,const TDesC& aFileName)
{
	if(aResult.iIdString)
	{
		aResult.iIdString->Des().Zero();
	}
	aResult.iConfidence = CYBRecognitionResult::ERecNotRecognized;
	
	TRAPD(err,
	if(iExtraFileRecognizer.Count())
	{		
		for(TInt rz=0; rz < iExtraFileRecognizer.Count(); rz++)
		{
			if(iExtraFileRecognizer[rz])
			{
				if(iExtraFileRecognizer[rz]->FileTypeCount())
				{
					if(aFileData.Length() >= iExtraFileRecognizer[rz]->MinimumFileSize())
					{
						CYBRecognitionResult* Rec = new(ELeave)CYBRecognitionResult();
						
						iExtraFileRecognizer[rz]->RecognizeFile(*Rec,aFileName,aFileData);
						if(Rec->iConfidence > aResult.iConfidence
						&& Rec->iIdString)
						{
							if(Rec->iIdString->Des().Length())
							{
								aResult.iConfidence = Rec->iConfidence;
								
								delete aResult.iIdString;
								aResult.iIdString = NULL;
								
								aResult.iIdString = Rec->iIdString;
								Rec->iIdString = NULL;
							}
						}
						
						delete Rec;
						Rec = NULL;
					}
				}
			}
		}
	});
	
	if(aResult.iConfidence <= CYBRecognitionResult::ERecPossible)
	{
		RApaLsSession RSession;
		if(RSession.Connect() == KErrNone)
		{	
			TDataRecognitionResult FileDataType;
			FileDataType.Reset();

			RSession.RecognizeData(aFileName,aFileData,*&FileDataType);
			
			if(FileDataType.iConfidence > aResult.iConfidence)
			{
				aResult.iConfidence = FileDataType.iConfidence;
				
				delete aResult.iIdString;
				aResult.iIdString = NULL;
				aResult.iIdString = FileDataType.iDataType.Des().AllocL();
			}
			
			RSession.Close();
		}
	}
	
	if(aResult.iIdString)
	{
		aResult.iIdString->Des().TrimAll();
		aResult.iIdString->Des().CopyLC(aResult.iIdString->Des());
	}	
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CIconHandler::RecognizeFile(CYBRecognitionResult& aResult,const TDesC& aFileName)
{
	TEntry FileEntry;

	if(CCoeEnv::Static()->FsSession().Entry(aFileName,FileEntry) == KErrNone)
	{
		TBuf8<255> FileBuffer;
		
		if(!FileEntry.IsDir())
		{
			TInt FileSize = FileEntry.iSize;

			if(FileSize > 255)
			{
				FileSize = 255;
			}
			
			if(CCoeEnv::Static()->FsSession().ReadFileSection(aFileName,0,FileBuffer,FileSize) == KErrNone)
			{
				RecognizeData(aResult,FileBuffer,aFileName);
			}
		}
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/	
void CIconHandler::ConstructL(void)
{
	iGeneralFile = new(ELeave)CFFileTypeItem();
	iGeneralFile ->iIdString 		= NULL;
	iGeneralFile ->iTypeId			= CFFileItem::EFJustFile;

	iDocumentHandler =  CDocumentHandler::NewL();
	iDocumentHandler->SetExitObserver(iParent);

	// add all recognizers here
	GetRecognizers();
	
	// Map all supported files to the icons
	GetSupportedFilesArrayL();
	
	// get all file handlers
	GetFileHandlers();

}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CIconHandler::GetRecognizers()
{		
	RImplInfoPtrArray infoArray2;
	REComSession::ListImplementationsL(KUidYRecogzrUID, infoArray2);

	CheckRecognizersL(infoArray2);
	
	infoArray2.ResetAndDestroy();	
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CIconHandler::CheckRecognizersL(RImplInfoPtrArray& aArray)
{
	TAny *impl;  // pointer to interface implementation
	TUid implementationUid;
	
	TFileName CurrName;
	TInt Errnum(KErrNone);

	for ( TInt i = 0; i < aArray.Count(); i++ )
	{
		Errnum = KErrNone;
		CurrName.Zero();
		
		if(aArray[i])
		{
		TRAP(Errnum,
			CurrName.Copy(aArray[i]->DisplayName());
			CurrName.TrimAll();
				
			if(CurrName.Length() && CheckVersionL(aArray[i]->OpaqueData()))
			{
				implementationUid = aArray[i]->ImplementationUid();
				
				impl = REComSession::CreateImplementationL(implementationUid, _FOFF(CYBrowserFileRecognizer,iDestructorIDKey));
				if ( impl )
				{
					iExtraFileRecognizer.Append((CYBrowserFileRecognizer*)impl);				
				}
			}
			
			impl = NULL;
			implementationUid.iUid = 0;
			);
		}
		
		if(Errnum != KErrNone)
		{
			ShowFileErrorNoteL(CurrName,Errnum);
		}
	}
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CIconHandler::GetSenderReceiverL(RPointerArray<CFHandlerItem>& aArray,TBool aSender)
{
	for(TInt i=0; i < iFHandlerArray.Count(); i++)
	{
		if(iFHandlerArray[i])
		{
			if(aSender)
			{
				if(iFHandlerArray[i]->iFlags & EYBFilehandlerSupportsSendFiles)
				{
					aArray.Append(iFHandlerArray[i]);
				}
			}
			else
			{
				if(iFHandlerArray[i]->iFlags & EYBFilehandlerSupportsRecvFiles)
				{
					aArray.Append(iFHandlerArray[i]);
				}
			}
		}
	}
}

/*
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
*/

void CIconHandler::GetFileHandlers()
{
	iFHandlerArray.ResetAndDestroy();
	
	RImplInfoPtrArray infoArray;		
	REComSession::ListImplementationsL(KUidYHandlerUID, infoArray);
		
	TFileName CurrName;
	TInt ErrNum(KErrNone);
	
	for ( TInt i = 0; i < infoArray.Count(); i++ )
	{
		ErrNum = KErrNone;
		CurrName.Zero();
		
		if(infoArray[i])
		{
			TRAP(ErrNum,
			CFHandlerItem* NewItem = new(ELeave)CFHandlerItem();
			CleanupStack::PushL(NewItem);
			
			CurrName.Copy(infoArray[i]->DisplayName());
			CurrName.TrimAll();
			
			TInt ItemStart 	= infoArray[i]->OpaqueData().Find(KtxStrtYTools);
			TInt ItemEnd 	= infoArray[i]->OpaqueData().Find(KtxEnddYTools);
			
			if(CurrName.Length()
			&& ItemStart != KErrNotFound
			&& ItemEnd != KErrNotFound)
			{
				ItemStart = ItemStart + KtxStrtYTools().Length();
				ItemEnd = (ItemEnd - ItemStart);
				
				if(ItemEnd > 0)
				{
					if(CheckSupportedTypesL(infoArray[i]->OpaqueData().Mid(ItemStart,ItemEnd),*NewItem))
					{
						NewItem->iUid = infoArray[i]->ImplementationUid();
						NewItem->iName = CurrName.Alloc();
						
						if(!iHasOpenWith)
						{
							if(NewItem->iFlags & EYBFilehandlerSupportsOpenWith)
							{
								iHasOpenWith = ETrue;
							}
						}
						
						if(!iHasPasteMany)
						{
							for(TInt s=0; s < NewItem->iSupArray.Count(); s++)
							{
								if(NewItem->iSupArray[s])
								{
									if(NewItem->iSupArray[s]->iFlags & EYBFilehandlerSupportsNewFileMultiple)
									{
										iHasPasteMany = ETrue;
									}
								}
							}
						}
						
						if(!iHasNewFile)
						{
							for(TInt s=0; s < NewItem->iSupArray.Count(); s++)
							{
								if(NewItem->iSupArray[s])
								{
									if(NewItem->iSupArray[s]->iFlags & EYBFilehandlerSupportsNewFileNoFiles)
									{
										iHasNewFile = ETrue;
									}
								}
							}
						}
						
						if(!iHasSenders)
						{
							if(NewItem->iFlags & EYBFilehandlerSupportsSendFiles)
							{
								iHasSenders = ETrue;
							}
						}
						
						if(!iHasReceivers)
						{
							if(NewItem->iFlags & EYBFilehandlerSupportsRecvFiles)
							{
								iHasReceivers = ETrue;
							}
						}
						
						CleanupStack::Pop(NewItem);
						iFHandlerArray.Append(NewItem);
						NewItem = NULL;
					}
				}
			}
			
			if(NewItem)
			{
				CleanupStack::PopAndDestroy(NewItem);
			}	
			);
		}
		if(ErrNum != KErrNone)
		{
			ShowFileErrorNoteL(CurrName,ErrNum);
		}
	}
	

	infoArray.ResetAndDestroy();
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
TBool CIconHandler::CheckVersionL(const TDesC8& aData)
{
	TBool Ret(EFalse);
	
	TInt VerStart 	= aData.Find(KtxStrtVERSION);
	TInt VerEnd 	= aData.Find(KtxEnddVERSION);
		
	if(VerStart != KErrNotFound
	&& VerEnd != KErrNotFound)
	{	
		VerStart = VerStart + KtxStrtVERSION().Length();
		VerEnd = (VerEnd - VerStart);
		
		if(VerEnd > 0 && (aData.Length() > (VerStart + VerEnd)))
		{
			if(aData.Mid(VerStart,VerEnd) == KtxCurrVERSION)
			{
				Ret = ETrue;
			}
		}
	}
	
	return Ret;
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
TBool CIconHandler::CheckSupportedTypesL(const TDesC8& aData,CFHandlerItem& aNewItem)
{	
	TBool Ret(EFalse);
	
	if(CheckVersionL(aData))
	{
		TInt ImplStart 	= aData.Find(KtxStrtImpementation);
		TInt ImplEnd 	= aData.Find(KtxEnddImpementation);

		if(ImplStart != KErrNotFound
		&& ImplEnd != KErrNotFound)
		{	
			ImplStart = ImplStart + KtxStrtImpementation().Length();
			ImplEnd = (ImplEnd - ImplStart);
			
			if(ImplEnd > 0 && (aData.Length() > (ImplStart + ImplEnd)))
			{	
				if(CheckImplTypeL(aData.Mid(ImplStart,ImplEnd),aNewItem))
				{
					Ret = ETrue;
					ReadSupportTypesL(aData.Mid(ImplStart,ImplEnd),aNewItem.iSupArray);		
				}
			}
		}
	}

	return Ret;
}
/*
-----------------------------------------------------------------------------------		
------------------------------------------------------------------------------------
*/
TBool CIconHandler::CheckImplTypeL(const TDesC8& aData,CFHandlerItem& aSupItem)
{
	TBool Ret(EFalse);

	TInt TypStart 	= aData.Find(KtxStrtTYPE);
	TInt TypEnd 	= aData.Find(KtxEnddTYPE);

	if(TypStart != KErrNotFound
	&& TypEnd != KErrNotFound)
	{	
		TypStart = TypStart + KtxStrtTYPE().Length();
		TypEnd = (TypEnd - TypStart);
		
		if(TypEnd > 0 && (aData.Length() > (TypStart + TypEnd)))
		{		
			if(aData.Mid(TypStart,TypEnd) == KtxImpFILEHANDLER)
			{
				Ret = ETrue;
			}
			else if(aData.Mid(TypStart,TypEnd) == KtxImpFILESENDER)
			{
				aSupItem.iFlags = EYBFilehandlerSupportsSendFiles;
				Ret = ETrue;	
			}
			else if(aData.Mid(TypStart,TypEnd) == KtxImpFILERECEIVER)
			{
				aSupItem.iFlags = EYBFilehandlerSupportsRecvFiles;
				Ret = ETrue;
			}
			else if(aData.Mid(TypStart,TypEnd) == KtxImpFILEFOLDER)
			{
				aSupItem.iFlags = EYBFilehandlerAddtoFoldersList;
				Ret = ETrue;
			}
			else if(aData.Mid(TypStart,TypEnd) == KtxImpOPENWITH)
			{
				aSupItem.iFlags = EYBFilehandlerSupportsOpenWith;
				Ret = ETrue;
			}
		}
	}
	
	return Ret;
}

/*
-----------------------------------------------------------------------------------		
------------------------------------------------------------------------------------
*/
void CIconHandler::ReadSupportTypesL(const TDesC8& aSupportData,RPointerArray<CSupportTypeItem>& aArray)
{	
	TInt ItemStart(KErrNotFound);
	TInt ItemEnd(KErrNotFound);
	TInt ReadAlready(0);
	
	TBuf<200> FilType;
	do
	{
		if(ReadAlready < aSupportData.Length())
		{
			ItemStart = aSupportData.Right((aSupportData.Length() - ReadAlready)).Find(KtxStrtSUPPORT);			
			ItemEnd = aSupportData.Right((aSupportData.Length() - ReadAlready)).Find(KtxEnddSUPPORT);
		}
		else
		{
			ItemStart	= KErrNotFound;
			ItemEnd 	= KErrNotFound;
		}
		
		if(ItemStart != KErrNotFound && ItemEnd != KErrNotFound)
		{
			ItemStart = ItemStart + KtxStrtSUPPORT().Length();
			
			if(ItemStart < ItemEnd && (aSupportData.Length() > (ReadAlready + ItemEnd)))
			{
				TPtrC8 CurrItem(aSupportData.Mid((ReadAlready + ItemStart),(ItemEnd - ItemStart)));
				
				TInt FTypStart 	= CurrItem.Find(KtxStrtFILETYPE);
				TInt FTypEnd 	= CurrItem.Find(KtxEnddFILETYPE);
		
				if(FTypStart != KErrNotFound
				&& FTypEnd != KErrNotFound)
				{	
					FTypStart = FTypStart + KtxStrtFILETYPE().Length();
					FTypEnd = (FTypEnd - FTypStart);
					
					if(FTypEnd > 0 && (CurrItem.Length() > (FTypStart + FTypEnd)))
					{
						CSupportTypeItem* SpIttem = new(ELeave)CSupportTypeItem();
						CleanupStack::PushL(SpIttem);
					
						FilType.Copy(CurrItem.Mid(FTypStart,FTypEnd));
						FilType.CopyLC(FilType);
						
						SpIttem->iIdString = FilType.AllocL();
						
						if(CurrItem.Find(KtxStrtFTPartial) != KErrNotFound
						&& CurrItem.Find(KtxEnddFTPartial) != KErrNotFound)
						{
							SpIttem->iPartialSting = ETrue;		
						}
						
						if(CurrItem.Find(KtxStrtRFILE) != KErrNotFound
						&& CurrItem.Find(KtxEnddRFILE) != KErrNotFound)
						{
							SpIttem->iFlags = (SpIttem->iFlags + EYBFilehandlerSupportsRFile);		
						}
						
						if(CurrItem.Find(KtxStrtNEWEMPY) != KErrNotFound
						&& CurrItem.Find(KtxEnddNEWEMPY) != KErrNotFound)
						{
							SpIttem->iFlags = (SpIttem->iFlags + EYBFilehandlerSupportsNewFileNoFiles);		
						}
						
						if(CurrItem.Find(KtxStrtNEWWITHFILES) != KErrNotFound
						&& CurrItem.Find(KtxEnddNEWWITHFILES) != KErrNotFound)
						{
							SpIttem->iFlags = (SpIttem->iFlags + EYBFilehandlerSupportsNewFileMultiple);		
						}
						
						CleanupStack::Pop();//SpIttem
						aArray.Append(SpIttem);
					}
				}
			}

			ReadAlready = ReadAlready + ItemEnd + KtxEnddSUPPORT().Length();
		}
		
	}while(ItemStart != KErrNotFound && ItemEnd != KErrNotFound);
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CIconHandler::AddExtraFoldersL(RPointerArray<CFFileItem>& aItemArray)
{
	for(TInt i=0; i < iFHandlerArray.Count(); i++)
	{
		if(iFHandlerArray[i])
		{
			if(iFHandlerArray[i]->iName)
			{
				if(iFHandlerArray[i]->iFlags & EYBFilehandlerAddtoFoldersList)
				{
					CFFileItem* NewItem = new(ELeave)CFFileItem();
					CleanupStack::PushL(NewItem);
					NewItem->iName 		= iFHandlerArray[i]->iName->Des().AllocL();
					NewItem->iType 		= CFFileItem::EFExtraFolder;
					NewItem->iExtra		= iFHandlerArray[i]->iUid.iUid;
					
					CleanupStack::Pop(NewItem);
					aItemArray.Append(NewItem);
				}
			}
		}
	}
}


/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
TInt CIconHandler::AskOpenFileWithL(RPointerArray<CFHandlerItem>& aArray,const TBool& aAppSelect) 
{
	TInt Ret(-1);
	
	TBuf<60> Hjelpper;

	// list ask here...
	CAknSinglePopupMenuStyleListBox* list = new(ELeave) CAknSinglePopupMenuStyleListBox;
    CleanupStack::PushL(list);

	CAknPopupList* popupList = CAknPopupList::NewL(list,R_AVKON_SOFTKEYS_OK_CANCEL);
    CleanupStack::PushL(popupList);
	
    list->ConstructL(popupList, CEikListBox::ELeftDownInViewRect);
	list->CreateScrollBarFrameL(ETrue);
    list->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,CEikScrollBarFrame::EAuto);
    
    CDesCArrayFlat* Ittems = new(ELeave)CDesCArrayFlat(5);
    CleanupStack::PushL(Ittems);
    
    if(aAppSelect)
    {	
    	StringLoader::Load(Hjelpper,R_SH_STR_SELAPPLICATION);
    	Ittems->AppendL(Hjelpper);
    }
    
	for(TInt i=0; i < aArray.Count(); i++)
	{
		if(aArray[i])
		{
			if(aArray[i]->iName)
			{
				Ittems->AppendL(*aArray[i]->iName);		
			}
		}
	}
    CleanupStack::Pop(Ittems);
    list->Model()->SetItemTextArray(Ittems);
	list->Model()->SetOwnershipType(ELbmOwnsItemArray);

	StringLoader::Load(Hjelpper,R_SH_STR_OPENFILE);
	popupList->SetTitleL(Hjelpper);
	
	if (popupList->ExecuteLD())
    {
		Ret = list->CurrentItemIndex();
	}				
	
    CleanupStack::Pop();            // popuplist
    CleanupStack::PopAndDestroy();  // list
    
    return Ret;
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CIconHandler::OpenFileWithL(RFile& aFile) 
{
	TFileName FilName;
	TInt FilSize(0);
	
	aFile.Name(FilName);
	if(aFile.Size(FilSize)  == KErrNone)
	{
		CYBRecognitionResult* Rec = new(ELeave)CYBRecognitionResult();

		TBuf8<255> FileBuffer;
			
		if(FilSize > 255)
		{
			FilSize = 255;
		}
		
		if(aFile.Read(FileBuffer, FilSize) == KErrNone)
		{
			RecognizeData(*Rec,FileBuffer,FilName);
		}
		
		RPointerArray<CFHandlerItem> OpenerArray;

		for(TInt i=0; i < iFHandlerArray.Count(); i++)
		{
			if(iFHandlerArray[i])
			{
				for(TInt s=0; s < iFHandlerArray[i]->iSupArray.Count(); s++)
				{
					if(iFHandlerArray[i]->iSupArray[s])
					{
						if(iFHandlerArray[i]->iFlags & EYBFilehandlerSupportsOpenWith
						&& iFHandlerArray[i]->iSupArray[s]->iFlags & EYBFilehandlerSupportsRFile)
						{
							OpenerArray.Append(iFHandlerArray[i]);
						}
					}
				}
			}
		}
		
		TInt Ret = AskOpenFileWithL(OpenerArray,EFalse);
		if(Ret >= 0
		&& Ret < OpenerArray.Count())
		{
			if(OpenerArray[Ret])
			{
				FilName.Zero();
				
				if(Rec->iIdString)
				{
					if(Rec->iIdString->Des().Length() > 100)
						FilName.Copy(Rec->iIdString->Des().Left(100));
					else
						FilName.Copy(Rec->iIdString->Des());
				}
				
				OpenWithHandlerL(*OpenerArray[Ret],aFile,FilName);	
			}
		}

	    OpenerArray.Reset();
	    
	    delete Rec;
	    Rec = NULL;
	}
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CIconHandler::OpenFileWithL(const TDesC& aFileName) 
{
	RPointerArray<CFHandlerItem> OpenerArray;

	for(TInt i=0; i < iFHandlerArray.Count(); i++)
	{
		if(iFHandlerArray[i])
		{		
			if(iFHandlerArray[i]->iFlags & EYBFilehandlerSupportsOpenWith)
			{
				OpenerArray.Append(iFHandlerArray[i]);
			}
		}
	}
	
	TInt Ret = AskOpenFileWithL(OpenerArray,ETrue);
	if(Ret > 0)
	{
		Ret = Ret - 1;
		if(Ret < OpenerArray.Count() && Ret >= 0)
		{
			if(OpenerArray[Ret])
			{
				TBuf<100> Hjelpper;
				CYBRecognitionResult* Res = new(ELeave)CYBRecognitionResult(); 
				RecognizeFile(*Res,aFileName);
				
				if(Res->iIdString)
				{
					if(Res->iIdString->Des().Length() > 100)
						Hjelpper.Copy(Res->iIdString->Des().Left(100));
					else
						Hjelpper.Copy(Res->iIdString->Des());
				}
				
				OpenWithHandlerL(*OpenerArray[Ret],aFileName,Hjelpper);	
				
				delete Res;
				Res = NULL;
			}
		}
	}
	else if(Ret == 0)
	{
		OpenWithApplicationL(aFileName);
	}

    OpenerArray.Reset();
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CIconHandler::OpenWithApplicationL(const TDesC& aFileName) 
{
	RApaLsSession ls;
	User::LeaveIfError(ls.Connect());
	CleanupClosePushL(ls);
	
	CDesCArrayFlat* AppNames = new(ELeave)CDesCArrayFlat(5);
    CleanupStack::PushL(AppNames);

    CDesCArrayFlat* Ittems = new(ELeave)CDesCArrayFlat(5);
    CleanupStack::PushL(Ittems);
	
	CArrayPtr<CGulIcon>* icons =new( ELeave ) CAknIconArray(10);
	CleanupStack::PushL(icons);
		
	User::LeaveIfError(ls.GetAllApps());

	TInt Errnono(KErrNone);
	TFileName HelpFn;
	TApaAppInfo AppInfo;
	
	MAknsSkinInstance* skin = AknsUtils::SkinInstance();
	
	do
	{
		Errnono = ls.GetNextApp(AppInfo);
		if(KErrNone == Errnono)
		{
			TRAPD(Err,
			CFbsBitmap*	AppIcon(NULL);
			CFbsBitmap*	AppIconMsk(NULL);
		    AknsUtils::CreateAppIconLC(skin,AppInfo.iUid,  EAknsAppIconTypeContext,AppIcon,AppIconMsk);
		
			if(AppIcon && AppIconMsk)
			{
				icons->AppendL(CGulIcon::NewL(AppIcon,AppIconMsk));
				CleanupStack::Pop(2);
						
				HelpFn.Num((icons->Count() - 1));
				HelpFn.Append(_L("\t"));
				HelpFn.Append(AppInfo.iCaption);
				HelpFn.TrimAll();
				
				Ittems->AppendL(HelpFn);	
				AppNames->AppendL(AppInfo.iFullName);					
			});
		}
		
	}while(KErrNone == Errnono);


	TInt OpenedItem(-1);

	CleanupStack::Pop(icons);

	CAknSelectionListDialog* dialog = CAknSelectionListDialog::NewL(OpenedItem,Ittems,0); 

	dialog->PrepareLC(R_SEL_APP_DIALOG);
	
	dialog->SetIconArrayL(icons); 
	if (dialog->RunLD ())
	{
		if(OpenedItem >= 0 && OpenedItem < AppNames->Count())
		{
			CApaCommandLine* cmdLine = CApaCommandLine::NewLC();
			
			TParsePtrC Hjelpper(AppNames->MdcaPoint(OpenedItem));
			cmdLine->SetExecutableNameL(Hjelpper.NameAndExt());
	
			cmdLine->SetDocumentNameL(aFileName);
			cmdLine->SetCommandL(EApaCommandOpen);

			User::LeaveIfError(ls.StartApp(*cmdLine) );
			CleanupStack::PopAndDestroy(cmdLine);
		}
	}				
	
   
	CleanupStack::PopAndDestroy(3); // ls,AppNames,Ittems
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
CArrayPtr<CGulIcon>* CIconHandler::GetSelectionIconArrayL(void)
{
    TSize IconSiz(0,0);	
	return GetSelectionIconArrayL(IconSiz);
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
CArrayPtr<CGulIcon>* CIconHandler::GetSelectionIconArrayL(TSize aSize)
    {
	CArrayPtr<CGulIcon>* icons =new( ELeave ) CAknIconArray(1);
	CleanupStack::PushL(icons);	

	if(iConFile.Length())
	{
		_LIT(KtxAvkonMBM	,"Z:\\resource\\apps\\avkon2.mif");
		
		CFbsBitmap* FrameMsk2(NULL);
		CFbsBitmap* FrameImg2(NULL);
		CFbsBitmap* FrameMsk(NULL);
		CFbsBitmap* FrameImg(NULL);
		
		AknIconUtils::CreateIconL(FrameImg2,FrameMsk2,KtxAvkonMBM,EMbmAvkonQgn_indi_checkbox_on,EMbmAvkonQgn_indi_checkbox_on_mask);
		AknIconUtils::CreateIconL(FrameImg,FrameMsk,KtxAvkonMBM,EMbmAvkonQgn_indi_checkbox_off,EMbmAvkonQgn_indi_checkbox_off_mask);

		if(FrameImg)
		{
			if(aSize.iHeight > 0 && aSize.iWidth > 0)
			{
				AknIconUtils::SetSize(FrameImg,aSize,EAspectRatioPreserved);  
			}
						
			if(FrameMsk && aSize.iHeight > 0 && aSize.iWidth > 0)
			{
				AknIconUtils::SetSize(FrameMsk,aSize,EAspectRatioPreserved);	
			}
		}
		
		if(FrameImg2)
		{
			if(aSize.iHeight > 0 && aSize.iWidth > 0)
			{
				AknIconUtils::SetSize(FrameImg2,aSize,EAspectRatioPreserved);  
			}
						
			if(FrameMsk2 && aSize.iHeight > 0 && aSize.iWidth > 0)
			{
				AknIconUtils::SetSize(FrameMsk2,aSize,EAspectRatioPreserved);	
			}
		}
	
		icons->AppendL(CGulIcon::NewL(FrameImg2,FrameMsk2));
		icons->AppendL(CGulIcon::NewL(FrameImg,FrameMsk));

	}
		
	CleanupStack::Pop(); //icons
	return icons;
    }

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
TInt CIconHandler::GetIconIndex(const TDesC& aIdString)
{
	TInt Ret(CFFileItem::EFJustFile);
				
	if(aIdString.Length())
	{	
		TBuf<100> TmpBug;
		if(aIdString.Length() > 100)
			TmpBug.Copy(aIdString.Left(99));
		else
			TmpBug.Copy(aIdString);
		
		for(TInt z=0; z < iItemTypeArray.Count(); z++)
		{
			if(iItemTypeArray[z])
			{
				if(iItemTypeArray[z]->iIdString)
				{
					if(iItemTypeArray[z]->iIdString->Des() == TmpBug)
					{
						Ret = iItemTypeArray[z]->iTypeId;
						break;													
					}
				}
			}
		}
	}
	
	return Ret;
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
CGulIcon* CIconHandler::GetIconL(const TDesC& aIdString)
{
	TSize IconSiz(0,0);
		
	return GetIconL(aIdString,IconSiz);
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
CGulIcon* CIconHandler::GetIconL(const TDesC& aIdString,TSize aSize)
{
	CGulIcon* RetIcon(NULL);
	
	RetIcon = GetIconL(GetIconIndex(aIdString),aSize);
	
	return RetIcon;
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
CGulIcon* CIconHandler::GetIconL(TInt aIndex)
{
	TSize IconSiz(0,0);

	return GetIconL(aIndex,IconSiz);
}
 
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
CGulIcon* CIconHandler::GetIconL(TInt aIndex,TSize aSize)
{
	CGulIcon* RetIcon(NULL);
	
	TRAPD(Errr,
	if(iConFile.Length())
	{
		switch(aIndex)
		{
		case 0:
			RetIcon = AddIconFromFileL(aSize,iConFile,0,1); // sel	
			break;
		case 1:
			RetIcon = AddIconFromFileL(aSize,iConFile,2,3); // extra folder
			break;
		case 2:
			RetIcon = AddIconFromFileL(aSize,iConFile,4,5);	// drive
			break;
		case 3:
			RetIcon = AddIconFromFileL(aSize,iConFile,6,7);	// folder
			break;
		case 4:
			RetIcon = AddIconFromFileL(aSize,iConFile,8,9);	// file
			break;
		default:
			{
				TInt Sel = (aIndex - (CFFileItem::EFJustFile + 1));
	
				if(Sel < iItemIconArray.Count() && Sel >= 0)
				{
					if(iItemIconArray[Sel])
					{
						if(iItemIconArray[Sel]->iSystemApp)
						{
							RetIcon = GetAppIconL(iItemIconArray[Sel]->iIconUid,aSize);	
						}
						else
						{
							TInt RecNum = (iItemIconArray[Sel]->iIconUid.iUid);
				
							if(RecNum < iExtraFileRecognizer.Count() && RecNum >= 0)
							{
								if(iExtraFileRecognizer[RecNum])
								{
									RetIcon = iExtraFileRecognizer[RecNum]->GetIconL(iItemIconArray[Sel]->iIndex,aSize);
								}
							}
						}
					}
				}
				
				if(RetIcon == NULL)
				{
					RetIcon = AddIconFromFileL(aSize,iConFile,8,9);	// file
				}
			}
			break;
		};
	});
	
	return RetIcon;
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
CArrayPtr<CGulIcon>* CIconHandler::GetIconArrayL(void)
{
    TSize IconSiz(0,0);
	
	return GetIconArrayL(IconSiz);
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/

CArrayPtr<CGulIcon>* CIconHandler::GetIconArrayL(TSize aSize)
    {
	CArrayPtr<CGulIcon>* icons =new( ELeave ) CAknIconArray(1);
	CleanupStack::PushL(icons);	
	
	if(iConFile.Length())
	{
		icons->AppendL(AddIconFromFileL(aSize,iConFile,0,1)); // sel		
		icons->AppendL(AddIconFromFileL(aSize,iConFile,2,3));	// extra folder
		icons->AppendL(AddIconFromFileL(aSize,iConFile,4,5));	// drive
		icons->AppendL(AddIconFromFileL(aSize,iConFile,6,7));	// folder
		icons->AppendL(AddIconFromFileL(aSize,iConFile,8,9));	// file		
		
		for(TInt i=0; i < iItemIconArray.Count(); i++)
		{
			if(iItemIconArray[i])
			{
				if(iItemIconArray[i]->iSystemApp)
				{
					icons->AppendL(GetAppIconL(iItemIconArray[i]->iIconUid,aSize));	
				}
				else
				{
					CGulIcon* AddIcon(NULL);
					TInt RecNum = (iItemIconArray[i]->iIconUid.iUid);
					
					if(RecNum < iExtraFileRecognizer.Count() && RecNum >= 0)
					{
						if(iExtraFileRecognizer[RecNum])
						{
							AddIcon = iExtraFileRecognizer[RecNum]->GetIconL(iItemIconArray[i]->iIndex,aSize);
						}
					}
					
					if(AddIcon)
					{
						icons->AppendL(AddIcon);
					}
					else
					{
						icons->AppendL(AddIconFromFileL(aSize,iConFile,14,15));	// file
					}
				}
			}
		}
	}
	
	CleanupStack::Pop(); //icons
	return icons;
    }

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
CGulIcon* CIconHandler::AddIconFromFileL(const TSize& aIconsize,const TDesC& aFileName, TInt aImg, TInt aMsk)
{
	CFbsBitmap* FrameMsk(NULL);
	CFbsBitmap* FrameImg(NULL);
	
	TInt MaskId(aMsk);
	TInt UseIndex(aImg);
		
	if(AknIconUtils::IsMifFile(aFileName))
	{
		AknIconUtils::ValidateLogicalAppIconId(aFileName, UseIndex, MaskId);	
	}
	
	if(MaskId >= 0)
	{
		AknIconUtils::CreateIconL(FrameImg,FrameMsk,aFileName,UseIndex,MaskId);
	}
	else
	{
		FrameImg = AknIconUtils::CreateIconL(aFileName, UseIndex);
	}
	
	if(FrameImg)
	{
		if(aIconsize.iHeight > 0 && aIconsize.iWidth > 0)
		{
			AknIconUtils::SetSize(FrameImg,aIconsize,EAspectRatioPreserved);  
		}
					
		if(FrameMsk && aIconsize.iHeight > 0 && aIconsize.iWidth > 0)
		{
			AknIconUtils::SetSize(FrameMsk,aIconsize,EAspectRatioPreserved);	
		}
	}
	
	return CGulIcon::NewL(FrameImg,FrameMsk);
} 
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
CGulIcon* CIconHandler::GetAppIconL(const TUid& aAppUid,TSize aSize)
{
	MAknsSkinInstance* skin = AknsUtils::SkinInstance();
  
 	CFbsBitmap* BgPic(NULL);
	CFbsBitmap* BgMsk(NULL);
		
    AknsUtils::CreateAppIconLC(skin,aAppUid,  EAknsAppIconTypeContext,BgPic,BgMsk);

	if(BgPic && aSize.iHeight > 0 && aSize.iWidth > 0)
	{
		AknIconUtils::SetSize(BgPic,aSize);  
	}
	
	if(BgMsk && aSize.iHeight > 0 && aSize.iWidth > 0)
	{
		AknIconUtils::SetSize(BgMsk,aSize);  
	}
	
	CleanupStack::Pop(2);
	
	return CGulIcon::NewL(BgPic,BgMsk);
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CIconHandler::ReReadIcosL(void)
{	
	GetSupportedFilesArrayL();
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CIconHandler::GetSupportedFilesArrayL()
{
	iItemTypeArray.ResetAndDestroy();
	iItemIconArray.ResetAndDestroy();

	TFileName HelpFn,HelpFn2;
	TBuf8<100> Hjelppp;

	RApaLsSession ls;
	User::LeaveIfError(ls.Connect());
	CleanupClosePushL(ls);
	
	CDataTypeArray* allDataTypes = new(ELeave)CDataTypeArray(50);
	CleanupStack::PushL(allDataTypes);
	ls.GetSupportedDataTypesL(*allDataTypes);
	
	TBool Isthere(EFalse);
	
	TRAPD(Err,
	for(TInt ii=0; ii < iExtraFileRecognizer.Count(); ii++)
	{
		if(iExtraFileRecognizer[ii])
		{
			for(TInt i=0; i < iExtraFileRecognizer[ii]->FileTypeCount(); i++)
			{
				HelpFn.Zero();
				iExtraFileRecognizer[ii]->FileTypeL(i,HelpFn);//,newItem->iPartialSting);
				if(HelpFn.Length())
				{
					HelpFn.CopyLC(HelpFn);
					
					Isthere = EFalse;
					for(TInt c=0; c < allDataTypes->Count(); c++)
					{
						HelpFn2.CopyLC(allDataTypes->At(c).Des());
						if(HelpFn2 == HelpFn)
						{
							Isthere = ETrue;
						}
					}
					
					if(!Isthere)
					{	
						if(HelpFn.Length() > 100)
							Hjelppp.Copy(HelpFn.Left(99));
						else
							Hjelppp.Copy(HelpFn);
						
						allDataTypes->AppendL(TDataType(Hjelppp));
					}
				}
			}
		}
	}
	);	
		
	TKeyArrayFix SortKey(0, ECmpNormal);	
	allDataTypes->Sort(SortKey);

	TApaAppInfo AppInfo;	
	TUid HandlerAppUid;
	
	TBool CheckUID(EFalse);
	TInt Errnum(KErrNone);

	for(TInt ty=(allDataTypes->Count()- 1); ty >=0 ; ty--)
	{
		HelpFn2.Copy(allDataTypes->At(ty).Des());
		if(HelpFn2.Length())
		{
			CFFileTypeItem* newItem = new(ELeave)CFFileTypeItem();
			iItemTypeArray.Append(newItem);
						
			TRAPD(Err,
			for(TInt ii=0; ii < iExtraFileRecognizer.Count(); ii++)
			{
				if(iExtraFileRecognizer[ii])
				{
					for(TInt i=0; i < iExtraFileRecognizer[ii]->FileTypeCount(); i++)
					{
						HelpFn.Zero();
						iExtraFileRecognizer[ii]->FileTypeL(i,HelpFn);
						
						HelpFn.TrimAll();
						HelpFn.CopyLC(HelpFn);
						if(HelpFn == HelpFn2)
						{
							CIconMapping* NewMap = new(ELeave)CIconMapping();
							NewMap->iSystemApp 		= EFalse;
							NewMap->iIconUid.iUid   =ii;
							NewMap->iIndex	   		=i;
							
							iItemIconArray.Append(NewMap);
							
							newItem->iIdString = HelpFn2.AllocL();
							newItem->iTypeId = (CFFileItem::EFJustFile + iItemIconArray.Count());
							break;
						}
					}
				}
			});
		
		
			if(newItem->iIdString == NULL)
			{
				newItem->iIdString = HelpFn2.AllocL();
				newItem->iTypeId = CFFileItem::EFJustFile;
				
				HandlerAppUid.iUid = 0;
				
				Errnum = KErrNone;
				CheckUID = EFalse;
				
				TRAP(Errnum,
				if(iDocumentHandler->CanOpenL(allDataTypes->At(ty)))
				{
					CheckUID = ETrue;
				}
				
				if(!CheckUID)
				{
					if(iDocumentHandler->CanHandleL(allDataTypes->At(ty)))
					{
						CheckUID = ETrue;
					}	
				}
				
				if(!CheckUID)
				{
					if(iDocumentHandler->CanSaveL(allDataTypes->At(ty)))
					{
						CheckUID = ETrue;
					}	
				}
				
				if(CheckUID)
				{
					iDocumentHandler->HandlerAppUid(HandlerAppUid);
				}	
				);
				
				if(Errnum != KErrNone && Errnum != KErrNotSupported)
				{
					ls.AppForDataType(allDataTypes->At(ty),HandlerAppUid);
				}
				
				if(((TUint)HandlerAppUid.iUid) > 0)
				{	
					TBool FoundMe(EFalse);
					
					for(TInt yy=0; yy < iItemIconArray.Count(); yy++)
					{
						if(iItemIconArray[yy])
						{
							if(((TUint)(iItemIconArray[yy]->iIconUid.iUid)) == ((TUint)HandlerAppUid.iUid))
							{
								newItem->iTypeId = (CFFileItem::EFJustFile + yy + 1);
								FoundMe = ETrue;
								break;
							}
						}
					}
					
					if(!FoundMe)
					{
						CIconMapping* NewMap = new(ELeave)CIconMapping();
						NewMap->iSystemApp 		= ETrue;
						NewMap->iIconUid.iUid   =((TUint)HandlerAppUid.iUid);
						NewMap->iIndex	   		=-1;
						
						iItemIconArray.Append(NewMap);
						
						newItem->iTypeId = (CFFileItem::EFJustFile + iItemIconArray.Count());
					}
				}	
			}
		}
	}
	
	CleanupStack::PopAndDestroy(2);//ls,allDataTypes
}

