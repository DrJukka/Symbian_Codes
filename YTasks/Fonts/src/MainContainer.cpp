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
#include <BACLIPB.H>
 
#include "MainContainer.h"
#include "Help_Container.h"
#include "YTools_A000312C.hrh"

#include "Common.h"

#ifdef SONE_VERSION
	#include "YTools_2002B0A6_res.rsg"
#else
	#ifdef LAL_VERSION
		#include "YTools_A000312C_res.rsg"
	#else
		#include "YTools_A000312C_res.rsg"
	#endif
#endif

#include <eikstart.h>
	
	

const TInt KAknExListFindBoxTextLength = 20;
/*
---------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------
*/
CMainContainer::~CMainContainer()
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
	
	delete iEditor;
	iEditor=NULL;
	delete iParaformMe;
	delete iCharformMe;
	delete iRichMessageMe; 
	
	delete iBgContext;
	iBgContext = NULL;
	
	if(iResId > 0)
	{
		CEikonEnv::Static()->DeleteResourceFile(iResId);
	}
	
	REComSession::DestroyedImplementation(iDestructorIDKey);
}


/*
-------------------------------------------------------------------------
This will be called when this file shandler is preraped to be used for, 
opening or constructing a file. The open/new etc. functions are called
strait after calling this function.
-------------------------------------------------------------------------
*/
void CMainContainer::ConstructL(CEikButtonGroupContainer* aCba)
{
	iCba = aCba;
	
 	CreateWindowL();
 	
 	iBgContext = CAknsBasicBackgroundControlContext::NewL(KAknsIIDQsnBgScreen,TRect(0,0,1,1), ETrue);
	
 	iResId = -1;
 	TFindFile AppFile(CCoeEnv::Static()->FsSession());
	if(KErrNone == AppFile.FindByDir(KFontsResourceFileName, KNullDesC))
	{
 		TFileName resourceName(AppFile.File());
		BaflUtils::NearestLanguageFile(CEikonEnv::Static()->FsSession(), resourceName);
		iResId = CEikonEnv::Static()->AddResourceFileL(resourceName);
	}
	
	iCharFormat.iFontSpec = AknLayoutUtils::FontFromId(EAknLogicalFontSecondaryFont)->FontSpecInTwips();
   	
   	MAknsSkinInstance* skin = AknsUtils::SkinInstance();
	TRgb textcol;
	AknsUtils::GetCachedColor(skin,textcol,KAknsIIDQsnTextColors,EAknsCIQsnTextColorsCG6);
	iCharFormat.iFontPresentation.iTextColor = TLogicalRgb(textcol);
	
   	iCharFormatMask.SetAll();
	
	SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect()); 	
		
	ActivateL();
	SetMenuL();
	DrawNow();
	ChangeIconAndNameL();
}
/*
-----------------------------------------------------------------------
just a internal function for constructing UI listbox
-----------------------------------------------------------------------
*/
void CMainContainer::MakeEdwinL(void)
{	
	delete iEditor;
	iEditor = NULL;	
	iEditor = new (ELeave) CEikGlobalTextEditor;
	iEditor->SetAknEditorFlags(EAknEditorFlagEnableScrollBars);
	iEditor->SetContainerWindowL(*this);
	iEditor->SetAvkonWrap(ETrue);
	iEditor->ConstructL(this,10,0,EAknEditorFlagDefault,EGulFontControlAll,EGulAllFonts);
	iEditor->SetBorder(TGulBorder::ESingleBlack);
	iEditor->EnableCcpuSupportL(ETrue);
	iEditor->SetAknEditorCurrentInputMode(EAknEditorTextInputMode);
    
	iEditor->SetRect(Rect());
//	iEditor->SetMaxLength(250);

	if(iParaformMe == NULL
	|| iCharformMe == NULL
	|| iRichMessageMe == NULL)
	{
		delete iParaformMe;
		iParaformMe = NULL;
		iParaformMe = CParaFormatLayer::NewL(); 
		
		delete iCharformMe;
		iCharformMe = NULL;
		iCharformMe = CCharFormatLayer::NewL(iCharFormat,iCharFormatMask); 
		
		delete iRichMessageMe;
		iRichMessageMe = NULL;
		iRichMessageMe  = CRichText::NewL(iParaformMe,iCharformMe);
	}
	
	ReadDoneL(KErrNone);
}

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CMainContainer::ChangeIconAndNameL(void)
{	
 	CEikStatusPane*		sp = ((CAknAppUi*)iEikonEnv->EikAppUi())->StatusPane();
	if(sp)
	{
		CAknTitlePane* TitlePane = STATIC_CAST(CAknTitlePane*,sp->ControlL(TUid::Uid(EEikStatusPaneUidTitle)));
		TitlePane->SetTextL(KFontsApplicationName);
			
    	CAknContextPane* 	ContextPane = (CAknContextPane *)sp->ControlL(TUid::Uid(EEikStatusPaneUidContext));
		if(ContextPane)
		{
			TFindFile AppFile(CCoeEnv::Static()->FsSession());
			if(KErrNone == AppFile.FindByDir(KFontsIconsFileName, KNullDesC))
			{
				TInt BgIndex(0),BgMask(1);
				
			/*	if(iServerIsOn)
				{
					BgIndex = 2;
					BgMask = 3;
				}
			*/	
				if(AknIconUtils::IsMifFile(AppFile.File()))
				{
					AknIconUtils::ValidateLogicalAppIconId(AppFile.File(), BgIndex,BgMask); 
				}
			
				ContextPane->SetPictureFromFileL(AppFile.File(),BgIndex,BgMask);  
			}
		}				
	}	
}



