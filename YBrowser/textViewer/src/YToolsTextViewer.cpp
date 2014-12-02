/*
	Copyright (C) 2006 Jukka Silvennoinen
	All right reserved
*/

// INCLUDE FILES
#include <avkon.hrh>
#include <APPARC.H>
#include <aknnotewrappers.h>
#include <stringloader.h>
#include <aknnotewrappers.h>
#include <BAUTILS.H> 
#include <ecom.h>
#include <implementationproxy.h>
#include <aknmessagequerydialog.h> 
#include <akniconarray.h> 
#include <akntitle.h> 
#include <akncontext.h> 
#include <TXTRICH.H>
#include <CHARCONV.H>
#include <eikmenub.h> 
#include <eikstart.h>
#include <stringloader.h>

#include "YToolsTextViewer.h"
#include "YuccaBrowser.hrh"


#include "icons.mbg"
#include "Help_Container.h"
#include "FileReader.h"

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
	

const TInt KStatusLineOffset = 6;
/*
---------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------
*/
CYBrowserFileHandler1::~CYBrowserFileHandler1()
{
	if(iProgressDialog)
	{
		iProgressDialog->ProcessFinishedL(); 
	}

	iProgressDialog = NULL;
	iProgressInfo = NULL;
	
	delete iMyHelpContainer;
	iMyHelpContainer = NULL;
		
	delete iFileReader;
	iFileReader = NULL;
		
/*	if(iResId > 0)
	{
		CEikonEnv::Static()->DeleteResourceFile(iResId);
	}
*/
	delete iEditor;
	iEditor=NULL;
	delete iParaformMe;
	delete iCharformMe;
	delete iRichMessageMe; 
	
	delete iModel;
	delete iFindList;
	delete iReplaceList;
	
	if(iDebugFile.SubSessionHandle())
	{
		iDebugFile.Write(_L8("Bye, bye, "));	
	  	iDebugFile.Close();
	}
	
	REComSession::DestroyedImplementation(iDestructorIDKey);
}
/*
-------------------------------------------------------------------------
this will be called right after first constructor has finished
there shouldn't be any need to chnage this one.
-------------------------------------------------------------------------
*/
void CYBrowserFileHandler1::SetUtils(MYBrowserFileHandlerUtils* aFileHandlerUtils)
{
	iFileHandlerUtils = aFileHandlerUtils;
}

