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

#include "YTools_A000257B.h" 

#include "Ecom_Handler.h"
#include "Main_Container.h"
#include "YTools_A000257B.h"
#include "YTools_A000257B.hrh"
#include "Help_Container.h"

#include "Common.h"

#ifdef SONE_VERSION
	#include <YTools_2002B09F.rsg>
#else
	#ifdef LAL_VERSION
		#include <YTools_A000257B.rsg>
	#else
		#include <YTools_A000257B.rsg>
	#endif
#endif



_LIT(KtxBtSendText1			,"You can now install Y-Tasks from another phones inbox\nNOTE: to allow sending Y-Tasks please run Y-Tasks before removing the installation file from the inbox\n\n If you are having prolems on the instalaltion, check that the time settings (year, month & day) are correctly set and that installation of \"All\" is enabled from the Application managers settings\n");
_LIT(KtxBtSendText2			,"Y-SuperDistribution was disabled, only a link to Y-Tasks homepage was sent. Please download Y-Tasks using the link.\n");

const TInt KSplashTimeOut		 = 1500000;

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CMainContainer::CMainContainer(CEikButtonGroupContainer* aCba)
:iCba(aCba)
{		

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
	
	delete iMyHelpContainer;
	iMyHelpContainer = NULL;
	    
	delete iYTasksContainer;
	iYTasksContainer = NULL;

	delete iEcomHandler;
    delete iProfileBox;
    delete iMyListBox;
    
    iMainServerClient.Close();
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::ConstructL()
{
	CreateWindowL();	

	iEcomHandler = CEcomHandler::NewL(this);

	iTimeOutTimer = CTimeOutTimer::NewL(CActive::EPriorityStandard,*this);
	
	GetSettingsL();
	
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
	if(iGridIsOn)
		MakeGridBoxL();	
	else
		MakeListBoxL();
	
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
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::GetSettingsL(void)
{	
	iGridIsOn = EFalse;
	
	TFindFile AufFolder(CCoeEnv::Static()->FsSession());
	if(KErrNone == AufFolder.FindByDir(KtxINIFileName, KNullDesC))
	{
		CDictionaryFileStore* MyDStore = CDictionaryFileStore::OpenLC(CCoeEnv::Static()->FsSession(),AufFolder.File(), TUid::Uid(0x102013AD));
		
		TUint32 TmpValue(0);

		GetValuesL(MyDStore,0x7777,TmpValue);
		
		if(TmpValue > 50)
			iGridIsOn = ETrue;
		else 
			iGridIsOn  = EFalse;
		
		CleanupStack::PopAndDestroy(1);// Store
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::GetValuesL(CDictionaryFileStore* aDStore,TInt aUID,TUint32& aValue)
{
	if(aDStore == NULL)
		return;
		
	TUid FileUid = {0x10};
	FileUid.iUid = aUID;

	if(aDStore->IsPresentL(FileUid))
	{
		RDictionaryReadStream in;
		in.OpenLC(*aDStore,FileUid);
		aValue = in.ReadInt32L();
		CleanupStack::PopAndDestroy(1);// in
	}
	else
		aValue = 0;
}

/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
void CMainContainer::SaveValuesL(void)
{
	TFindFile AufFolder(CCoeEnv::Static()->FsSession());
	if(KErrNone == AufFolder.FindByDir(KtxINIFileName, KNullDesC))
	{
		User::LeaveIfError(CCoeEnv::Static()->FsSession().Delete(AufFolder.File()));

		CDictionaryFileStore* MyDStore = CDictionaryFileStore::OpenLC(CCoeEnv::Static()->FsSession(),AufFolder.File(), TUid::Uid(0x102013AD));
				
		if(iGridIsOn)
			SaveValuesL(MyDStore,0x7777,0x100);	
		else
			SaveValuesL(MyDStore,0x7777,0x000);

		MyDStore->CommitL();
		CleanupStack::PopAndDestroy(1);// Store
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::SaveValuesL(CDictionaryFileStore* aDStore,TInt aUID, TUint32 aValue)
{
	if(aDStore == NULL)
		return;
		
	TUid FileUid = {0x10};
	FileUid.iUid = aUID;

	if(aDStore->IsPresentL(FileUid))
	{
		aDStore->Remove(FileUid);
		aDStore->CommitL();
	}
			
	RDictionaryWriteStream out2;
	out2.AssignLC(*aDStore,FileUid);
	out2.WriteInt32L(aValue);
	out2.CommitL(); 	
	CleanupStack::PopAndDestroy(1);// out2
}

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CMainContainer::MakeListBoxL()
{
	TInt MySetIndex(0);
	
	if(iMyListBox)
	{
		MySetIndex = HandleViewSelectedIndex();
	}
    
	delete iMyListBox;
	iMyListBox = NULL;
	delete iProfileBox;
	iProfileBox = NULL;

    iMyListBox   = new( ELeave ) CAknDoubleLargeStyleListBox();
	iMyListBox->ConstructL(this,EAknListBoxSelectionList);

	CArrayPtr<CGulIcon>* icons =new( ELeave ) CAknIconArray(1);
	CleanupStack::PushL(icons);
	
	iMyListBox->Model()->SetItemTextArray(GetProfilesArrayL(icons,TSize(0,0)));
   	iMyListBox->Model()->SetOwnershipType(ELbmOwnsItemArray);
    
    CleanupStack::Pop(icons);
	iMyListBox->ItemDrawer()->ColumnData()->SetIconArray(icons);
	
	iMyListBox->CreateScrollBarFrameL( ETrue );
    iMyListBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
	iMyListBox->SetRect(Rect());
	
//	iMyListBox->View()->SetListEmptyTextL(KtxNoData);

	iMyListBox->ActivateL();

	TInt ItemsCount = iMyListBox->Model()->ItemTextArray()->MdcaCount();
	
	if(ItemsCount > MySetIndex && MySetIndex >= 0)
		iMyListBox->SetCurrentItemIndex(MySetIndex);
	else if(ItemsCount > 0)
		iMyListBox->SetCurrentItemIndex(0);
		
	UpdateScrollBar(iMyListBox);
	iMyListBox->DrawNow();
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
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
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/

void CMainContainer::MakeGridBoxL()
{
	TInt MySetIndex(0);
	
	if(iProfileBox)
	{
		MySetIndex = HandleViewSelectedIndex();
	}

	delete iProfileBox;
	iProfileBox = NULL;
	delete iMyListBox;
	iMyListBox = NULL;

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

	if(iMyHelpContainer)
	{
		Ret = iMyHelpContainer->OfferKeyEventL(aKeyEvent,aEventCode);
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
			if(iMyListBox)
			{
				Ret = iMyListBox->OfferKeyEventL(aKeyEvent,aEventCode);
			}
			else if(iProfileBox)
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
		if(iGridIsOn)
			MakeGridBoxL();	
		else
			MakeListBoxL();
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
	if(iGridIsOn)
		MakeGridBoxL();	
	else
		MakeListBoxL();
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

TInt CMainContainer::HandleViewSelectedIndex(void)
{
	TInt ret(-1);

	if(iProfileBox){
		ret = iProfileBox->CurrentItemIndex();
	}else if(iMyListBox){
		ret = iMyListBox->CurrentItemIndex();
	}
	
	return ret;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/

void CMainContainer::SetGridOnL(const TBool& aGridOn)
{
	iGridIsOn = aGridOn;

	if(iGridIsOn){
		MakeGridBoxL();	
	}else{
		MakeListBoxL();
	}
	
	SaveValuesL();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/

void CMainContainer::HandleViewCommandL(TInt aCommand)
{
	switch(aCommand)
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
	case ESwitchList:
		{
			SetGridOnL(EFalse);
		}
		SetMenuL();
		DrawNow();
		break;
	case ESwitchGrid:
		{
			SetGridOnL(ETrue);
		}
		SetMenuL();
		DrawNow();
		break;
	case EAppOpen:
		if(!iYTasksContainer && iEcomHandler )
		{
			RPointerArray<CHandlerItem> Handlers = iEcomHandler->HandlerArray();
		
			TInt CurrInd = HandleViewSelectedIndex();
			if(CurrInd >= 0 && CurrInd < Handlers.Count())
			{
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
CCoeControl* CMainContainer::ComponentControl( TInt /*aIndex*/) const
{
	if(iMyHelpContainer)
	{
		return iMyHelpContainer;		
	}
	else if(iYTasksContainer)
	{
		return iYTasksContainer;	
	}
	else if(iMyListBox)
	{
		return iMyListBox;
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
	if(iMyHelpContainer)
	{
		return 1;
	}
	else if(iYTasksContainer)
	{
		return 1;
	}
	else if(iMyListBox)
	{
		return 1;
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
		if(iMyHelpContainer)
		{
			iCba->SetCommandSetL(R_APPHELP_CBA);
			iCba->DrawDeferred();
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
		
		TInt CurrInd = HandleViewSelectedIndex();
		if(CurrInd >= 0)
		{
			Dimopen = EFalse;		
		}
		
		if(Dimopen)
		{
			aMenuPane->SetItemDimmed(EAppOpen,ETrue);
		}
		
		if(iProfileBox)
			aMenuPane->SetItemDimmed(ESwitchGrid,ETrue);
		else
			aMenuPane->SetItemDimmed(ESwitchList,ETrue);
	}
	
	if(iYTasksContainer)
	{
		iYTasksContainer->InitMenuPanelL(aResourceId, aMenuPane);
	}
}



