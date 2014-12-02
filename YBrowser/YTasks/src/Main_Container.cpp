/* 	
	Copyright (c) 2001 - 2007, 
	J.P. Silvennoinen.
	All rights reserved 
*/

#include <barsread.h>
#include <CHARCONV.H>
#include <EIKSPANE.H>
#include <EIKBTGPC.H>
#include <eikmenub.h>
#include <bautils.h>
#include <EIKPROGI.H>
#include <APGCLI.H>
#include <eikclbd.h>
#include <EIKFUTIL.H>
#include <akntitle.h> 
#include <ecom.h>
#include <implementationproxy.h>
#include <AknsUtils.h>
#include <AknsDrawUtils.h>
#include <akncontext.h> 
#include <aknmessagequerydialog.h>
#include <AknGlobalNote.h>
#include <stringloader.h> 
#include <aknquerydialog.h> 
#include <AknIconArray.h>
#include <aknnavi.h> 
#include <aknnavide.h> 
#include <akntabgrp.h> 
#include <aknlists.h> 
#include <akniconarray.h> 
#include <AknQueryDialog.h>


#include "Ecom_Handler.h"
#include "Main_Container.h"
#include "YTools_A000955F.hrh"


#ifdef __YTOOLS_SIGNED
	#include "YTools_A000955F_res.rsg"
#else
	#include "YTools_A000955F_res.rsg"
#endif

_LIT(KtxAbbout				,"version 0.90\nSeptember 20th 2008\nfor S60, Symbian OS 9,\n\nCopyright Jukka Silvennoinen 2006-2008, All right reserved.\n\nemail:\nsymbianyucca@gmail.com\nHomepage:\nwww.DrJukka.com\n");

#ifdef __YTOOLS_SIGNED
	_LIT(KMyResourceFileName	,"\\resource\\apps\\YTools_A000955F_res.RSC");
#else
	_LIT(KMyResourceFileName	,"\\resource\\apps\\YTools_A000955F_res.RSC");
#endif

_LIT(KtxIconFileName			,"\\system\\data\\A00007A6\\A000955F\\icons.mbm");


const TInt KSplashTimeOut		 = 1500000;