/*
-------------------------------------------------------------------------
This will be called when this file shandler is preraped to be used for, 
opening or constructing a file. The open/new etc. functions are called
strait after calling this function.
-------------------------------------------------------------------------
*/
void CYBrowserFileHandler1::ConstructL()
{
 	CreateWindowL();
 	
 /*	iResId = -1;
 	TFindFile AppFile(CCoeEnv::Static()->FsSession());
	if(KErrNone == AppFile.FindByDir(KTextResourceFileName, KNullDesC))
	{
 		TFileName resourceName(AppFile.File());
		BaflUtils::NearestLanguageFile(CEikonEnv::Static()->FsSession(), resourceName);
		iResId = CEikonEnv::Static()->AddResourceFileL(resourceName);
	}
*/
	
//	_LIT(KRecFilename			,"C:\\ZipUI.txt");
//	CEikonEnv::Static()->FsSession().Delete(KRecFilename);		
//	User::LeaveIfError(iDebugFile.Create(CEikonEnv::Static()->FsSession(),KRecFilename,EFileWrite|EFileShareAny));	

	if(iDebugFile.SubSessionHandle())
	{	
		iDebugFile.Write(_L8("start, "));
	}
	
	
	SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect()); 	
	
	iFileHandlerUtils->GetFileUtils().SetNaviTextL(_L(""));	
	
 	ActivateL();
	SetMenuL();
}
/*
-----------------------------------------------------------------------
just a internal function for constructing UI listbox
-----------------------------------------------------------------------
*/
void CYBrowserFileHandler1::MakeEdwinL(void)
{	
	if(iDebugFile.SubSessionHandle())
	{	
		iDebugFile.Write(_L8("Make box, "));
	}
	
	delete iEditor;
	iEditor = NULL;	
	iEditor = new (ELeave) CEikRichTextEditor;
	
	iEditor->SetContainerWindowL(*this);
	iEditor->SetAvkonWrap(ETrue);
	
	iEditor->ConstructL(this,0,0,0);
	//iEditor->ConstructL(this,10,0,EAknEditorFlagDefault |EAknEditorFlagEnableScrollBars,EGulFontControlAll,EGulAllFonts);
	iEditor->SetAknEditorFlags(EAknEditorFlagEnableScrollBars);
	
//	iEditor->InitFindL(iModel,iFindList,iReplaceList);
	
	iEditor->SetRect(Rect());
//	iEditor->SetMaxLength(250);

	if(iParaformMe == NULL || iCharformMe == NULL || iRichMessageMe == NULL)
	{
		TCharFormat		charFormat;	
		TCharFormatMask charFormatMask;	
	
		_LIT(KFontArial,"Arial");
			
		TFontSpec MyeFontSpec (KFontArial, 12*10); // 12 points = 1/6 inch
			
		MyeFontSpec.iTypeface.SetIsProportional(ETrue); // Arial is proportional 
		charFormat.iFontSpec = MyeFontSpec;
	
		charFormatMask.SetAll();
		
		delete iParaformMe;
		iParaformMe = NULL;
		iParaformMe = CParaFormatLayer::NewL(); 
		
		delete iCharformMe;
		iCharformMe = NULL;
		iCharformMe = CCharFormatLayer::NewL(charFormat,charFormatMask); 
		
		delete iRichMessageMe;
		iRichMessageMe = NULL;
		iRichMessageMe  = CRichText::NewL(iParaformMe,iCharformMe);
	}
	
	iEditor->SetAknEditorFlags(EAknEditorFlagEnableScrollBars);
	TRect rect = Rect();
	
	iEditor->CreatePreAllocatedScrollBarFrameL();
	iEditor->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);
	
	CEikScrollBarFrame* hhh = iEditor->ScrollBarFrame();
	if(hhh)
	{	
		CEikScrollBar* cccc = hhh->VerticalScrollBar();
		if(cccc)
		{
			TRect ScrollBarRect = cccc->Rect();
			iEditor->SetExtent(TPoint(0,0), TSize(rect.Width()-ScrollBarRect.Width(), rect.Height()));
		}
	}
	iEditor->SetFocus(ETrue);
	iEditor->SetDocumentContentL(*iRichMessageMe);
	iEditor->UpdateScrollBarsL();
	iEditor->DrawNow();
}

/*
-----------------------------------------------------------------------------
Y-Browser will call setsize for client rect when lay-out or size changes,
This will be then called by the framework
-----------------------------------------------------------------------------
*/
void CYBrowserFileHandler1::SizeChanged()
{
	MakeEdwinL();
	
	if(iMyHelpContainer)
	{
		iMyHelpContainer->SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect()); // Sets rectangle of lstbox.
	}
}

