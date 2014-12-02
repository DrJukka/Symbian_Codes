/* 

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
#include <eikmenub.h> 
#include <aknglobalnote.h> 


#include "YTools_A000257A.h"
#include "YTools_A000257A_res.rsg"


#include <eikstart.h>
_LIT(KMyResourceFileName		,"\\resource\\apps\\YTools_A000257A_res.RSC");

_LIT(KtxApplicationName			,"Y-Update");
_LIT(KtxAbbout					,"version 0.87 (Testi)\nJanuary 29th 2007\nfor S60, Symbian OS 9,\n\nCopyright Jukka Silvennoinen 2006-08, All right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nwww.DrJukka.com\n");


_LIT(KtxDownloadFile			,"C:\\Jukka.txt");
_LIT(KtxDrJukkaUrl1				,"http://www.drjukka.com/Me_Echo2.php");


_LIT(KtxXMLFileName				,"\\private\\2000713D\\Update_Data\\YUpdate.txt");
_LIT(KtxIMGFileName				,"\\private\\2000713D\\Update_Data\\YUpdate.mif");

/*
---------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------
*/
CYUpdateContainer::~CYUpdateContainer()
{	
	CEikonEnv::Static()->DeleteResourceFile(iResId);

	delete iRSSFeedParser;
	iRSSFeedParser = NULL;
	delete iInfoGetter;
	iInfoGetter = NULL;
	delete iHTTPEngine;
	iHTTPEngine = NULL;
	
	delete iSmallBox;
	iSmallBox = NULL;

	delete iBgContext;
	iBgContext = NULL;
	
	REComSession::DestroyedImplementation(iDestructorIDKey);
}