/*
-----------------------------------------------------------------------------
Y-Browser will call setsize for client rect when lay-out or size changes,
This will be then called by the framework
-----------------------------------------------------------------------------
*/
void CMainContainer::SizeChanged()
{
	if ( iBgContext )
	{
		iBgContext->SetRect(Rect());

		if ( &Window() )
		{
			iBgContext->SetParentPos( PositionRelativeToScreen() );
		}
	}
	
 	MakeEdwinL();
 	
 	if(iMyHelpContainer)
 	{
		iMyHelpContainer->SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect()); // Sets rectangle of lstbox.
 	} 
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::HandleResourceChange(TInt aType)
{	
	TRect rect;
	TBool SetR(EFalse);
	
#ifdef __SERIES60_3X__
    if ( aType==KEikDynamicLayoutVariantSwitch )
    {  	
    	SetR = ETrue;
    	
//		AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EStatusPaneBottom, Brect);
//		AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EStatusPane, Srect);
        AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, rect);
    }
#else
    if ( aType == 0x101F8121 )
    {
    	SetR = ETrue;
    	rect = CEikonEnv::Static()->EikAppUi()->ClientRect();
    }
#endif   
 
 	if(SetR)
 	{   
	    SetRect(rect);
 	}
 	
	CCoeControl::HandleResourceChange(aType);
}
/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
TTypeUid::Ptr CMainContainer::MopSupplyObject(TTypeUid aId)
{	
	if (iBgContext)
	{
		return MAknsControlContext::SupplyMopObject(aId, iBgContext );
	}
	
	return CCoeControl::MopSupplyObject(aId);
}