/*
-------------------------------------------------------------------------
Y-Browser will call this when focus changes.

i.e. When you ask y-browser to open a file, and it will be opened by
other y-browser handler, then you will be called with focus false, 
right before new handler takes focus.

And when it returns back to y-browser, this function will be called with
ETrue,
-------------------------------------------------------------------------
*/
void CYBrowserFileHandler1::SetFocusL(TBool aFocus)
{
	if(iDebugFile.SubSessionHandle())
	{	
		iDebugFile.Write(_L8("focusing, "));
	}
	
	if(aFocus)
	{
		CEikStatusPane*		sp = ((CAknAppUi*)iEikonEnv->EikAppUi())->StatusPane();
		if(sp)
		{
			CAknTitlePane* TitlePane = STATIC_CAST(CAknTitlePane*,sp->ControlL(TUid::Uid(EEikStatusPaneUidTitle)));
			TitlePane->SetTextL(KTextApplicationName);
				
	    	CAknContextPane* 	ContextPane = (CAknContextPane *)sp->ControlL(TUid::Uid(EEikStatusPaneUidContext));
			if(ContextPane)
			{
				TFindFile AppFile(CCoeEnv::Static()->FsSession());
				if(KErrNone == AppFile.FindByDir(KTextIconsFileName, KNullDesC))
				{
					ContextPane->SetPictureFromFileL(AppFile.File(),EMbmIconsB,EMbmIconsB_m);  
				}
			}				
		}		
	
		SetMenuL();
		DrawNow();	
	}
}
/*
-------------------------------------------------------------------------
internal function for updating menu and CBA's for the application
-------------------------------------------------------------------------
*/
void CYBrowserFileHandler1::SetMenuL(void)
{
	if(iFileHandlerUtils)
	{
		if(iMyHelpContainer)
		{
			iFileHandlerUtils->GetCba().SetCommandSetL(R_PLUG_APPHELP_CBA);				
			iFileHandlerUtils->GetCba().DrawDeferred();
		}
		else
		{
			iFileHandlerUtils->GetCba().SetCommandSetL(R_PLUG_MAIN_CBA);				
			iFileHandlerUtils->GetCba().DrawDeferred();
		}
	}
	
	CEikonEnv::Static()->AppUiFactory()->MenuBar()->SetMenuTitleResourceId(R_TE_MAIN_MENUBAR);	
}
/*
-------------------------------------------------------------------------
Normal DynInitMenuPaneL, which Y-Browser will call.

See SDK documentation for more information
-------------------------------------------------------------------------
*/
void CYBrowserFileHandler1::DynInitMenuPaneL(TInt /*aResourceId*/, CEikMenuPane* /*aMenuPane*/)
{
	if(iDebugFile.SubSessionHandle())
	{	
		iDebugFile.Write(_L8("DynMenu, "));
	}
	
/*	if(aResourceId == R_MAIN_MENU)
	{
		TBool DimExtract(ETrue);
		if(iSelectionBox)
		{
			if(iSelectionBox->CurrentItemIndex() >= 0)
			{
				DimExtract = EFalse;
			}
		}
		
		if(DimExtract)
		{
			aMenuPane->SetItemDimmed(EExtract,ETrue);	
		}
	}*/
}

/*
-------------------------------------------------------------------------
we only support one file type, so we don't care what aFileTypeID is
-------------------------------------------------------------------------
*/
void CYBrowserFileHandler1::OpenFileL(const TDesC& aFileName,const TDesC& /*aFileTypeID*/)	
{
	if(iDebugFile.SubSessionHandle())
	{	
		iDebugFile.Write(_L8("Openfile, "));
	}
	
	if(BaflUtils::FileExists(CCoeEnv::Static()->FsSession(),aFileName)
	&& aFileName.Length())
	{
		delete iFileReader;
		iFileReader = NULL;
		iFileReader = CFileReader::NewL(CCoeEnv::Static()->FsSession(),*this);
		TInt FilSiz = iFileReader->ReadFileL(aFileName);
		
		if(FilSiz > (ReadAtOneTime * 4))
		{
			iProgressDialog = new (ELeave) CAknProgressDialog((REINTERPRET_CAST(CEikDialog**,&iProgressDialog)),ETrue);
			iProgressDialog->PrepareLC(R_PROGRESS_NOTE);
			iProgressInfo = iProgressDialog->GetProgressInfoL();
			iProgressDialog->SetCallback(this);
			iProgressDialog->RunLD();
			iProgressInfo->SetFinalValue(FilSiz);
		}
	}
	else
	{
		if(iFileHandlerUtils)
		{
			iFileHandlerUtils->GetFileUtils().ShowFileErrorNoteL(aFileName,KErrNotFound);
		}
		HandleCommandL(EBacktoYBrowser);
	}
}
/*
-------------------------------------------------------------------------
we only support one file type, so we don't care what aFileTypeID is
-------------------------------------------------------------------------
*/
void CYBrowserFileHandler1::OpenFileL(RFile& aOpenFile,const TDesC& /*aFileTypeID*/)
{
	if(iDebugFile.SubSessionHandle())
	{	
		iDebugFile.Write(_L8("Openfile, "));
	}
	
	if(aOpenFile.SubSessionHandle())
	{
		delete iFileReader;
		iFileReader = NULL;
		iFileReader = CFileReader::NewL(CCoeEnv::Static()->FsSession(),*this);
		TInt FilSiz = iFileReader->ReadFileL(aOpenFile);
		
		if(FilSiz > (ReadAtOneTime * 4))
		{
			iProgressDialog = new (ELeave) CAknProgressDialog((REINTERPRET_CAST(CEikDialog**,&iProgressDialog)),ETrue);
			iProgressDialog->PrepareLC(R_PROGRESS_NOTE);
			iProgressInfo = iProgressDialog->GetProgressInfoL();
			iProgressDialog->SetCallback(this);
			iProgressDialog->RunLD();
			iProgressInfo->SetFinalValue(FilSiz);
		}	
	}
	else
	{
		if(iFileHandlerUtils)
		{
			iFileHandlerUtils->GetFileUtils().ShowFileErrorNoteL(KNullDesC,KErrBadHandle);
		}
		
		HandleCommandL(EBacktoYBrowser);
	}
}