_LIT(KTxtYTasks					,"Y-Tasks");

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::SetUtils(MYBrowserFileHandlerUtils* aFileHandlerUtils)
{
	iFileHandlerUtils = aFileHandlerUtils;
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CMainContainer::~CMainContainer()
{  		
	if(iTimeOutTimer)
		iTimeOutTimer->Cancel();
	
	delete iTimeOutTimer;
	iTimeOutTimer = NULL;
	
	delete iYTasksContainer;
	iYTasksContainer = NULL;

	delete iEcomHandler;
    delete iProfileBox;
    
    if(iResId > 0)
	{
		CEikonEnv::Static()->DeleteResourceFile(iResId);
	}
	
	REComSession::DestroyedImplementation(iDestructorIDKey);
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::ConstructL()
{
	CreateWindowL();	

 	iResId = -1;
 	TFindFile AppFile(CCoeEnv::Static()->FsSession());
	if(KErrNone == AppFile.FindByDir(KMyResourceFileName, KNullDesC))
	{
 		TFileName resourceName(AppFile.File());
		BaflUtils::NearestLanguageFile(CEikonEnv::Static()->FsSession(), resourceName);
		iResId = CEikonEnv::Static()->AddResourceFileL(resourceName);
	}
	
	iEcomHandler = CEcomHandler::NewL(this);

	iTimeOutTimer = CTimeOutTimer::NewL(CActive::EPriorityStandard,*this);
	
	SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());

	ActivateL();
	SetMenuL();
	DrawNow();

}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::SizeChanged()
{
	MakeGridBoxL();	
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/

void CMainContainer::HandleResourceChange(TInt aType)
{
	if(iYTasksContainer)
 	{
 		iYTasksContainer->HandleResourceChange(aType);
 	}
 		
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
-------------------------------------------------------------------------
Y-Browser will call this when focus changes.

i.e. When you ask y-browser to open a file, and it will be opened by
other y-browser handler, then you will be called with focus false, 
right before new handler takes focus.

And when it returns back to y-browser, this function will be called with
ETrue,
-------------------------------------------------------------------------
*/
void CMainContainer::SetFocusL(TBool aFocus)
{
	if(aFocus)
	{
		CEikStatusPane*		sp = ((CAknAppUi*)iEikonEnv->EikAppUi())->StatusPane();
		if(sp)
		{
			CAknTitlePane* TitlePane = STATIC_CAST(CAknTitlePane*,sp->ControlL(TUid::Uid(EEikStatusPaneUidTitle)));
			TitlePane->SetTextL(KTxtYTasks);					
		}		
		
		SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());
		SetMenuL();
		DrawNow();	
	}
	
	if(iYTasksContainer)
	{
		iYTasksContainer->ForegroundChangedL(aFocus);
	}
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/

void CMainContainer::MakeGridBoxL()
{
	TInt MySetIndex(0);
	
	if(iProfileBox)
	{
		MySetIndex = iProfileBox->CurrentItemIndex();
	}

	delete iProfileBox;
	iProfileBox = NULL;

	 // create a grid
    iProfileBox = new( ELeave ) CAknGrid;
    
    CAknGrid* MyGrid = STATIC_CAST(CAknGrid*, iProfileBox);
    
    MyGrid->SetContainerWindowL( *this );

    CAknGridM* GridViewM = new( ELeave ) CAknGridM;
    MyGrid->SetModel( GridViewM );

	MyGrid->ConstructL( this, EAknListBoxSelectionGrid );

	TRect MyRect(Rect());	
	MyGrid->SetRect(MyRect);

	TInt NumH = (MyRect.Height() / 80);
	TInt NumW = (MyRect.Width() / 80);

	if(NumH < 3)
	{
		NumH = 3;
	}
	
	if(NumW < 3)
	{
		NumW = 3;
	}

	TSize SizeOfItems;

	SizeOfItems.iWidth = ( MyRect.iBr.iX - MyRect.iTl.iX ) / NumW;
	SizeOfItems.iHeight = ( MyRect.iBr.iY - MyRect.iTl.iY )/ NumH;
 
	MyGrid->SetLayoutL(EFalse,ETrue,ETrue,NumW,NumH,SizeOfItems);

	MyGrid->SetPrimaryScrollingType (CAknGridView::EScrollIncrementLineAndLoops);
	MyGrid->SetSecondaryScrollingType(CAknGridView::EScrollIncrementLineAndLoops);
	MyGrid->SetCurrentDataIndex( 0 );
    
	CArrayPtr<CGulIcon>* icons =new( ELeave ) CAknIconArray(2);
	CleanupStack::PushL(icons);

	const CFont* MyUseFont = AknLayoutUtils::FontFromId(EAknLogicalFontSecondaryFont);
	
	TPoint TxtStartPoint(0,(SizeOfItems.iHeight - (MyUseFont->HeightInPixels() + 3)));
	TPoint TxtEndddPoint(SizeOfItems.iWidth,SizeOfItems.iHeight);

	TPoint StartIcon(0,0);
	TPoint EnddIcon(SizeOfItems.iWidth,TxtStartPoint.iY);
	
	TSize IconsSize((((EnddIcon.iX - StartIcon.iX) * 9) / 10),(((EnddIcon.iY - StartIcon.iY) * 9) / 10));

	MyGrid->Model()->SetItemTextArray(GetProfilesArrayL(icons,IconsSize));
	MyGrid->ItemDrawer()->FormattedCellData()->SetIconArrayL(icons);
	CleanupStack::Pop();//icons

	MyGrid->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EAuto,CEikScrollBarFrame::EAuto);
    
	MyGrid->HandleItemAdditionL();

	AknListBoxLayouts::SetupStandardGrid( *MyGrid);		

	AknListBoxLayouts::SetupFormGfxCell( *MyGrid, MyGrid->ItemDrawer(),
	0, 0, 0, 0, 0,  SizeOfItems.iWidth,(EnddIcon.iY - StartIcon.iY), 
	StartIcon, EnddIcon);

	TInt BaseLine = (TxtEndddPoint.iY - MyUseFont->BaselineOffsetInPixels() - 3);

	AknListBoxLayouts::SetupFormTextCell( *MyGrid, MyGrid->ItemDrawer(),
				1,  MyUseFont, 
				215, 
				3, 
				0, 
				BaseLine,
				SizeOfItems.iWidth, 
				CGraphicsContext::ECenter,
				TxtStartPoint, 
				TxtEndddPoint
				);


	TInt ItemsCount = MyGrid->Model()->ItemTextArray()->MdcaCount();
	
	if(ItemsCount > MySetIndex && MySetIndex >= 0)
		MyGrid->SetCurrentItemIndex(MySetIndex);
	else if(ItemsCount > 0)
		MyGrid->SetCurrentItemIndex(0);
	
    MyGrid->MakeVisible( ETrue );
    MyGrid->SetFocus( ETrue );
    MyGrid->ActivateL();
    MyGrid->DrawNow();
}

/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
CDesCArray* CMainContainer::GetProfilesArrayL(CArrayPtr<CGulIcon>* aIcon,const TSize& aIconsize)
{
	aIcon->AppendL(GetMyOwnIconL(aIconsize));
	
	TFindFile MyIconFile(CCoeEnv::Static()->FsSession());
	if(KErrNone == MyIconFile.FindByDir(KtxIconFileName, KNullDesC))
	{
		AddIconL(aIcon,aIconsize,MyIconFile.File(),0,1);
	}
	else
	{
		aIcon->AppendL(GetMyOwnIconL(aIconsize));
	}
	
	
	CDesCArrayFlat* MyArray = new(ELeave)CDesCArrayFlat(1);
	CleanupStack::PushL(MyArray);
	
	TFileName AddBuffer;			
	
	if(iEcomHandler)
	{
		RPointerArray<CHandlerItem> Handlers = iEcomHandler->HandlerArray();
		for(TInt i=0; i < Handlers.Count(); i++)
		{
			if(Handlers[i])
			{
				TInt IconNum(0);
				
				if(Handlers[i]->iIconfile)
				{
					TFindFile IconFile(CCoeEnv::Static()->FsSession());
					if(KErrNone == IconFile.FindByDir(*Handlers[i]->iIconfile, KNullDesC))
					{
						TInt Img(0),ImgMask(1);
						
						if(Handlers[i]->iServerName)
						{
							if(IsServerOn(*Handlers[i]->iServerName))
							{
								Img = 2;
								ImgMask = 3;
							}
						}
						
						AddIconL(aIcon,aIconsize,IconFile.File(),Img,ImgMask);
					
						IconNum = (aIcon->Count() - 1);
					}
				}
				
				AddBuffer.Num(IconNum);
				AddBuffer.Append(_L("\t"));
				if(Handlers[i]->iName)
				{
					AddBuffer.Append(*Handlers[i]->iName);
				}
				
				MyArray->AppendL(AddBuffer);			
			}
		}
	}
	
	CleanupStack::Pop(MyArray);
	return MyArray;
}	

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
TBool CMainContainer::IsServerOn(const TDesC& aName)
{
	TBool Ret(EFalse);
	
	TFileName res;
	TFindProcess find;
  	while(find.Next(res) == KErrNone)
    {
    	RProcess ph;
	  	ph.Open(res);
	  	  			
		if(ph.FileName().Find(aName) != KErrNotFound)
	 	{
	  		Ret = ETrue;
	  		break;
	  	}
	  	
	  	ph.Close();
    }
 
    return Ret;
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
CGulIcon* CMainContainer::GetMyOwnIconL(const TSize& aSize)
{
	MAknsSkinInstance* skin = AknsUtils::SkinInstance();
  
	CFbsBitmap* Img;
	CFbsBitmap* Msk;
		// fix the UId later..
    AknsUtils::CreateAppIconLC(skin,TUid::Uid(0x100),  EAknsAppIconTypeContext,Img,Msk);

	if(aSize.iWidth && aSize.iHeight)
	{
		AknIconUtils::SetSize(Img,aSize);  
		AknIconUtils::SetSize(Msk,aSize);  
	}
	
	CleanupStack::Pop(2);
	
	return CGulIcon::NewL(Img,Msk);
}
       
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::AddIconL(CArrayPtr<CGulIcon>* aIcon,const TSize& aIconsize,const TDesC& aFileName, TInt aImg, TInt aMsk)
{
	CFbsBitmap* FrameImg(NULL);
	CFbsBitmap* FrameMsk(NULL);
	
	TInt BgIndex(aImg),BgMask(aMsk);
	
	if(AknIconUtils::IsMifFile(aFileName))
	{
		AknIconUtils::ValidateLogicalAppIconId(aFileName, BgIndex,BgMask); 
	}
	
	AknIconUtils::CreateIconL(FrameImg, FrameMsk, aFileName,BgIndex,BgMask);
	
	if(aIconsize.iHeight > 0 && aIconsize.iWidth > 0)
	{
		AknIconUtils::SetSize(FrameImg,aIconsize,EAspectRatioPreserved);  
		AknIconUtils::SetSize(FrameMsk,aIconsize,EAspectRatioPreserved); 
	}


	aIcon->AppendL(CGulIcon::NewL(FrameImg,FrameMsk));
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::Draw(const TRect& /*aRect*/) const
{
	CWindowGc& gc = SystemGc();
	gc.Clear(Rect());
}

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
TKeyResponse CMainContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aEventCode)
{
	TKeyResponse Ret = EKeyWasNotConsumed;	

	if(iYTasksContainer)
	{
		Ret = iYTasksContainer->OfferKeyEventL(aKeyEvent,aEventCode);
	}
	else
	{
		switch (aKeyEvent.iCode)
	    {
	    case EKeyDevice3:
			HandleCommandL(EAppOpen);
			break;
		case EKeyDownArrow:
		case EKeyUpArrow:
		default:
			if(iProfileBox)
			{
				Ret = iProfileBox->OfferKeyEventL(aKeyEvent,aEventCode);
			}
			break;
		}
	}
	
	return Ret;
}
/*
-------------------------------------------------------------------------
we only support one file type, so we don't care what aFileTypeID is
-------------------------------------------------------------------------
*/
void CMainContainer::OpenFileL(const TDesC& /*aFileName*/,const TDesC& /*aFileTypeID*/)	
{
	HandleCommandL(EBacktoYBrowser);
}
/*
-------------------------------------------------------------------------
we only support one file type, so we don't care what aFileTypeID is
-------------------------------------------------------------------------
*/
void CMainContainer::OpenFileL(RFile& /*aOpenFile*/,const TDesC& /*aFileTypeID*/)
{
	HandleCommandL(EBacktoYBrowser);
}

/*
-------------------------------------------------------------------------
we only support one filetype, so no need to wonder what aFileTypeID is

-------------------------------------------------------------------------
*/
void CMainContainer::NewFileL(const TDesC& /*aPath*/,const TDesC& /*aFileTypeID*/)
{
	HandleCommandL(EBacktoYBrowser);
}
void CMainContainer::NewFileL(const TDesC& /*aPath*/,const TDesC& /*aFileTypeID*/,MDesCArray& /*aFileArray*/)
{
	HandleCommandL(EBacktoYBrowser);
}
/*
-------------------------------------------------------------------------
we are not supporting adding files to existing zip files
so no need to implement this function
-------------------------------------------------------------------------
*/
void CMainContainer::AddFilesL(const TDesC& /*aFileName*/,MDesCArray& /*aFileArray*/)
{	
	HandleCommandL(EBacktoYBrowser);
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::HandlerExitL(CYTasksContainer* /*aHandler*/)
{
	delete iYTasksContainer;
	iYTasksContainer = NULL;
	
	CEikStatusPane*		sp = ((CAknAppUi*)iEikonEnv->EikAppUi())->StatusPane();
	if(sp)
	{
		CAknTitlePane* TitlePane = STATIC_CAST(CAknTitlePane*,sp->ControlL(TUid::Uid(EEikStatusPaneUidTitle)));
		TitlePane->SetTextToDefaultL();

		CAknContextPane* 	ContextPane = (CAknContextPane *)sp->ControlL(TUid::Uid(EEikStatusPaneUidContext));
		if(ContextPane)
		{
			ContextPane->SetPictureToDefaultL();
		}

		if(sp->CurrentLayoutResId() != R_AVKON_STATUS_PANE_LAYOUT_USUAL)
		{
			sp->SwitchLayoutL(R_AVKON_STATUS_PANE_LAYOUT_USUAL);
		}
		
		sp->DrawNow();
	}
	
	if(iTimeOutTimer)
	{
		iTimeOutTimer->After(KSplashTimeOut);
	}
	else
	{
		MakeGridBoxL();	
	}
	
	SetMenuL();
	DrawNow();	
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::TimerExpired(void)
{
	MakeGridBoxL();	
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::ShowNoteL(const TDesC& aMessage)
{
	CAknGlobalNote* dialog = CAknGlobalNote::NewLC();
	TInt NoteId = dialog->ShowNoteL(EAknGlobalInformationNote,aMessage);
	User::After(2000000);
	dialog->CancelNoteL(NoteId);
	CleanupStack::PopAndDestroy(dialog);;
}

 
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/

void CMainContainer::HandleCommandL(TInt aCommand)
{
	switch(aCommand)
	{
	case EAbout:
		{       		                     
			HBufC* Abbout = KtxAbbout().AllocLC();
			TPtr Pointter(Abbout->Des());
			CAknMessageQueryDialog* dlg = CAknMessageQueryDialog::NewL(Pointter);
			dlg->PrepareLC(R_ABOUT_HEADING_PANE);
			dlg->SetHeaderTextL(KTxtYTasks);  
			dlg->RunLD();
			
			CleanupStack::PopAndDestroy(Abbout);
		}
		break;
	case EAppOpen:
		if(!iYTasksContainer && iEcomHandler && iProfileBox)
		{
			RPointerArray<CHandlerItem> Handlers = iEcomHandler->HandlerArray();
		
			TInt CurrInd = iProfileBox->CurrentItemIndex();
			if(CurrInd >= 0 && CurrInd < Handlers.Count())
			{
				if(Handlers[CurrInd])
				{
					iYTasksContainer = iEcomHandler->GetHandlerL(*Handlers[CurrInd],&iFileHandlerUtils->GetCba());
				}
			}
		}
		SetMenuL();
		DrawNow();
		break;
	case EBacktoYBrowser:
    	if(iFileHandlerUtils)
    	{
    		iFileHandlerUtils->HandleExitL(this,MYBrowserFileHandlerUtils::EChangesMade);	
    	}
    	break;
	default:
		if(iYTasksContainer)
		{
			iYTasksContainer->HandleCommandL(aCommand);
		}
		break;
	}	
}

	
			
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CCoeControl* CMainContainer::ComponentControl( TInt aIndex) const
{
	if(iYTasksContainer)
	{
		return iYTasksContainer->ComponentControl(aIndex);	
	}
	else
	{
		return iProfileBox;
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TInt CMainContainer::CountComponentControls() const
{	
	if(iYTasksContainer)
	{
		return iYTasksContainer->CountComponentControls();
	}
	else if(iProfileBox)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
   

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::SetMenuL(void)
{
	CEikonEnv::Static()->AppUiFactory()->MenuBar()->StopDisplayingMenuBar();
	
	if(iYTasksContainer)
	{
		iYTasksContainer->SetMenuL();
	}
	else
	{
		CEikonEnv::Static()->AppUiFactory()->MenuBar()->SetMenuTitleResourceId(R_TAKS_MAIN_MENUBAR);
	
		iFileHandlerUtils->GetCba().SetCommandSetL(R_TAKS_MAIN_CBA);
		iFileHandlerUtils->GetCba().DrawDeferred();
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane)
{
	if(aResourceId == R_TAKS_MAIN_MENU)
	{
		TBool Dimopen(ETrue);
		
		if(iProfileBox)
		{
			TInt CurrInd = iProfileBox->CurrentItemIndex();
			if(CurrInd >= 0)
			{
				Dimopen = EFalse;		
			}
		}
		
		if(Dimopen)
		{
			aMenuPane->SetItemDimmed(EAppOpen,ETrue);
		}
	}
	
	if(iYTasksContainer)
	{
		iYTasksContainer->InitMenuPanelL(aResourceId, aMenuPane);
	}
}
/*
-------------------------------------------------------------------------
interface function for getting the implementation instance
-------------------------------------------------------------------------
*/
CYBrowserFileHandler* NewFileHandler()
    {
    return (new CMainContainer);
    }

/*
-------------------------------------------------------------------------
ECom ImplementationTable function

See SDK documentations for more information
-------------------------------------------------------------------------
*/

LOCAL_D const TImplementationProxy ImplementationTable[] = 
{
	IMPLEMENTATION_PROXY_ENTRY(0x955F,NewFileHandler)
};


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


