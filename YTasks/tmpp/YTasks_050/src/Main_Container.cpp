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


#ifdef __SERIES60_3X__
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

	#include "UpdateContainer.h"
	#include "ObjectExchangeClient.h"
	#include "InboxReader.h"
#else

#endif


#include "YTools_A000257B.h" 

#include "Ecom_Handler.h"
#include "Main_Container.h"
#include "YTools_A000257B.h"
#include "YTools_A000257B.hrh"


#include <YTools_A000257B.rsg>

_LIT(KtxIconFileName		,"\\resource\\apps\\YTools_A000257B.mif");

_LIT(KtxBtSendText1			,"You can now install Y-Tasks from another phones inbox\nNOTE: to allow sending Y-Tasks please run Y-Tasks before removing the installation file from the inbox\n\n If you are having prolems on the instalaltion, check that the time settings (year, month & day) are correctly set and that installation of \"All\" is enabled from the Application managers settings\n");
_LIT(KtxBtSendText2			,"Y-SuperDistribution was disabled, only a link to Y-Tasks homepage was sent. Please download Y-Tasks using the link.\n");

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CMainContainer::CMainContainer(CEikButtonGroupContainer* aCba)
:iCba(aCba),iHasGrid(ETrue)
{		

}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CMainContainer::~CMainContainer()
{  	
#ifdef __SERIES60_3X__
	if(iObjectExchangeClient)
	{
		iObjectExchangeClient->StopL();
	    iObjectExchangeClient->DisconnectL();
	}
	
	if(iProgressDialog)
	{
		iProgressDialog->ProcessFinishedL(); 
	}

	iProgressDialog = NULL;
	
	delete iMailBoxReader;
	iMailBoxReader = NULL;
		
	delete iObjectExchangeClient;
	iObjectExchangeClient = NULL;

	delete iUpdateContainer;
	iUpdateContainer = NULL;
	
	delete iInfoGetter;
	iInfoGetter = NULL;
#else
//#endif
#endif	
	delete iYTasksContainer;
	iYTasksContainer = NULL;

	delete iEcomHandler;
    delete iProfileBox;
    
    iMainServerClient.Close();
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::ConstructL()
{
	CreateWindowL();	

#ifdef __SERIES60_3X__
	//	iMainServerClient.Connect();
	#ifdef __WINS__

	#else
//		iInfoGetter = new(ELeave)CInfoGetter(*this);
//		iInfoGetter->ConstructL();
	#endif
#else
//#endif
#endif
	iEcomHandler = CEcomHandler::NewL(this);

	SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());

	ActivateL();
	SetMenuL();
	DrawNow();
#ifdef __WINS__

#else	
//	iMailBoxReader = new(ELeave)CMailBoxReader();
//	iMailBoxReader->ConstructL();
#endif
}
/*
---------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------
*/
void CMainContainer::GotAllInfoL(void)
{
	
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::SizeChanged()
{
#ifdef __SERIES60_3X__
	if(!iHasGrid)
	{
		MakeListBoxL();
	}
	else
	{
		MakeGridBoxL();	
	}
#else
	MakeListBoxL();
#endif
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
#ifdef __SERIES60_3X__

void CMainContainer::HandleResourceChange(TInt aType)
{
	if(iYTasksContainer)
 	{
 		iYTasksContainer->HandleResourceChange(aType);
 	}
 		
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
 		if(iUpdateContainer)
 		{
 			iUpdateContainer->SetRect(rect);
 		}
 	   
	    SetRect(rect);
 	}
 	
	CCoeControl::HandleResourceChange(aType);
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

	iHasGrid = ETrue;
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

#ifdef __SERIES60_3X__
	const CFont* MyUseFont = AknLayoutUtils::FontFromId(EAknLogicalFontSecondaryFont);
#else
	const CFont* MyUseFont = LatinBold13();
#endif
	
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
#endif
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CMainContainer::MakeListBoxL()
{
	TInt MySetIndex(0);
	
	if(iProfileBox)
	{
		MySetIndex = iProfileBox->CurrentItemIndex();
	}
    
	delete iProfileBox;
	iProfileBox = NULL;

	iHasGrid = EFalse;
	
    iProfileBox   = new( ELeave ) CAknSingleLargeStyleListBox();
    
    CAknSingleLargeStyleListBox* MyBox = STATIC_CAST(CAknSingleLargeStyleListBox*,iProfileBox);
    
	MyBox->ConstructL(this,EAknListBoxSelectionList);

	CArrayPtr<CGulIcon>* icons =new( ELeave ) CAknIconArray(1);
	CleanupStack::PushL(icons);
	
	MyBox->Model()->SetItemTextArray(GetProfilesArrayL(icons,TSize(0,0)));
   	MyBox->Model()->SetOwnershipType(ELbmOwnsItemArray);
    
    CleanupStack::Pop(icons);
	MyBox->ItemDrawer()->ColumnData()->SetIconArray(icons);
	
	MyBox->CreateScrollBarFrameL( ETrue );
    MyBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
	MyBox->SetRect(Rect());
		
	MyBox->ActivateL();

	TInt ItemsCount = MyBox->Model()->ItemTextArray()->MdcaCount();
	
	if(ItemsCount > MySetIndex && MySetIndex >= 0)
		MyBox->SetCurrentItemIndex(MySetIndex);
	else if(ItemsCount > 0)
		MyBox->SetCurrentItemIndex(0);
		
	UpdateScrollBar(MyBox);
	MyBox->DrawNow();
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
	
	TFileName AddBuffer;//(_L("1\tY-Update"));
	
//	MyArray->AppendL(AddBuffer);			
	
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
	  	  			
//	#ifdef __SERIES60_3X__	
//	  	if(ph.SecureId() == )
//	#else
		if(ph.FileName().Find(aName) != KErrNotFound)
//	#endif
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
		
    AknsUtils::CreateAppIconLC(skin,KUidTOneViewer,  EAknsAppIconTypeContext,Img,Msk);

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
	
#ifdef __SERIES60_3X__

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
#else
	FrameImg = new(ELeave)CFbsBitmap();
	TInt Err = FrameImg->Load(aFileName,aImg);
	if(Err == KErrNone)
	{
		FrameMsk = new(ELeave)CFbsBitmap();
		TInt FrErr = FrameMsk->Load(aFileName,aMsk);
		if(FrErr != KErrNone)
		{
			delete FrameMsk;
			FrameMsk = NULL;
		}
	}
	else
	{
		delete FrameImg;
		FrameImg = NULL;
	}
	
#endif

	aIcon->AppendL(CGulIcon::NewL(FrameImg,FrameMsk));
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::UpdateScrollBar(CEikListBox* aListBox)
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

	if(iUpdateContainer)
	{
		Ret = iUpdateContainer->OfferKeyEventL(aKeyEvent,aEventCode);
	}
	else if(iYTasksContainer)
	{
		Ret = iYTasksContainer->OfferKeyEventL(aKeyEvent,aEventCode);
	}
	else
	{
		switch (aKeyEvent.iCode)
	    {
	    case EKeyDevice3:
	    	//CEikonEnv::Static()->AppUiFactory()->MenuBar()->TryDisplayMenuBarL();
			HandleViewCommandL(EAppOpen);
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
---------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------
*/
void CMainContainer::ForegroundChangedL(TBool aForeground)
{
	if(iYTasksContainer)
	{
		iYTasksContainer->ForegroundChangedL(aForeground);
	}
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

/*		if(sp->CurrentLayoutResId() != R_AVKON_STATUS_PANE_LAYOUT_USUAL)
		{
			sp->SwitchLayoutL(R_AVKON_STATUS_PANE_LAYOUT_USUAL);
		}
		
		sp->DrawNow();*/
	}
	
	if(!iHasGrid)
	{
		MakeListBoxL();
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
void CMainContainer::LogL( const TDesC& aText)
{
	if(iProgressDialog)
	{
		iProgressDialog->SetTextL(aText);
	}
}
 /*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::LogL( const TDesC& aText, const TDesC& /*aExtraText*/)
{
	if(iProgressDialog)
	{
		iProgressDialog->SetTextL(aText);
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::LogL( const TDesC& aText, TInt aNumber)
{
	if(iProgressDialog)
	{
		TBuf<255> Hjelpper;
		Hjelpper.Copy(aText);
		Hjelpper.AppendNum(aNumber);
		
		iProgressDialog->SetTextL(Hjelpper);
	}
}  
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/

void CMainContainer::DoneL(TInt aError,TBool aRealSis)
{
	if(iProgressDialog)
	{
		iProgressDialog->ProcessFinishedL(); 
	}

	iProgressDialog = NULL;
	
	if(aError == KErrNone)
	{
		HBufC* Abbout(NULL); 
		
		if(aRealSis)
		{
			Abbout = KtxBtSendText1().AllocLC();
		}
		else
		{
			Abbout = KtxBtSendText2().AllocLC();
		}
		
		TPtr Pointter(Abbout->Des());
			
		CAknMessageQueryDialog* dlg = CAknMessageQueryDialog::NewL(Pointter);
		dlg->PrepareLC(R_ABOUT_HEADING_PANE);
		dlg->SetHeaderTextL(_L("Y-Superdistribution"));  
		dlg->RunLD();
				
		CleanupStack::PopAndDestroy(1);//,Abbout
	}
	if(iObjectExchangeClient)
	{
		iObjectExchangeClient->StopL();
	    iObjectExchangeClient->DisconnectL();
	}
	
	delete iObjectExchangeClient;
	iObjectExchangeClient = NULL;
}

 /*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CMainContainer::DialogDismissedL (TInt /*aButtonId*/)
{		  	
	iProgressDialog = NULL;
	
	
	if(iObjectExchangeClient)
	{
		iObjectExchangeClient->StopL();
	    iObjectExchangeClient->DisconnectL();
	}

	delete iObjectExchangeClient;
	iObjectExchangeClient = NULL;
} 
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/

void CMainContainer::HandleViewCommandL(TInt aCommand)
{
	switch(aCommand)
	{
	case ESendSisFile:
		{
			delete iObjectExchangeClient;
			iObjectExchangeClient = NULL;
			iObjectExchangeClient = CObjectExchangeClient::NewL( *this );
			iObjectExchangeClient->ConnectL();
		
			iProgressDialog = new (ELeave) CAknWaitDialog((REINTERPRET_CAST(CEikDialog**,&iProgressDialog)), ETrue);
			iProgressDialog->PrepareLC(R_WAIT_NOTE_SOFTKEY_CANCEL);
			iProgressDialog->SetCallback(this);
			iProgressDialog->RunLD();
		}
		break;
	case EUpdateBack:
		{
			delete iUpdateContainer;
			iUpdateContainer = NULL;
		}
		SetMenuL();
		DrawNow();
		break;
	case EListGrid:
		if(iHasGrid)
		{
			iHasGrid = EFalse;
			MakeListBoxL();
		}
		else
		{
			iHasGrid = ETrue;
			MakeGridBoxL();	
		}
		SetMenuL();
		DrawNow();
		break;
	case EAppOpen:
		if(!iYTasksContainer && iEcomHandler && iProfileBox)
		{
			RPointerArray<CHandlerItem> Handlers = iEcomHandler->HandlerArray();
		
			TInt CurrInd = iProfileBox->CurrentItemIndex();
/*			if(CurrInd == 0)
			{
				delete iUpdateContainer;
				iUpdateContainer = NULL;
				iUpdateContainer = new(ELeave)CUpdateContainer(iCba,*iInfoGetter);
    			iUpdateContainer->ConstructL();
			}
			else if(CurrInd > 0 && CurrInd <= Handlers.Count())	
		*/	if(CurrInd >= 0 && CurrInd < Handlers.Count())
			{
		//		CurrInd = CurrInd - 1;
				
				if(Handlers[CurrInd])
				{
					iYTasksContainer = iEcomHandler->GetHandlerL(*Handlers[CurrInd],iCba);
				}
			}
		}
		SetMenuL();
		DrawNow();
		break;
	default:
		if(iUpdateContainer)
		{
			iUpdateContainer->HandleViewCommandL(aCommand);
		}
		else if(iYTasksContainer)
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
	if(iUpdateContainer)
	{
		return iUpdateContainer->ComponentControl(aIndex);	
	}
	else if(iYTasksContainer)
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
	if(iUpdateContainer)
	{
		return iUpdateContainer->CountComponentControls();	
	}
	else if(iYTasksContainer)
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
	if(iCba)
	{
		if(iUpdateContainer)
		{
			iUpdateContainer->SetMenuL();
		}
		else if(iYTasksContainer)
		{
			iYTasksContainer->SetMenuL();
		}
		else
		{
			CEikonEnv::Static()->AppUiFactory()->MenuBar()->SetMenuTitleResourceId(R_MAIN_MENUBAR);
		
			iCba->SetCommandSetL(R_MAIN_CBA);
			iCba->DrawDeferred();
		}
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
		TBool Dimopen(ETrue);
		
		if(iProfileBox)
		{
			TInt CurrInd = iProfileBox->CurrentItemIndex();
			if(CurrInd >= 0)
			{
				Dimopen = EFalse;		
			}
		}
		
		aMenuPane->SetItemDimmed(ESendSisFile,ETrue);
		
		if(Dimopen)
		{
			aMenuPane->SetItemDimmed(EAppOpen,ETrue);
		}
	}
	
	if(iUpdateContainer)
	{
		iUpdateContainer->InitMenuPanelL(aResourceId, aMenuPane);
	}
	else if(iYTasksContainer)
	{
		iYTasksContainer->InitMenuPanelL(aResourceId, aMenuPane);
	}
}