/*
-------------------------------------------------------------------------
we only support one filetype, so no need to wonder what aFileTypeID is

-------------------------------------------------------------------------
*/
void CYBrowserFileHandler1::NewFileL(const TDesC& /*aPath*/,const TDesC& /*aFileTypeID*/)
{
	HandleCommandL(EBacktoYBrowser);
}

void CYBrowserFileHandler1::NewFileL(const TDesC& /*aPath*/,const TDesC& /*aFileTypeID*/,MDesCArray& /*aFileArray*/)
{
	HandleCommandL(EBacktoYBrowser);
}
/*
-------------------------------------------------------------------------
we are not supporting adding files to existing zip files
so no need to implement this function
-------------------------------------------------------------------------
*/
void CYBrowserFileHandler1::AddFilesL(const TDesC& /*aFileName*/,MDesCArray& /*aFileArray*/)
{	
	HandleCommandL(EBacktoYBrowser);
}
/*
-------------------------------------------------------------------------
Handle command called by Y-Browser,make sure you set your menu & CBA's 
when focus chnages, and that your commands are started at least with
first command as 0x7000.

-------------------------------------------------------------------------
*/

void CYBrowserFileHandler1::HandleCommandL(TInt aCommand)
{
	if(iDebugFile.SubSessionHandle())
	{	
		iDebugFile.Write(_L8("cmd, "));
	}
	
	switch (aCommand)
    { 
	case EPlugAppHelpBack:
		{
			delete iMyHelpContainer;
			iMyHelpContainer = NULL;
		}
		SetMenuL();
		DrawNow();
		break;
	case EPlugAppHelp:
		{
			delete iMyHelpContainer;
			iMyHelpContainer = NULL;    		
			iMyHelpContainer = CMyHelpContainer::NewL();
		}
		SetMenuL();
		DrawNow();    		
		break;    
 /*   case EFindNew:
    	if(iEditor)
    	{
    		iEditor->RunFindDialogL();
    	}
    	break;*/
    case EReReadFile:
    	if(iFileReader)
    	{
    		if(AskToSelectReadStyleL())
    		{
    			if(iEditor && iRichMessageMe)
				{
					iRichMessageMe->Reset();
					iEditor->SetDocumentContentL(*iRichMessageMe);
					iEditor->UpdateScrollBarsL();
					iEditor->DrawNow();
				}
			    		
	    		TInt FilSiz = iFileReader->ReReadFileL();
	    		if(FilSiz > (ReadAtOneTime * 4))
				{
					iProgressDialog = new (ELeave) CAknProgressDialog((REINTERPRET_CAST(CEikDialog**,&iProgressDialog)),ETrue);
					iProgressDialog->PrepareLC(R_PROGRESS_NOTE);
					iProgressInfo = iProgressDialog->GetProgressInfoL();
					iProgressDialog->SetCallback(this);
					iProgressDialog->RunLD();
					iProgressInfo->SetFinalValue(FilSiz);
				}
    		}
    	}
    	break;
    case EPlugAbout:
    	{
    		HBufC* Abbout = KTextAbbout().AllocLC();
			TPtr Pointter(Abbout->Des());
			CAknMessageQueryDialog* dlg = CAknMessageQueryDialog::NewL(Pointter);
			dlg->PrepareLC(R_ABOUT_HEADING_PANE);
			dlg->SetHeaderTextL(KTextApplicationName);  
			dlg->RunLD();
			
			CleanupStack::PopAndDestroy(Abbout);
    	}
    	break;
    case EBacktoYBrowser:
    	if(iFileHandlerUtils)
    	{
    		iFileHandlerUtils->HandleExitL(this,MYBrowserFileHandlerUtils::ENoChangesMade);	
    	}
    	break; 
	default:
		break;
    };
}
/*
-------------------------------------------------------------------------
-------------------------------------------------------------------------
*/
TBool CYBrowserFileHandler1::AskToSelectReadStyleL(void)
{
	TBool OkToContinue(EFalse);

	if(iFileReader)
	{
		CAknSinglePopupMenuStyleListBox* list = new(ELeave) CAknSinglePopupMenuStyleListBox;
	    CleanupStack::PushL(list);

		CAknPopupList* popupList = CAknPopupList::NewL(list,R_AVKON_SOFTKEYS_OK_CANCEL);
	    CleanupStack::PushL(popupList);
		
	    list->ConstructL(popupList, CEikListBox::ELeftDownInViewRect);
		list->CreateScrollBarFrameL(ETrue);
	    list->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,CEikScrollBarFrame::EAuto);
	    
	    CDesCArrayFlat* Ittems = new(ELeave)CDesCArrayFlat(5);
	    CleanupStack::PushL(Ittems);

	    TBuf<100> Hjrlppp;
	    StringLoader::Load(Hjrlppp,R_SH_STR_TXASCII);
	    Ittems->AppendL(Hjrlppp);
	    
	    StringLoader::Load(Hjrlppp,R_SH_STR_TXUNICOD);
	    Ittems->AppendL(Hjrlppp);
	    
	    StringLoader::Load(Hjrlppp,R_SH_STR_TXHEX);
	    Ittems->AppendL(Hjrlppp);
	    
	    StringLoader::Load(Hjrlppp,R_SH_STR_TXSELCONVERT);
		Ittems->AppendL(Hjrlppp);
		
	    CleanupStack::Pop(Ittems);
	    list->Model()->SetItemTextArray(Ittems);
		list->Model()->SetOwnershipType(ELbmOwnsItemArray);

		StringLoader::Load(Hjrlppp,R_SH_STR_TXREDFILAS);		        
		popupList->SetTitleL(Hjrlppp);
		
		if (popupList->ExecuteLD())
	    {
			TInt Ret = list->CurrentItemIndex();
			switch(Ret)
			{
			case 0:
				OkToContinue = ETrue;
				iFileReader->SetReadStyle(CFileReader::ETextAscii);
				break;
			case 1:
				OkToContinue = ETrue;
				iFileReader->SetReadStyle(CFileReader::ETextUnicode);
				break;
			case 2:
				OkToContinue = ETrue;
				iFileReader->SetReadStyle(CFileReader::ETextHex);
				break;
			case 3:
				{
					TUint SelConv(0);
					OkToContinue = AskToSelectConverterL(SelConv);
					if(OkToContinue)
					{
						iFileReader->SetReadStyle(CFileReader::ETextCharConv,SelConv);
					}
					break;
				}
			};
	    }
	    
	    CleanupStack::Pop();            // popuplist
	    CleanupStack::PopAndDestroy();  // list
	}
	
	return OkToContinue;
}
/*
-------------------------------------------------------------------------
-------------------------------------------------------------------------
*/
TBool CYBrowserFileHandler1::AskToSelectConverterL(TUint & aConverter)
{
	TBool OkToContinue(EFalse);
	
	CAknSinglePopupMenuStyleListBox* list = new(ELeave) CAknSinglePopupMenuStyleListBox;
	CleanupStack::PushL(list);
	    
	CAknPopupList* popupList = CAknPopupList::NewL(list,R_AVKON_SOFTKEYS_OK_CANCEL);
    CleanupStack::PushL(popupList);
	
    list->ConstructL(popupList, CEikListBox::ELeftDownInViewRect);
	list->CreateScrollBarFrameL(ETrue);
    list->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,CEikScrollBarFrame::EAuto);
    
	CArrayFix<CCnvCharacterSetConverter::SCharacterSet>* CharSets = CCnvCharacterSetConverter::CreateArrayOfCharacterSetsAvailableLC(CCoeEnv::Static()->FsSession());

    CDesCArrayFlat* Ittems = new(ELeave)CDesCArrayFlat(5);
    CleanupStack::PushL(Ittems);
        
	for(TInt i=0; i < CharSets->Count(); i++)
	{
		Ittems->AppendL(CharSets->At(i).Name());
    }
    
    
    CleanupStack::Pop(Ittems);
    list->Model()->SetItemTextArray(Ittems);
	list->Model()->SetOwnershipType(ELbmOwnsItemArray);


	TBuf<100> Hjrlppp;
	StringLoader::Load(Hjrlppp,R_SH_STR_TXSELCONVERT);
	
	popupList->SetTitleL(Hjrlppp);
	
	if (popupList->ExecuteLD())
    {
    	TInt Ret = list->CurrentItemIndex();
		if(Ret >= 0 && Ret < CharSets->Count())
		{
			aConverter = CharSets->At(Ret).Identifier();
			OkToContinue = ETrue;
		}
    }
	
	CleanupStack::PopAndDestroy();  // CharSets
	CleanupStack::Pop();            // popuplist
	CleanupStack::PopAndDestroy();  // list
	
	return OkToContinue;
}
/*
-----------------------------------------------------------------------
normal implementation of the OfferKeyEventL, called  by Y-Browser when in focus

See SDK documentations for more information
-----------------------------------------------------------------------
*/
TKeyResponse CYBrowserFileHandler1::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
	TKeyResponse Ret = EKeyWasNotConsumed;
	
	if(iMyHelpContainer)
	{
		Ret = iMyHelpContainer->OfferKeyEventL(aKeyEvent,aType);
	}
	else
	{	
		switch (aKeyEvent.iCode)
		{
		case EKeyDevice3:
	/*		if(iEditor)
			{
				iEditor->DoFindL();
			}
	*/		CEikonEnv::Static()->AppUiFactory()->MenuBar()->TryDisplayMenuBarL();
			break;
		case EKeyUpArrow:
			if(iEditor)
			{
				iEditor->MoveCursorL(TCursorPosition::EFPageUp, EFalse);
				iEditor->UpdateScrollBarsL();
				iEditor->DrawNow();
				Ret = EKeyWasConsumed;
			}
			DrawNow();
			break;
		case EKeyDownArrow:
			if(iEditor)
			{
				iEditor->MoveCursorL(TCursorPosition::EFPageDown,EFalse);
				iEditor->UpdateScrollBarsL();
				iEditor->DrawNow();
				Ret = EKeyWasConsumed;
			}
			DrawNow();
		default:
			if(iEditor)
			{
		//		Ret = iEditor->OfferKeyEventL(aKeyEvent,aType);
			}
			break;
		};
	}
	
	return Ret;
}
/*
-----------------------------------------------------------------------
just normal Draw

See SDK documentations for more information
-----------------------------------------------------------------------
*/
void CYBrowserFileHandler1::Draw(const TRect& aRect) const
{
 	CWindowGc& gc = SystemGc();
 	gc.Clear(aRect);
}
/*
-------------------------------------------------------------------------
just normal CountComponentControls

See SDK documentations for more information
-------------------------------------------------------------------------
*/
TInt CYBrowserFileHandler1::CountComponentControls() const
{
	if(iMyHelpContainer)
		return 1;
	else if(iEditor)
		return 1;
	else
		return 0;
}
/*
-------------------------------------------------------------------------
just normal ComponentControl

See SDK documentations for more information
-------------------------------------------------------------------------
*/
CCoeControl* CYBrowserFileHandler1::ComponentControl(TInt /*aIndex*/) const
{
	if(iMyHelpContainer)
		return iMyHelpContainer;
	else
		return iEditor;
}