/*
-------------------------------------------------------------------------

-------------------------------------------------------------------------
*/
void CMainContainer::ForegroundChangedL(TBool /*aForeground*/)
{
	
}
/*
-------------------------------------------------------------------------

-------------------------------------------------------------------------
*/
void CMainContainer::SetMenuL(void)
{
	if(iMyHelpContainer)
	{
		iCba->SetCommandSetL(R_APPHELP_CBA);
		iCba->DrawDeferred();
	}
	else 
	{
		CEikonEnv::Static()->AppUiFactory()->MenuBar()->SetMenuTitleResourceId(R_MAIN_MENUBAR);				
		iCba->SetCommandSetL(R_MAIN_CBA);
		iCba->DrawDeferred();
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::InitMenuPanelL(TInt aResourceId, CEikMenuPane* aMenuPane)
{
	if(aResourceId == R_MAIN_MENU)
	{		
		if(!iFileReader)
		{
			aMenuPane->SetItemDimmed(EReReadFile,ETrue);
		}
	}
	else if(aResourceId == R_TEXT_MENU)
	{		
		aMenuPane->SetItemDimmed(EAddChar,ETrue);
	}
} 

/*
-------------------------------------------------------------------------
-------------------------------------------------------------------------
*/
void CMainContainer::OpenFileL(const TDesC& aFileName)	
{
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
}
/*
-------------------------------------------------------------------------

-------------------------------------------------------------------------
*/
TBool CMainContainer::Accept (const TDesC& /*aDriveAndPath*/, const TEntry& /*aEntry*/) const 
{
	TBool Ret(ETrue);

	return Ret;
}
/*
-------------------------------------------------------------------------

-------------------------------------------------------------------------
*/
void CMainContainer::SelectAndSetFontL(void)
{
	if(iRichMessageMe)
	{
		if(SelectFontL())
		{
		   	MAknsSkinInstance* skin = AknsUtils::SkinInstance();
			TRgb textcol;
			AknsUtils::GetCachedColor(skin,textcol,KAknsIIDQsnTextColors,EAknsCIQsnTextColorsCG6);
			iCharFormat.iFontPresentation.iTextColor = TLogicalRgb(textcol);
			
		   	iCharFormatMask.SetAll(); 
			
			iCharformMe->SetL(iCharFormat,iCharFormatMask);
			
			if(iRichMessageMe->LdDocumentLength() > 1)
			{
				iRichMessageMe->ApplyCharFormatL(iCharFormat,iCharFormatMask,0, (iRichMessageMe->LdDocumentLength() - 1));
			}
			
			MakeEdwinL();
		}
	}
}
/*
-------------------------------------------------------------------------
-------------------------------------------------------------------------
*/
TBool CMainContainer::SelectFontL(void)
{
	TBool OkToContinue(EFalse);

	CAknSinglePopupMenuStyleListBox* list = new(ELeave) CAknSinglePopupMenuStyleListBox;
    CleanupStack::PushL(list);

	CAknPopupList* popupList = CAknPopupList::NewL(list,R_AVKON_SOFTKEYS_OK_CANCEL);
    CleanupStack::PushL(popupList);
	
    list->ConstructL(popupList, CEikListBox::ELeftDownInViewRect);
	list->CreateScrollBarFrameL(ETrue);
    list->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,CEikScrollBarFrame::EAuto);
    
    CDesCArrayFlat* Ittems = new(ELeave)CDesCArrayFlat(5);
    CleanupStack::PushL(Ittems);
    
    Ittems->AppendL(_L("System font"));
    Ittems->AppendL(_L("Logical font"));
    Ittems->AppendL(_L("Typeface font"));
	
    CleanupStack::Pop(Ittems);
    list->Model()->SetItemTextArray(Ittems);
	list->Model()->SetOwnershipType(ELbmOwnsItemArray);

	popupList->SetTitleL(_L("Select Font:"));
	
	if (popupList->ExecuteLD())
    {
		TInt Ret = list->CurrentItemIndex();
		switch(Ret)
		{
		case 0:
			OkToContinue = SelectSystemFontL();
			break;
		case 1:
			OkToContinue = SelectLogicalFontL();
			break;
		case 2:
			OkToContinue = SelectTypeFaceFontL();
			break;
		};
    }
    
    CleanupStack::Pop();            // popuplist
    CleanupStack::PopAndDestroy();  // list

	return OkToContinue;
}
/*
-------------------------------------------------------------------------
-------------------------------------------------------------------------
*/
TBool CMainContainer::SelectTypeFaceFontL(void)
{
	TBool OkToContinue(EFalse);

	TInt NumTfaces = CEikonEnv::Static()->ScreenDevice()->NumTypefaces();
	if(NumTfaces > 0)
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
	    
	    TTypefaceSupport tfs;
	    
	    for(TInt i=0; i < NumTfaces; i++)
	    {
	    	CEikonEnv::Static()->ScreenDevice()->TypefaceSupport(tfs, i);
			Ittems->AppendL(tfs.iTypeface.iName);
	    }
		
	    CleanupStack::Pop(Ittems);
	    list->Model()->SetItemTextArray(Ittems);
		list->Model()->SetOwnershipType(ELbmOwnsItemArray);

		popupList->SetTitleL(_L("Select Typeface:"));
		
		if (popupList->ExecuteLD())
	    {
			TInt Ret = list->CurrentItemIndex();
			if(Ret >= 0 && Ret < NumTfaces)
			{
				TInt FHeight(100);
			
				CAknNumberQueryDialog* Dialog = CAknNumberQueryDialog::NewL(FHeight,CAknQueryDialog::ENoTone);
				Dialog->PrepareLC(R_BIG_DIALOG);
				Dialog->SetPromptL(_L("Font height"));
				if(Dialog->RunLD())
				{
					CEikonEnv::Static()->ScreenDevice()->TypefaceSupport(tfs, Ret);
					TFontSpec spec(tfs.iTypeface.iName, FHeight);
					
					CFont *UseFont(NULL);
	    			CEikonEnv::Static()->ScreenDevice()->GetNearestFontInTwips(UseFont, spec);
					
					OkToContinue = ETrue;
					iCharFormat.iFontSpec = UseFont->FontSpecInTwips();
				
					CEikonEnv::Static()->ScreenDevice()->ReleaseFont(UseFont);  
				}
			}
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
TBool CMainContainer::SelectSystemFontL(void)
{
	TBool OkToContinue(EFalse);

	CAknSinglePopupMenuStyleListBox* list = new(ELeave) CAknSinglePopupMenuStyleListBox;
    CleanupStack::PushL(list);

	CAknPopupList* popupList = CAknPopupList::NewL(list,R_AVKON_SOFTKEYS_OK_CANCEL);
    CleanupStack::PushL(popupList);
	
    list->ConstructL(popupList, CEikListBox::ELeftDownInViewRect);
	list->CreateScrollBarFrameL(ETrue);
    list->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,CEikScrollBarFrame::EAuto);
    
    CDesCArrayFlat* Ittems = new(ELeave)CDesCArrayFlat(5);
    CleanupStack::PushL(Ittems);
    
    Ittems->AppendL(_L("LatinPlain12"));
    Ittems->AppendL(_L("LatinBold12"));
    Ittems->AppendL(_L("LatinBold13"));
    Ittems->AppendL(_L("LatinBold16"));
    Ittems->AppendL(_L("LatinBold17"));
    Ittems->AppendL(_L("LatinBold19"));
    Ittems->AppendL(_L("NumberPlain5"));
    Ittems->AppendL(_L("ClockBold30"));
    Ittems->AppendL(_L("LatinClock14"));
    Ittems->AppendL(_L("ApacPlain12"));
    Ittems->AppendL(_L("ApacPlain16"));
	
    CleanupStack::Pop(Ittems);
    list->Model()->SetItemTextArray(Ittems);
	list->Model()->SetOwnershipType(ELbmOwnsItemArray);

	popupList->SetTitleL(_L("System Font:"));
	
	if (popupList->ExecuteLD())
    {
		TInt Ret = list->CurrentItemIndex();
		switch(Ret)
		{
		case 0:
			OkToContinue = ETrue;
			iCharFormat.iFontSpec = LatinPlain12()->FontSpecInTwips();
			break;
		case 1:
			OkToContinue = ETrue;
			iCharFormat.iFontSpec = LatinBold12()->FontSpecInTwips();
			break;
		case 2:
			OkToContinue = ETrue;
			iCharFormat.iFontSpec = LatinBold13()->FontSpecInTwips();
			break;
		case 3:
			OkToContinue = ETrue;
			iCharFormat.iFontSpec = LatinBold16()->FontSpecInTwips();
			break;
		case 4:
			OkToContinue = ETrue;
			iCharFormat.iFontSpec = LatinBold17()->FontSpecInTwips();
			break;
		case 5:
			OkToContinue = ETrue;
			iCharFormat.iFontSpec = LatinBold19()->FontSpecInTwips();
			break;
		case 6:
			OkToContinue = ETrue;
			iCharFormat.iFontSpec = NumberPlain5()->FontSpecInTwips();
			break;
		case 7:
			OkToContinue = ETrue;
			iCharFormat.iFontSpec = ClockBold30()->FontSpecInTwips();
			break;
		case 8:
			OkToContinue = ETrue;
			iCharFormat.iFontSpec = LatinClock14()->FontSpecInTwips();
			break;
		case 9:
			OkToContinue = ETrue;
			iCharFormat.iFontSpec = ApacPlain12()->FontSpecInTwips();
			break;
		case 10:
			OkToContinue = ETrue;
			iCharFormat.iFontSpec = ApacPlain16()->FontSpecInTwips();
			break;
		};
    }
    
    CleanupStack::Pop();            // popuplist
    CleanupStack::PopAndDestroy();  // list

	return OkToContinue;
}

/*
-------------------------------------------------------------------------
-------------------------------------------------------------------------
*/
TBool CMainContainer::SelectLogicalFontL(void)
{
	TBool OkToContinue(EFalse);

	CAknSinglePopupMenuStyleListBox* list = new(ELeave) CAknSinglePopupMenuStyleListBox;
    CleanupStack::PushL(list);

	CAknPopupList* popupList = CAknPopupList::NewL(list,R_AVKON_SOFTKEYS_OK_CANCEL);
    CleanupStack::PushL(popupList);
	
    list->ConstructL(popupList, CEikListBox::ELeftDownInViewRect);
	list->CreateScrollBarFrameL(ETrue);
    list->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,CEikScrollBarFrame::EAuto);
    
    CDesCArrayFlat* Ittems = new(ELeave)CDesCArrayFlat(5);
    CleanupStack::PushL(Ittems);
    
    Ittems->AppendL(_L("PrimaryFont"));
    Ittems->AppendL(_L("SecondaryFont"));
    Ittems->AppendL(_L("TitleFont"));
    Ittems->AppendL(_L("PrimarySmallFont"));
    Ittems->AppendL(_L("DigitalFont"));
    Ittems->AppendL(_L("AnnotationFont (env)"));
    Ittems->AppendL(_L("TitleFont (env)"));
    Ittems->AppendL(_L("LegendFont (env)"));
    Ittems->AppendL(_L("SymbolFont (env)"));
    Ittems->AppendL(_L("DenseFont (env)"));
	
    CleanupStack::Pop(Ittems);
    list->Model()->SetItemTextArray(Ittems);
	list->Model()->SetOwnershipType(ELbmOwnsItemArray);

	popupList->SetTitleL(_L("Logical Font:"));
	
	if (popupList->ExecuteLD())
    {
		TInt Ret = list->CurrentItemIndex();
		switch(Ret)
		{
		case 0:
			OkToContinue = ETrue;
			iCharFormat.iFontSpec = AknLayoutUtils::FontFromId(EAknLogicalFontPrimaryFont)->FontSpecInTwips();
			break;
		case 1:
			OkToContinue = ETrue;
			iCharFormat.iFontSpec = AknLayoutUtils::FontFromId(EAknLogicalFontSecondaryFont)->FontSpecInTwips();
			break;
		case 2:
			OkToContinue = ETrue;
			iCharFormat.iFontSpec = AknLayoutUtils::FontFromId(EAknLogicalFontTitleFont)->FontSpecInTwips();
			break;
		case 3:
			OkToContinue = ETrue;
			iCharFormat.iFontSpec = AknLayoutUtils::FontFromId(EAknLogicalFontPrimarySmallFont)->FontSpecInTwips();
			break;
		case 4:
			OkToContinue = ETrue;
			iCharFormat.iFontSpec = AknLayoutUtils::FontFromId(EAknLogicalFontDigitalFont)->FontSpecInTwips();
			break;
		case 5:
			OkToContinue = ETrue;
			iCharFormat.iFontSpec = CEikonEnv::Static()->AnnotationFont()->FontSpecInTwips();
			break;
		case 6:
			OkToContinue = ETrue;
			iCharFormat.iFontSpec = CEikonEnv::Static()->TitleFont()->FontSpecInTwips();
			break;
		case 7:
			OkToContinue = ETrue;
			iCharFormat.iFontSpec = CEikonEnv::Static()->LegendFont()->FontSpecInTwips();
			break;
		case 8:
			OkToContinue = ETrue;
			iCharFormat.iFontSpec = CEikonEnv::Static()->SymbolFont()->FontSpecInTwips();
			break;
		case 9:
			OkToContinue = ETrue;
			iCharFormat.iFontSpec = CEikonEnv::Static()->DenseFont()->FontSpecInTwips();
			break;
		};
    }
    
    CleanupStack::Pop();            // popuplist
    CleanupStack::PopAndDestroy();  // list

	return OkToContinue;
}	




/*
-------------------------------------------------------------------------

-------------------------------------------------------------------------
*/

void CMainContainer::HandleCommandL(TInt aCommand)
{	
	TFileName fileName;
	
	switch (aCommand)
    {
	case EAppHelpBack:
		{
			delete iMyHelpContainer;
			iMyHelpContainer = NULL;
		}
		SetMenuL();
		DrawNow();
		break;
	case EAppHelp:
		{
			delete iMyHelpContainer;
			iMyHelpContainer = NULL;    		
			iMyHelpContainer = CMyHelpContainer::NewL();
		}
		SetMenuL();
		DrawNow();    		
		break;	    
    case EAddChar:
    	{
    		
    	}
    	break;
    case EAddCharOk:
    	{
    		
    	}
    case EAddCharCancel:
    	{
    		
    	}
    	break;
    case EPasteText:
    	{
    		CPlainText* BPlainText = CPlainText::NewL();
			CleanupStack::PushL(BPlainText);
			
			CClipboard* cb = CClipboard::NewForReadingL(CCoeEnv::Static()->FsSession());
			CleanupStack::PushL(cb);
			cb->StreamDictionary().At(KClipboardUidTypePlainText);

			BPlainText->PasteFromStoreL(cb->Store(),cb->StreamDictionary(),0);

			// the text is now inside the BPlainText 
			if(BPlainText->DocumentLength())
			{
				ReadprocessL(BPlainText->Read(0),0);
				ReadDoneL(KErrNone);
			}
			
	//		cb->CommitL();
			CleanupStack::PopAndDestroy(); // cb
			CleanupStack::PopAndDestroy(); // CBPlainText
    	}
    	DrawNow();
    	break;
	case EAddText:
		{
			CAknTextQueryDialog* Dialog = CAknTextQueryDialog::NewL(fileName,CAknQueryDialog::ENoTone);
			Dialog->PrepareLC(R_ASK_NAME_DIALOG);
			Dialog->SetPromptL(_L("Add text"));
			Dialog->SetPredictiveTextInputPermitted( ETrue );
			//Dialog->SetMaxLength(250);
			if(Dialog->RunLD())
			{
				ReadprocessL(fileName,0);
				ReadDoneL(KErrNone);
			}
    	}
    	break;
	case EClearText:
		if(iEditor && iRichMessageMe)
		{
			iRichMessageMe->Reset();
			
			iEditor->SetDocumentContentL(*iRichMessageMe);
			iEditor->SetDocumentContentL(*iRichMessageMe);
			
			iEditor->SetFocus(EFalse);
			iEditor->SetFocusing(ETrue);
			iEditor->ClearSelectionL();
		
			iEditor->UpdateScrollBarsL();
			iEditor->DrawNow();
    	}
    	DrawNow();
    	break;
    case ESelectFile:
    	{
			if(AknCommonDialogs::RunSelectDlgLD (fileName, R_SELAUDIO_PROMPT,this,NULL))
			{
				OpenFileL(fileName);
			}
    	} 
    	break;
    case EChangeFont:
    	{
    		SelectAndSetFontL();
    	} 
    	break;
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
    case EAbout:
    	{
    		HBufC* Abbout = KFontsAbbout().AllocLC();
			TPtr Pointter(Abbout->Des());
			CAknMessageQueryDialog* dlg = CAknMessageQueryDialog::NewL(Pointter);
			dlg->PrepareLC(R_ABOUT_HEADING_PANE);
			dlg->SetHeaderTextL(KFontsApplicationName);  
			dlg->RunLD();
			
			CleanupStack::PopAndDestroy(Abbout);
    	}
    	break;
    case EBacktoYTasks:
    	if(iTasksHandlerExit)
    	{
    		iTasksHandlerExit->HandlerExitL(this);	
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
TBool CMainContainer::AskToSelectReadStyleL(void)
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
	    
	    Ittems->AppendL(_L("Ascii"));
	    Ittems->AppendL(_L("Unicode"));
	    Ittems->AppendL(_L("Hex"));
		Ittems->AppendL(_L("Select converter"));
		
	    CleanupStack::Pop(Ittems);
	    list->Model()->SetItemTextArray(Ittems);
		list->Model()->SetOwnershipType(ELbmOwnsItemArray);

		popupList->SetTitleL(_L("Read file as:"));
		
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
TBool CMainContainer::AskToSelectConverterL(TUint & aConverter)
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

	popupList->SetTitleL(_L("Select converter"));
	
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
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CMainContainer::DialogDismissedL (TInt /*aButtonId*/)
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
void CMainContainer::ReadprocessL(const TDesC& aStuff, TInt aReadSoFar)
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
void CMainContainer::ReadDoneL(TInt aError)
{
	if(iProgressDialog)
	{
		iProgressDialog->ProcessFinishedL(); 
	}

	iProgressDialog = NULL;
	iProgressInfo = NULL;

	if(aError != KErrNone)
	{
	// show error ?
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
	
	DrawNow();
}
/*
-----------------------------------------------------------------------

-----------------------------------------------------------------------
*/
TKeyResponse CMainContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
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
			CEikonEnv::Static()->AppUiFactory()->MenuBar()->TryDisplayMenuBarL();
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

-----------------------------------------------------------------------
*/
void CMainContainer::Draw(const TRect& /*aRect*/) const
{
	if(iBgContext)
	{
	 	CWindowGc& gc = SystemGc();
	  	MAknsSkinInstance* skin = AknsUtils::SkinInstance();
		AknsDrawUtils::Background( skin, iBgContext, this, gc, Rect() );
	}
}
/*
-------------------------------------------------------------------------

-------------------------------------------------------------------------
*/
TInt CMainContainer::CountComponentControls() const
{
	if(iMyHelpContainer)
	{
		return 1;
	}
	else if(iEditor)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
/*
-------------------------------------------------------------------------

-------------------------------------------------------------------------
*/
CCoeControl* CMainContainer::ComponentControl(TInt /*aIndex*/) const
{
	if(iMyHelpContainer)
		return iMyHelpContainer;
	else 
		return iEditor;
}

/*
-------------------------------------------------------------------------
interface function for getting the implementation instance
-------------------------------------------------------------------------
*/
CYTasksContainer* NewFileHandler()
    {
    return (new CMainContainer);
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
	IMPLEMENTATION_PROXY_ENTRY(0x312C,NewFileHandler)
};
#else

// Define the interface UIDs
const TImplementationProxy ImplementationTable[] = 
    {
    {{0x312C}, NewFileHandler}
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