/*
-------------------------------------------------------------------------
-------------------------------------------------------------------------
*/
void CYUpdateContainer::ConstructL()
{
 	CreateWindowL();
 	
 	TFileName resourceName;
 	
 	TFindFile AppFile(CCoeEnv::Static()->FsSession());
	if(KErrNone == AppFile.FindByDir(KMyResourceFileName, KNullDesC))
	{
 		resourceName.Copy(AppFile.File());
		BaflUtils::NearestLanguageFile(CEikonEnv::Static()->FsSession(), resourceName);
		iResId = CEikonEnv::Static()->AddResourceFileL(resourceName);
	}
	
	iBgContext = CAknsBasicBackgroundControlContext::NewL(KAknsIIDQsnBgAreaMain,TRect(0,0,1,1), ETrue);

	resourceName.Zero();
	
	TFindFile XMLFile(CCoeEnv::Static()->FsSession());
	if(KErrNone != XMLFile.FindByDir(KtxXMLFileName, KNullDesC))
	{
		if(KErrNone ==CCoeEnv::Static()->FsSession().PrivatePath(resourceName))
		{
			TFindFile privFile(CCoeEnv::Static()->FsSession());
			if(KErrNone == privFile.FindByDir(resourceName, KNullDesC))
			{
				TParsePtrC hjelp(privFile.File());
				iInfoXMLFile.Copy(hjelp.Drive());
				iInfoXMLFile.Append(KtxXMLFileName);
			}
		}
	}
	else
	{
		iInfoXMLFile.Copy(XMLFile.File());
	}
	
	SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect()); 	
	
 	ActivateL();
	SetMenuL();
	
	iInfoGetter = new(ELeave)CInfoGetter(*this);
	iInfoGetter->ConstructL();

	DrawNow();
	
	BaflUtils::EnsurePathExistsL(CEikonEnv::Static()->FsSession(),iInfoXMLFile);

	iRSSFeedParser = CRSSFeedParser::NewL(CEikonEnv::Static()->FsSession(),*this);
	iRSSFeedParser->ReadMainL(iInfoXMLFile);
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
CGulIcon* CYUpdateContainer::GetIconL(const TDesC& aName, TInt aIcon, TInt aMask)
{
	TInt MaskId(aMask);
	TInt UseIndex(aIcon);
	
	
	AknIconUtils::ValidateLogicalAppIconId (aName, UseIndex, MaskId);
	
	CFbsBitmap*	AppIcon(NULL);
	CFbsBitmap*	AppIconMsk(NULL);

	AknIconUtils::CreateIconL(AppIcon,AppIconMsk,aName, UseIndex, MaskId);
	 
	return CGulIcon::NewL(AppIcon,AppIconMsk);
}

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CYUpdateContainer::DisplayBoxL(TInt aSelected)
{
	TInt MyCurrent = aSelected;
	if(iSmallBox && aSelected < 0)
	{
		MyCurrent = iSmallBox->CurrentItemIndex();
	}		
	
	delete iSmallBox;
	iSmallBox = NULL;
	iSmallBox   = new( ELeave ) CAknDoubleLargeStyleListBox();//CAknSingleLargeStyleListBox();//CAknSingleNumberStyleListBox();
	iSmallBox->SetMopParent(this);
	iSmallBox->ConstructL(this,EAknListBoxSelectionList);
	
	CArrayPtr<CGulIcon>* icons =new( ELeave ) CAknIconArray(1);
	CleanupStack::PushL(icons);	
	
	TFileName tmpIconName;
	TParsePtrC hjelp(iInfoXMLFile);
	tmpIconName.Copy(hjelp.Drive());
	tmpIconName.Append(KtxIMGFileName);
	
	
	icons->AppendL(GetIconL(tmpIconName,0,1));	
	icons->AppendL(GetIconL(tmpIconName,2,3));	
	icons->AppendL(GetIconL(tmpIconName,4,5));
	icons->AppendL(GetIconL(tmpIconName,6,7));	
	CleanupStack::Pop(); //icons
	iSmallBox->ItemDrawer()->ColumnData()->SetIconArray(icons);

	CDesCArrayFlat* Array = new (ELeave) CDesCArrayFlat(1);
	CleanupStack::PushL(Array);
	
	CleanupStack::Pop(); 
	iSmallBox->Model()->SetItemTextArray(Array);

    iSmallBox->Model()->SetOwnershipType(ELbmOwnsItemArray);
	iSmallBox->CreateScrollBarFrameL( ETrue );
    iSmallBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
	iSmallBox->ItemDrawer()->ColumnData()->EnableMarqueeL(ETrue);

	TRect MyRect(CEikonEnv::Static()->EikAppUi()->ClientRect());
	iSmallBox->SetRect(TRect(0,0,MyRect.Width(),MyRect.Height()));
//	iSmallBox->SetRect(Rect());
	iSmallBox->ActivateL();

	if(iSmallBox->Model()->ItemTextArray()->MdcaCount() > MyCurrent && MyCurrent >= 0)
		iSmallBox->SetCurrentItemIndex(MyCurrent);
	else if(iSmallBox->Model()->ItemTextArray()->MdcaCount() > 0)
		iSmallBox->SetCurrentItemIndex(0);
		
	UpdateScrollBar(iSmallBox);
	iSmallBox->DrawNow();
	
	if(iRSSFeedParser)
	{
		if(iRSSFeedParser->IsDone())
		{
			ReadFinishedL(KErrNone);
		}
	}
	
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CYUpdateContainer::ReadFinishedL(TInt /*aError*/)
{	
	if(iSmallBox && iRSSFeedParser)
	{
		RPointerArray<CCategoryItems> FilArr = iRSSFeedParser->GetArray();
	
		MDesCArray* itemList = iSmallBox->Model()->ItemTextArray();
    	CDesCArray* itemArray = STATIC_CAST( CDesCArray* ,itemList );
    
		TFileName ListFil;
		
		for(TInt i=0; i < FilArr.Count(); i++)
		{
			if(FilArr[i])
			{
				if(FilArr[i]->iTitle)
				{
					ListFil.Num(FilArr[i]->iIcon);
					ListFil.Append(_L("\t"));
				
					//ListFil.Copy(_L("\t"));
					
					if(FilArr[i]->iTitle->Des().Length() > 200)
						ListFil.Append(FilArr[i]->iTitle->Des().Left(200));
					else
						ListFil.Append(FilArr[i]->iTitle->Des());
					
					ListFil.Append(_L("\t"));
					ListFil.AppendNum(FilArr[i]->iItemsArray.Count());
					ListFil.Append(_L(" items(s)"));
					
					itemArray->AppendL(ListFil);
				}
			}
		}	
		
		iSmallBox->HandleItemAdditionL();
		UpdateScrollBar(iSmallBox);
		iSmallBox->DrawNow();	
	}	
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/

TInt CYUpdateContainer::CurrentlySelectedLine(void)
{
	TInt Selected(-1);
	if(iSmallBox)
	{
		Selected = iSmallBox->CurrentItemIndex();
	}
	
	return Selected;
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CYUpdateContainer::UpdateScrollBar(CEikListBox* aListBox)
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
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CYUpdateContainer::NewsParsedL(void)
{
	ReadFinishedL(KErrNone);
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CYUpdateContainer::GotAllInfoL(void)
{
	iInfoIsReady = ETrue;
	DrawNow();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CYUpdateContainer::HTMLDataReceivedL(const TDesC8& aData, TInt aLength)
{
	
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CYUpdateContainer::ClientEvent(const TDesC& aText1,const TDesC& aText2,TInt aProgress,TInt aFinal)
{
	ClientEvent(aText1,aText2,KErrNone);
	
	if(iProgressInfo && aFinal > 0)
	{
		iProgressInfo->SetFinalValue(aFinal);
		iProgressInfo->SetAndDraw(aProgress);
	}	
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CYUpdateContainer::ClientEvent(const TDesC& aText1,const TDesC& aText2,TInt /*aError*/)
{
	if(iProgressDialog)
	{
		TBuf<200> pillu;
		pillu.Copy(aText1);
		pillu.Append(_L(": "));
		pillu.Append(aText2);
		
		iProgressDialog->SetTextL(pillu);
	}	
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CYUpdateContainer::ClientBodyReceived(const TDesC& aFileName)
{	
	TFileName SaveName(aFileName);
	
	if(iProgressDialog)
	{
		iProgressDialog->ProcessFinishedL(); 
	}

	iProgressDialog = NULL;
	iProgressInfo= NULL;
	
	ShowNoteL(_L("Got stufff"));}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CYUpdateContainer::ClientCancelled(const TDesC& aFileName,TInt aError)
{
	if(iProgressDialog)
	{
		iProgressDialog->ProcessFinishedL(); 
	}

	iProgressDialog = NULL;
	iProgressInfo= NULL;
	

	TBuf<100> ErrorMsg(_L("Cancelled"));
	if(aError == 7373)
	{
		ErrorMsg.Copy(_L("Invalid URL "));	
	}
	else if(aError != KErrCancel)
	{
		ErrorMsg.Copy(_L("Download failed with error: "));
		ErrorMsg.AppendNum(aError);
	}

	ShowNoteL(ErrorMsg);	
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CYUpdateContainer::ShowNoteL(const TDesC& aMessage)
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
void CYUpdateContainer::DialogDismissedL (TInt aButtonId)
{		  	
	iProgressDialog = NULL;
	iProgressInfo = NULL;
	
	if(iHTTPEngine && aButtonId < 0)
	{
		iHTTPEngine->CancelTransaction();
	}
} 
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CYUpdateContainer::SizeChanged()
{
	if ( iBgContext )
	{
		iBgContext->SetRect(Rect());

		if ( &Window() )
		{
			iBgContext->SetParentPos( PositionRelativeToScreen() );
		}
	}
	
	TInt Sel(-1);
	if(iSmallBox)
	{
		Sel = iSmallBox->CurrentItemIndex();
	}
	
	DisplayBoxL(Sel);	
}
/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
TTypeUid::Ptr CYUpdateContainer::MopSupplyObject(TTypeUid aId)
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
void CYUpdateContainer::SetFocusL(TBool aFocus)
{
	if(aFocus)
	{			
		CEikStatusPane*		sp = ((CAknAppUi*)iEikonEnv->EikAppUi())->StatusPane();
		if(sp)
		{	
			CAknTitlePane* TitlePane = STATIC_CAST(CAknTitlePane*,sp->ControlL(TUid::Uid(EEikStatusPaneUidTitle)));
			TitlePane->SetTextL(KtxApplicationName);		
		}
		
		SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());
		SetMenuL();
		DrawNow();	
	}
}
/*
------------------------------------------------------------------------
------------------------------------------------------------------------
*/
TBool CYUpdateContainer::GetFeedL(const TDesC& aUrl,const TDesC& aFileName, TBool aGet)
{
	TBool Ret(ETrue);
		
	iProgressDialog = new (ELeave) CAknProgressDialog((REINTERPRET_CAST(CEikDialog**,&iProgressDialog)),EFalse);
	iProgressDialog->PrepareLC(R_PROGRESS_NOTE);
	iProgressInfo = iProgressDialog->GetProgressInfoL();
	iProgressDialog->SetCallback(this);
	iProgressDialog->RunLD();
	iProgressInfo->SetFinalValue(5);
		
	if(!iHTTPEngine)
  	{
  		iHTTPEngine = CHTTPEngine::NewL(*this);
  	}
				
	
	TBuf8<255> Urlhjelp;	
	
	if(aUrl.Length() > 255)
		Urlhjelp.Copy(aUrl.Left(255));
	else
		Urlhjelp.Copy(aUrl);
	
	HBufC8* BodyBuf(NULL);
	
/*	if(aGet)
	{
*/		
		BodyBuf = iInfoGetter->InfoBufferLC(_L8("jukkas testing time password string"),ETrue);//KNullDesC8,EFalse);

	/*	_LIT(KtxtmpDbgFile, "C:\\YupTest.txt");
		CCoeEnv::Static()->FsSession().Delete(KtxtmpDbgFile);
		
		RFile YupTmpFil;	
		YupTmpFil.Create(CCoeEnv::Static()->FsSession(),KtxtmpDbgFile,EFileWrite|EFileShareAny);
		YupTmpFil.Write(*BodyBuf);
		YupTmpFil.Close();
*/
/*	}
	else
	{
		BodyBuf = iInfoGetter.InfoBufferLC(_L8(""),ETrue);
	}
*/	
/*	if(aGet)
	{
		iHTTPEngine->IssueHTTPGetL(Urlhjelp,*BodyBuf,aFileName, ETrue);		
	}
	else
*/	{
		iHTTPEngine->IssueHTTPPostL(Urlhjelp,*BodyBuf,aFileName,CHTTPEngine::EMultiPart, ETrue);
	}
	
 	CleanupStack::PopAndDestroy(BodyBuf);
 	
	return Ret;
}
/*
-------------------------------------------------------------------------
-------------------------------------------------------------------------
*/

void CYUpdateContainer::HandleCommandL(TInt aCommand)
{
	switch (aCommand)
    { 
    case ETestCommand:
    	{
    		GetFeedL(KtxDrJukkaUrl1,KtxDownloadFile, EFalse);
    	}
    	break;
    case EAbout:
    	{
    		HBufC* Abbout = KtxAbbout().AllocLC();
			TPtr Pointter(Abbout->Des());
			CAknMessageQueryDialog* dlg = CAknMessageQueryDialog::NewL(Pointter);
			dlg->PrepareLC(R_ABOUT_HEADING_PANE);
			dlg->SetHeaderTextL(KtxApplicationName);  
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
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
TKeyResponse CYUpdateContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
	TKeyResponse Ret = EKeyWasNotConsumed;
	
	switch (aKeyEvent.iCode)
    {
	case EKeyDevice3:
		CEikonEnv::Static()->AppUiFactory()->MenuBar()->TryDisplayMenuBarL();
		break;	
	case EKeyDownArrow:
	case EKeyUpArrow:
	default:
		if(iSmallBox)
		{
			Ret = iSmallBox->OfferKeyEventL(aKeyEvent,aType);
			
		//	TInt Curr = iSmallBox->CurrentItemIndex();
		}
		SetMenuL();
		break;
	}
	
	return Ret;
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CYUpdateContainer::Draw(const TRect& /*aRect*/) const
{
	CWindowGc& gc = SystemGc();
	if(iBgContext)
	{
 		MAknsSkinInstance* skin = AknsUtils::SkinInstance();
		AknsDrawUtils::Background( skin, iBgContext, this, gc, Rect() );
	}
	else
	{
 		gc.Clear(Rect());
	}
	

	const CFont* MyUseFont = AknLayoutUtils::FontFromId(EAknLogicalFontSecondaryFont);

	gc.SetPenStyle(CGraphicsContext::ESolidPen);
	gc.SetPenColor(KRgbBlack);
	gc.SetBrushStyle(CGraphicsContext::ENullBrush);
	
	TRect DrawTxtRect(10,100,Rect().Width(),(100 + MyUseFont->HeightInPixels()));
	
	gc.UseFont(MyUseFont);
	if(iInfoIsReady)
		gc.DrawText(_L("Ready !"),DrawTxtRect,MyUseFont->AscentInPixels(), CGraphicsContext::ECenter, 0);									
	else
		gc.DrawText(_L("Initializing.."),DrawTxtRect,MyUseFont->AscentInPixels(), CGraphicsContext::ECenter, 0);
}
/*
-------------------------------------------------------------------------
-------------------------------------------------------------------------
*/
TInt CYUpdateContainer::CountComponentControls() const
{
	if(iSmallBox)
		return 1;
	else
		return 0;
}
/*
-------------------------------------------------------------------------
-------------------------------------------------------------------------
*/
CCoeControl* CYUpdateContainer::ComponentControl(TInt /*aIndex*/) const
{
	return iSmallBox;
}

/*
-------------------------------------------------------------------------
-------------------------------------------------------------------------
*/
void CYUpdateContainer::SetMenuL(void)
{
	CEikonEnv::Static()->AppUiFactory()->MenuBar()->SetMenuTitleResourceId(R_MAIN_MENUBAR);	
		
	iFileHandlerUtils->GetCba().SetCommandSetL(R_MAIN_CBA);				
	iFileHandlerUtils->GetCba().DrawDeferred();
}
/*
-------------------------------------------------------------------------
-------------------------------------------------------------------------
*/
void CYUpdateContainer::DynInitMenuPaneL(TInt /*aResourceId*/, CEikMenuPane* /*aMenuPane*/)
{

}	
/*
-------------------------------------------------------------------------
*************************************************************************
-------------------------------------------------------------------------
*/
CYBrowserFileHandler* NewFileHandler()
    {
    return (new CYUpdateContainer);
    }
/*
-------------------------------------------------------------------------
-------------------------------------------------------------------------
*/

LOCAL_D const TImplementationProxy ImplementationTable[] = 
{
	IMPLEMENTATION_PROXY_ENTRY(0x257A,NewFileHandler)
};


/*
-------------------------------------------------------------------------
-------------------------------------------------------------------------
*/
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
{
	aTableCount = sizeof(ImplementationTable)/sizeof(ImplementationTable[0]);
	return ImplementationTable;
}