/*
-----------------------------------------------------------------------------
normal MProgressDialogCallback callback function.

See SDK documentations for more information
----------------------------------------------------------------------------
*/
void CYBrowserFileHandler1::DialogDismissedL (TInt /*aButtonId*/)
{		  		
	iProgressDialog = NULL;
	iProgressInfo = NULL;
	
	if(iFileReader)
	{
		if(iFileReader->IsActive())
		{
			iFileReader->Cancel();
		}
	}	
	
	if(iEditor && iRichMessageMe)
	{
		iEditor->SetDocumentContentL(*iRichMessageMe);
		iEditor->SetDocumentContentL(*iRichMessageMe);
		iEditor->SetCursorPosL(0,EFalse);
	
		iEditor->SetFocus(EFalse);
		iEditor->SetFocusing(ETrue);
		iEditor->ClearSelectionL();
		
		iEditor->UpdateScrollBarsL();
		iEditor->DrawNow();
	}
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CYBrowserFileHandler1::ReadprocessL(const TDesC& aStuff, TInt aReadSoFar)
{
	if(iRichMessageMe)
	{
		TBuf<1> LineFeedBuf(_L("0"));
		LineFeedBuf[0] = 0x000A;

		TInt Ret = aStuff.Find(LineFeedBuf);
		if(Ret != KErrNotFound)
		{
			TInt i = 0;

			do
			{
				if(iRichMessageMe)
				{
					iRichMessageMe->InsertL(iRichMessageMe->LdDocumentLength(),aStuff.Mid(i,Ret));
					iRichMessageMe->InsertL(iRichMessageMe->LdDocumentLength(),CEditableText::ELineBreak);
				}

				i = i + Ret + 1;
				Ret = aStuff.Right(aStuff.Length() - i).Find(LineFeedBuf);

			}while(Ret != KErrNotFound && i < aStuff.Length());

			if(aStuff.Length() > i && iRichMessageMe)
				iRichMessageMe->InsertL(iRichMessageMe->LdDocumentLength(),aStuff.Right(aStuff.Length() - i));
		}
		else if(iRichMessageMe)
			iRichMessageMe->InsertL(iRichMessageMe->LdDocumentLength(),aStuff);
		
//		iRichMessageMe->InsertL(iRichMessageMe->LdDocumentLength(),aStuff);
	}
	
	if(iProgressInfo)
	{
		iProgressInfo->SetAndDraw(aReadSoFar);	
	}	
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CYBrowserFileHandler1::ReadDoneL(TInt aError)
{
	if(iProgressDialog)
	{
		iProgressDialog->ProcessFinishedL(); 
	}

	iProgressDialog = NULL;
	iProgressInfo = NULL;

	if(aError != KErrNone)
	{
		iFileHandlerUtils->GetFileUtils().ShowFileErrorNoteL(KNullDesC,aError);
	}
	
	if(iEditor && iRichMessageMe)
	{
		iEditor->SetDocumentContentL(*iRichMessageMe);
		iEditor->SetDocumentContentL(*iRichMessageMe);
		iEditor->SetCursorPosL(0,EFalse);
	
		iEditor->SetFocus(EFalse);
		iEditor->SetFocusing(ETrue);
		iEditor->ClearSelectionL();
		
		iEditor->UpdateScrollBarsL();
		
		CEikScrollBarFrame* hhh = iEditor->ScrollBarFrame();
		if(hhh)
		{	
			TRect rect(Rect());
			CEikScrollBar* cccc = hhh->VerticalScrollBar();
			if(cccc)
			{
				TRect ScrollBarRect = cccc->Rect();
				iEditor->SetExtent(TPoint(0,0), TSize(rect.Width()-ScrollBarRect.Width(), rect.Height()));
			}
		}
			
		iEditor->DrawNow();
	}
	
	DrawNow();
}

/*
-------------------------------------------------------------------------
interface function for getting the implementation instance
-------------------------------------------------------------------------
*/
CYBrowserFileHandler* NewFileHandler()
    {
    return (new CYBrowserFileHandler1);
    }
/*
-------------------------------------------------------------------------
ECom ImplementationTable function

See SDK documentations for more information
-------------------------------------------------------------------------
*/
#ifdef __SERIES60_3X__
LOCAL_D const TImplementationProxy ImplementationTable[] = 
{
	IMPLEMENTATION_PROXY_ENTRY(0x0F6A,NewFileHandler)
};
#else

// Define the interface UIDs
const TImplementationProxy ImplementationTable[] = 
    {
    {{0x0F6A}, NewFileHandler}
    };
    
// DLL Entry point
GLDEF_C TInt E32Dll(TDllReason /*aReason*/)
    {
    return(KErrNone);
    }
#endif


/*
-------------------------------------------------------------------------
ECom ImplementationGroupProxy function

See SDK documentations for more information
-------------------------------------------------------------------------
*/
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
{
	aTableCount = sizeof(ImplementationTable)/sizeof(ImplementationTable[0]);
	return ImplementationTable;
}

